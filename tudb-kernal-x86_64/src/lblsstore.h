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
#include <iconv.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "tuidstore.h"
#include "structlblsdef.h"
#include "macrodef.h"
/*
 * lblsstore.h
 *
 * Created on: 2020年9月20日
 * Author: Dahai CAO
 */
#ifndef LBLSSTORE_H_
#define LBLSSTORE_H_

// labels pages by default.
lbls_page_t *lbls_pages;
// bytes in one labels element record
size_t lbls_record_bytes;
// records in one page with labels records, configurable in .conf file
size_t LABELS_PAGE_RECORDS;
// bytes in one page with labels records
size_t lbls_page_bytes;
// labels page's expire time in memory
int LABELS_PAGE_EXPIRE_TIME;

lbls_page_t* readOneLabelsPage(lbls_page_t *pages, long long start,
		long long start_no, FILE *lbls_db_fp);

void initLabelsDBMemPages(lbls_page_t *pages, FILE *lbls_db_fp);

lbls_t** insertLabels(long long *lblIdxIds);

long long commitLabels(long long ta_id, lbls_t **labels, FILE *lbls_db_fp,
		FILE *lbls_id_fp);

#endif /* LBLSSTORE_H_ */
