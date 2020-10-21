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
#include "lblsstore.h"
/*
 * lblsstore.c
 *
 * Created on: 2020年9月20日
 * Author: Dahai CAO
 */

// read one labels page
// start the start pointer in memory
// start_no the start record id in this page
// lbls_db_fp labels DB file
lbls_page_t* readOneLabelsPage(lbls_page_t *pages, long long start,
		long long start_no, FILE *lbls_db_fp) {
	unsigned char *page = (unsigned char*) malloc(
			sizeof(unsigned char) * lbls_page_bytes);
	memset(page, start_no, sizeof(unsigned char) * lbls_page_bytes);
	fseek(lbls_db_fp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(lbls_db_fp)) != EOF) {
		fseek(lbls_db_fp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), lbls_page_bytes, lbls_db_fp); // read one page
	}

	lbls_page_t *p = (lbls_page_t*) malloc(sizeof(lbls_page_t));
	p->dirty = 0;
	p->expiretime = LABELS_PAGE_EXPIRE_TIME; // 10 minutes, by default
	p->startNo = start_no;
	p->hit = 0;
	p->nxtpage = NULL;
	p->prvpage = NULL;
	p->content = page;
	p->start = start;
	p->end = start + lbls_page_bytes;

	lbls_page_t *pp = pages;
	if (pp != NULL) {
		while (pp->nxtpage != NULL) {
			pp = pp->nxtpage;
		}
		pp->nxtpage = p;
		p->prvpage = pp;
	} else {
		lbls_pages = p;
	}
	return p;
}

void initLabelsDBMemPages(lbls_page_t *pages, FILE *lbls_db_fp) {
	// read the first page from 0 pointer.
	readOneLabelsPage(pages, 0LL, 0LL, lbls_db_fp);
}

lbls_t** insertLabels(long long *lblIdxIds, int lblIdxLength) {
	lbls_t **labels = (lbls_t**) calloc(lblIdxLength + 1, sizeof(lbls_t*));
	for (int i = 0; i < lblIdxLength; i++) {
		lbls_t *lbls = (lbls_t*) malloc(sizeof(lbls_t));
		lbls->id = NULL_POINTER;
		lbls->inUse = 1;
		lbls->lblIdxId = lblIdxIds[i];
		lbls->prvLblsId = NULL_POINTER;
		lbls->nxtLblsId = NULL_POINTER;
		lbls->taId = 0;
		lbls->page = NULL;
		*(labels + i) = lbls;
	}
	return labels;
}

long long commitLabels(long long ta_id, lbls_t **labels, FILE *lbls_db_fp,
		FILE *lbls_id_fp) {
	long long id = 0;
	lbls_t **t;
	// assign a ID to every token block
	int j = 0;
	t = labels;
	while (*(t + j) != NULL) {
		(*(t + j))->id = getOneId(lbls_id_fp, caches->lblsIds,
				LABEL_ID_QUEUE_LENGTH);
		if (j > 0) {
			(*(t + j - 1))->nxtLblsId = (*(t + j))->id;
			(*(t + j))->prvLblsId = (*(t + j - 1))->id;
		} else {
			id = (*(t + j))->id;
		}
		j++;
	}
	t = labels;
	j = 0;
	// one by one to store label tokens
	while (*(t + j) != NULL) {
		// search a label token space for storing label token
		lbls_page_t *ps = lbls_pages;
		bool found = false;
		unsigned char *pos;
		while (!found) {
			while (ps != NULL) {
				if (ps->startNo <= (*(t + j))->id
						&& (*(t + j))->id < ps->startNo + LABELS_PAGE_RECORDS) {
					pos = ps->content
							+ ((*(t + j))->id - ps->startNo)
									* lbls_record_bytes;
					// convert label token block to byte array
					unsigned char ta_ids[LONG_LONG] = { 0 };
					longToByteArray(ta_id, ta_ids);			// ta Id
					unsigned char prvLblsId[LONG_LONG] = { 0 };
					longToByteArray((*(t + j))->prvLblsId, prvLblsId);// previous labels Id
					unsigned char nxtLblsId[LONG_LONG] = { 0 };
					longToByteArray((*(t + j))->nxtLblsId, nxtLblsId);// next labels Id
					unsigned char inuse[1] = { (*(t + j))->inUse };
					unsigned char lblidxId[LONG_LONG] = { 0 };
					longToByteArray((*(t + j))->lblIdxId, lblidxId);// label index Id
					memcpy(pos, ta_ids, LONG_LONG);
					memcpy(pos + LONG_LONG, inuse, 1LL);
					memcpy(pos + LONG_LONG + 1, nxtLblsId, LONG_LONG);
					memcpy(pos + LONG_LONG + 1 + LONG_LONG, prvLblsId,
					LONG_LONG);
					memcpy(pos + LONG_LONG + 1 + LONG_LONG + LONG_LONG,
							lblidxId, LONG_LONG);
					// update to DB
					fseek(lbls_db_fp, (*(t + j))->id * lbls_record_bytes,
					SEEK_SET); //
					fwrite(pos, sizeof(unsigned char), lbls_record_bytes,
							lbls_db_fp);
					found = true;
					pos = NULL;
					break;
				}
				ps = ps->nxtpage;
			}
			if (!found) {
				// read a new page
				long long pagenum = ((*(t + j))->id * lbls_record_bytes)
						/ lbls_page_bytes;
				readOneLabelsPage(lbls_pages, pagenum * lbls_page_bytes,
						pagenum * LABELS_PAGE_RECORDS, lbls_db_fp);
				continue;
			} else {
				ps = NULL;
				break;
			}
		}
		j++;
	}
	t = NULL;
	return id;
}

void deallocLabelsPages(lbls_page_t *pages) {
	lbls_page_t *p;
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
