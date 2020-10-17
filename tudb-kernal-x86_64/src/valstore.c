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
#include "valstore.h"
/*
 * valstore.c
 *
 * Created on: 2020年10月7日
 * Author: Dahai CAO
 */

/*
 * read one value page
 * start the start pointer in memory
 * start_no the start record id in this page
 * val_db_fp label token DB file
 */
static val_page_t* readOneValuePage(val_page_t *pages,
		long long start, long long start_no, FILE *val_db_fp) {
	unsigned char *page = (unsigned char*) malloc(
			sizeof(unsigned char) * val_page_bytes);
	memset(page, 0, sizeof(unsigned char) * val_page_bytes);
	fseek(val_db_fp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(val_db_fp)) != EOF) {
		fseek(val_db_fp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), val_page_bytes, val_db_fp); // read one page
	}

	val_page_t *p = (val_page_t*) malloc(sizeof(val_page_t));
	p->dirty = 0;
	p->expiretime = VALUE_PAGE_EXPIRE_TIME; // 10 minutes
	p->startNo = start_no;
	p->hit = 0;
	p->nxtpage = NULL;
	p->prvpage = NULL;
	p->content = page;
	p->start = start;
	p->end = start + val_page_bytes;

	val_page_t *pp = pages;
	if (pp != NULL) {
		while (pp->nxtpage != NULL) {
			pp = pp->nxtpage;
		}
		pp->nxtpage = p;
		p->prvpage = pp;
		pp = NULL;
	} else {
		val_pages = p;
	}
	return p;
}

void initValueDBMemPages(val_page_t *pages, FILE *val_db_fp) {
	// read the first page from 0 pointer.
	readOneValuePage(pages, 0LL, 0LL, val_db_fp);
}



