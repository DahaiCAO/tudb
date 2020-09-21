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

lbls_t** insertLabels(long long ta_id, long long * lblIdxIds) {
	int count = 0;
	long long *p = lblIdxIds;
	while (*p) {
		count++;
	}
	lbls_t **lbls = (lbls_t**) calloc(count, sizeof(lbls_t*));
	for (int i=0;i<count;i++) {
		lbls_t *idx = (lbls_t*) malloc(sizeof(lbls_t));
		idx->id = NULL_POINTER;

		idx->taId = ta_id;
		idx->page = NULL;
		*(lbls+i)=idx;
	}
	return lbls;
}
