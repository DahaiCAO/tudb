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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#include "tuidstore.h"
#include "structlblidxdef.h"
#include "macrodef.h"
/*
 * lblidxstore.h
 *
 * Created on: 2020年8月15日
 * Author: Dahai CAO
 */

#ifndef LBLIDXSTORE_H_
#define LBLIDXSTORE_H_

// label index pages by default.
lbl_idx_page_t *lbl_idx_pages;
// bytes in one label index record
size_t lbl_idx_record_bytes;
// records in one page with label index records, configurable in .conf file
size_t LABEL_INDEX_PAGE_RECORDS;
// bytes in one page with label index records
size_t lbl_idx_page_bytes;
// label index page's expire time in memory
int LABEL_INDEX_PAGE_EXPIRE_TIME;

lbl_idx_page_t* readOneLabelIndexPage(lbl_idx_page_t *pages, long long start,
		long long start_no, FILE *lbl_idx_db_fp);

void initLabelIndexDBMemPages(lbl_idx_page_t *pages, FILE *lbl_idx_db_fp);

lbl_idx_t* insertLabelIndex(long long ta_id, long long tknId, int length,
		int codingtype);

long long commitLabelIndex(lbl_idx_t *idx, FILE *lbl_idx_db_fp,
		FILE *lbl_idx_id_fp);

void deallocLabelIndexPages(lbl_idx_page_t *pages);

#endif /* LBLIDXSTORE_H_ */
