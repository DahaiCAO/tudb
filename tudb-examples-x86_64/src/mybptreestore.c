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
#include "mybptreestore.h"
/*
 * mybptreestore.c
 *
 * Created on: 2020年10月17日
 * Author: Dahai CAO
 */

static bptree_idx_page_t* readOneBPTreePage(long long start,
		FILE *lbl_bptree_idx_db_fp) {
	BPLUSTREE_INDEX_PAGE_SIZE = M + M + 1 + 1;
	unsigned char *page = (unsigned char*) malloc(
			sizeof(unsigned char) * BPLUSTREE_INDEX_PAGE_SIZE);
	memset(page, 0, sizeof(unsigned char) * BPLUSTREE_INDEX_PAGE_SIZE);
	fseek(lbl_bptree_idx_db_fp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(lbl_bptree_idx_db_fp)) != EOF) {
		fseek(lbl_bptree_idx_db_fp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), BPLUSTREE_INDEX_PAGE_SIZE,
				lbl_bptree_idx_db_fp); // read one page
	}
	bptree_idx_page_t *p = (bptree_idx_page_t*) malloc(sizeof(bptree_idx_page_t));
	p->dirty = 0;
	p->expiretime = 10; // 10 minutes
	p->hit = 0;
	return p;
}


void initLabelIndexRootNode(FILE *lbl_bptree_idx_db_fp) {
	// read the first page from 0 pointer.
	lbl_bptree = readOneBPTreePage(0LL, lbl_bptree_idx_db_fp);
}
