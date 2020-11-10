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
#include "keyidxstore.h"
/*
 * keyidxstore.c
 *
 * Created on: 2020年9月29日
 * Author: Dahai CAO
 */

// read one key index page
// start the start pointer in memory
// start_no the start record id in this page
// key_idx_db_fp label index DB file
static key_idx_page_t* readOneKeyIndexPage(key_idx_page_t *pages, long long start,
		long long start_no, FILE *key_idx_db_fp) {
	unsigned char *page = (unsigned char*) malloc(
			sizeof(unsigned char) * key_idx_page_bytes);
	memset(page, start_no, sizeof(unsigned char) * key_idx_page_bytes);
	fseek(key_idx_db_fp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(key_idx_db_fp)) != EOF) {
		fseek(key_idx_db_fp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), key_idx_page_bytes, key_idx_db_fp); // read one page
	}

	key_idx_page_t *p = (key_idx_page_t*) malloc(sizeof(key_idx_page_t));
	p->dirty = 0;
	p->expiretime = KEY_INDEX_PAGE_EXPIRE_TIME; // 10 minutes, by default
	p->startNo = start_no;
	p->hit = 0;
	p->nxtpage = NULL;
	p->prvpage = NULL;
	p->content = page;
	p->start = start;
	p->end = start + key_idx_page_bytes;

	key_idx_page_t *pp = pages;
	if (pp != NULL) {
		while (pp->nxtpage != NULL) {
			pp = pp->nxtpage;
		}
		pp->nxtpage = p;
		p->prvpage = pp;
	} else {
		key_idx_pages = p;
	}
	return p;
}

void initKeyIndexDBMemPages(key_idx_page_t *pages, FILE *key_idx_db_fp) {
	// read the first page from 0 pointer.
	readOneKeyIndexPage(pages, 0LL, 0LL, key_idx_db_fp);
}

key_idx_t* insertKeyIndex(long long ta_id, long long firstBlkId, int length) {
	key_idx_t *idx = (key_idx_t*) malloc(sizeof(key_idx_t));
	idx->id = NULL_POINTER;
	idx->useCount = 1;
	idx->keyBlkId = firstBlkId;
	idx->length = length;
	idx->taId = ta_id;
	idx->page = NULL;
	return idx;
}

long long commitKeyIndex(key_idx_t *idx, FILE *key_idx_db_fp,
		FILE *key_idx_id_fp) {
	long long id = getOneId(key_idx_id_fp, caches->keyIdxIds,
			KEY_ID_QUEUE_LENGTH);
	idx->id = id;
	key_idx_page_t *ps = key_idx_pages;
	bool found = false;
	unsigned char *pos;
	while (!found) {
		while (ps != NULL) {
			if (ps->startNo <= idx->id
					&& idx->id < ps->startNo + KEY_INDEX_PAGE_RECORDS) {
				pos = ps->content
						+ (idx->id - ps->startNo) * key_idx_record_bytes;
				unsigned char ta_ids[LONG_LONG] = { 0 };
				longToByteArray(idx->taId, ta_ids);			// ta Id
				unsigned char length[LONG] = { 0 };
				integer2Bytes(idx->length, length); // length
				unsigned char count[LONG_LONG] = { 0 };
				longToByteArray(idx->useCount, count); // label counting
				unsigned char blkIds[LONG_LONG] = { 0 };
				longToByteArray(idx->keyBlkId, blkIds);
				memcpy(pos, ta_ids, LONG_LONG);
				memcpy(pos + LONG_LONG, blkIds, LONG_LONG);
				memcpy(pos + LONG_LONG + LONG_LONG, length, LONG);
				memcpy(pos + LONG_LONG + LONG_LONG + LONG, count,
				LONG_LONG);
				fseek(key_idx_db_fp, idx->id * key_idx_record_bytes,
				SEEK_SET); //
				fwrite(pos, sizeof(unsigned char), key_idx_record_bytes,
						key_idx_db_fp);
				found = true;
				pos = NULL;
				break;
			}
			ps = ps->nxtpage;
		}
		if (!found) {
			// read a new page
			long long pagenum = (idx->id * key_idx_record_bytes)
					/ key_idx_page_bytes;
			readOneKeyIndexPage(key_idx_pages, pagenum * key_idx_page_bytes,
					pagenum * KEY_INDEX_PAGE_RECORDS, key_idx_db_fp);
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

void deallocKeyIndexPages(key_idx_page_t *pages) {
	key_idx_page_t *p;
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
