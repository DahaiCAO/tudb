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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "structlblidxdef.h"


// read one label token page
// start the start pointer in memory
// start_no the start record id in this page
// lbl_tkn_db_fp label token DB file
/*
lbl_tkn_page_t* readOneLabelIndexPage(long long start, long long start_no,
		FILE *lbl_idx_db_fp) {
	lbl_tkn_page_bytes = lbl_tkn_record_bytes * lbl_tkn_page_records;
	unsigned char *page = (unsigned char*) malloc(
			sizeof(unsigned char) * lbl_tkn_page_bytes);
	memset(page, start_no, sizeof(unsigned char) * lbl_tkn_page_bytes);
	fseek(lbl_idx_db_fp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(lbl_idx_db_fp)) != EOF) {
		fseek(lbl_idx_db_fp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), lbl_tkn_page_bytes, lbl_idx_db_fp); // read one page
	}

	lbl_tkn_page_t *p = (lbl_tkn_page_t*) malloc(sizeof(lbl_tkn_page_t));
	p->dirty = 0;
	p->expiretime = expr_time; // 10 minutes
	p->startNo = start_no;
	p->hit = 0;
	p->nxtpage = NULL;
	p->prvpage = NULL;
	p->content = page;
	p->start = start;
	p->end = start + lbl_tkn_page_bytes;

	lbl_tkn_page_t *pp = lbl_tkn_pages;
	if (pp != NULL) {
		while (pp->nxtpage != NULL) {
			pp = pp->nxtpage;
		}
		pp->nxtpage = p;
		p->prvpage = pp;
	} else {
		lbl_tkn_pages = p;
	}
	return p;
}

*/
