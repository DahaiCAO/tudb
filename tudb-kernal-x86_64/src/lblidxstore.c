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

#include "lblidxstore.h";

// read one label index page
// start the start pointer in memory
// start_no the start record id in this page
// lbl_idx_db_fp label index DB file
lbl_idx_page_t* readOneLabelIndexPage(lbl_tkn_page_t *pages, long long start,
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

lbl_idx_t* insertLabelIndex(long long ta_id, long long tknId, int length,
		int codingtype) {
	lbl_idx_t *idx = (lbl_idx_t*) malloc(sizeof(lbl_idx_t*));
	idx->id = NULL_POINTER;
	idx->codingType = codingtype;
	idx->lblCount = 1;
	idx->lblTknId = tknId;
	idx->length = length;
	idx->taId = ta_id;
	idx->page = NULL;
	return idx;
}

// search one Id in all memory pages
lbl_idx_page_t* findLabelIndexPage(long long id) {
	lbl_idx_page_t *page = lbl_idx_pages;
	if (page != NULL) {
		while (page != NULL) {
			long long pos = 16LL + id * (3 * LONG_LONG + 1);// ?
			if (pos >= page->start && pos < page->end) {
				return page;
			} else {
				page = page->nxtpage;
			}
		}
	}
	return NULL;
}

// search one page that contain the specified Id.
lbl_idx_page_t* searchLabelIndexPage(long long id, int recordbytes, long long startbyte,
		int pagererecords, int pagebytes, FILE *tadbfp) {
	lbl_idx_page_t *nxtp = findLabelIndexPage(id);
	if (nxtp == NULL) {
		long long pagenum = (id * recordbytes) / pagebytes;
		long long start = pagenum * pagebytes + startbyte;
		nxtp = readOneLabelIndexPage(start, pagenum * pagererecords, tadbfp);
	}
	return nxtp;
}

void commitLabelIndex(lbl_idx_t *idx, FILE *lbl_idx_db_fp, FILE *lbl_idx_id_fp) {
	idx->id = getOneId(lbl_idx_id_fp, caches->lblidxIds, LABEL_ID_QUEUE_LENGTH);

}
