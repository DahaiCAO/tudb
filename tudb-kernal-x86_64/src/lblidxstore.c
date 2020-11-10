/**
 * Copyright 2020 Dahai Cao
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * lblidxstore.c
 *
 * Created on: 2020年8月14日
 * Author: Dahai CAO
 */

#include "lblidxstore.h"

// read one label index page
// start the start pointer in memory
// start_no the start record id in this page
// lbl_idx_db_fp label index DB file
static lbl_idx_page_t* readOneLabelIndexPage(lbl_idx_page_t *pages, long long start,
		long long start_no, FILE *lbl_idx_db_fp) {
	unsigned char *page = (unsigned char*) malloc(
			sizeof(unsigned char) * lbl_idx_page_bytes);
	memset(page, start_no, sizeof(unsigned char) * lbl_idx_page_bytes);
	fseek(lbl_idx_db_fp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(lbl_idx_db_fp)) != EOF) {
		fseek(lbl_idx_db_fp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), lbl_idx_page_bytes, lbl_idx_db_fp); // read one page
	}

	lbl_idx_page_t *p = (lbl_idx_page_t*) malloc(sizeof(lbl_idx_page_t));
	p->dirty = 0;
	p->expiretime = LABEL_INDEX_PAGE_EXPIRE_TIME; // 10 minutes, by default
	p->startNo = start_no;
	p->hit = 0;
	p->nxtpage = NULL;
	p->prvpage = NULL;
	p->content = page;
	p->start = start;
	p->end = start + lbl_idx_page_bytes;

	lbl_idx_page_t *pp = pages;
	if (pp != NULL) {
		while (pp->nxtpage != NULL) {
			pp = pp->nxtpage;
		}
		pp->nxtpage = p;
		p->prvpage = pp;
	} else {
		lbl_idx_pages = p;
	}
	return p;
}

void initLabelIndexDBMemPages(lbl_idx_page_t *pages, FILE *lbl_idx_db_fp) {
	// read the first page from 0 pointer.
	readOneLabelIndexPage(pages, 0LL, 0LL, lbl_idx_db_fp);
}

lbl_idx_t* insertLabelIndex(long long ta_id, long long firstBlkId, int length) {
	lbl_idx_t *idx = (lbl_idx_t*) malloc(sizeof(lbl_idx_t));
	idx->id = NULL_POINTER;
	idx->useCount = 1;
	idx->lblBlkId = firstBlkId;
	idx->length = length;
	idx->taId = ta_id;
	idx->page = NULL;
	return idx;
}

long long commitLabelIndex(lbl_idx_t *idx, FILE *lbl_idx_db_fp,
		FILE *lbl_idx_id_fp) {
	long long id = getOneId(lbl_idx_id_fp, caches->lblIdxIds,
			LABEL_ID_QUEUE_LENGTH);
	idx->id = id;
	lbl_idx_page_t *ps = lbl_idx_pages;
	bool found = false;
	unsigned char *pos;
	while (!found) {
		while (ps != NULL) {
			if (ps->startNo <= idx->id
					&& idx->id < ps->startNo + LABEL_INDEX_PAGE_RECORDS) {
				pos = ps->content
						+ (idx->id - ps->startNo) * lbl_idx_record_bytes;
				unsigned char ta_ids[LONG_LONG] = { 0 };
				longToByteArray(idx->taId, ta_ids);			// ta Id
				unsigned char length[LONG] = { 0 };
				integer2Bytes(idx->length, length); // length
				unsigned char count[LONG_LONG] = { 0 };
				longToByteArray(idx->useCount, count); // label counting
				unsigned char blkIds[LONG_LONG] = { 0 };
				longToByteArray(idx->lblBlkId, blkIds);
				memcpy(pos, ta_ids, LONG_LONG);
				memcpy(pos + LONG_LONG, blkIds, LONG_LONG);
				memcpy(pos + LONG_LONG + LONG_LONG, length, LONG);
				memcpy(pos + LONG_LONG + LONG_LONG + LONG, count,
				LONG_LONG);
				fseek(lbl_idx_db_fp, idx->id * lbl_idx_record_bytes,
				SEEK_SET); //
				fwrite(pos, sizeof(unsigned char), lbl_idx_record_bytes,
						lbl_idx_db_fp);
				found = true;
				pos = NULL;
				break;
			}
			ps = ps->nxtpage;
		}
		if (!found) {
			// read a new page
			long long pagenum = (idx->id * lbl_idx_record_bytes)
					/ lbl_idx_page_bytes;
			readOneLabelIndexPage(lbl_idx_pages, pagenum * lbl_idx_page_bytes,
					pagenum * LABEL_INDEX_PAGE_RECORDS, lbl_idx_db_fp);
			continue;
		} else {
			ps = NULL;
			break;
		}
	}
	idx->page = NULL;
	free(idx);
	return id;
}

void deallocLabelIndexPages(lbl_idx_page_t *pages) {
	lbl_idx_page_t *p;
	while (pages) {
		p = pages;
		pages = pages->nxtpage;
		free(p->content);
		p->prvpage = NULL;
		p->nxtpage = NULL;
		free(p);
		p = NULL;
	}
	pages = NULL;
}
