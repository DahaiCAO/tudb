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

#include "structlbltkndef.h"
#include "macrodef.h"
/*
 * lbltknstore.h
 *
 * Created on: 2020年8月12日
 * Author: Dahai CAO
 */

#ifndef LBLTKNSTORE_H_
#define LBLTKNSTORE_H_

// label token pages by default.
lbl_tkn_page_t *lbl_tkn_pages;
// bytes in one label token record
int lbl_tkn_record_bytes;
// records in one page with label token records
int lbl_tkn_page_records;
// bytes in one page with label token records
int lbl_tkn_page_bytes;

lbl_tkn_page_t* readOneLabelTokenPage(lbl_tkn_page_t *pages, long long start, long long start_no,
		FILE *lbl_tkn_db_fp);

void initLabelTokenDBMemPages(lbl_tkn_page_t *pages, FILE *lbl_tkn_db_fp);

long long insertLabelToken(long long ta_id, char *label, FILE *lbl_tkn_id_fp,
		FILE *lbl_tkn_fp);

#endif /* LBLTKNSTORE_H_ */
