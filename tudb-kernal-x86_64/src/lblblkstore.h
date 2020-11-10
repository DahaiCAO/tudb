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

#include "utf8convert.h"
#include "tuidstore.h"
#include "macrodef.h"
#include "structlblblkdef.h"
/*
 * lblblkstore.h
 *
 * Created on: 2020年8月12日
 * Author: Dahai CAO
 */

#ifndef LBLBLKSTORE_H_
#define LBLBLKSTORE_H_

// label block pages by default.
lbl_blk_page_t *lbl_blk_pages;
// bytes in one label block record
size_t lbl_blk_record_bytes;
// records in one page with label block records, configurable in .conf file
size_t LABEL_BLOCK_PAGE_RECORDS;
// bytes in one page with label block records
size_t lbl_blk_page_bytes;
// label block length, configurable in .conf file
size_t LABEL_BLOCK_LENGTH;
// label buffer length, configurable in .conf file
size_t LABEL_BUFFER_LENGTH;
// label block page's expire time in memory
int LABEL_BLOCK_PAGE_EXPIRE_TIME;

void initLabelBlockDBMemPages(lbl_blk_page_t *pages, FILE *lbl_blk_db_fp);

void commitLabelBlocks(long long ta_id, lbl_blk_t **list, FILE *lbl_blk_db_fp,
		FILE *lbl_blk_id_fp);

unsigned char* findLabelBlock(long long id, FILE *lbl_blk_db_fp);

lbl_blk_t** searchLabelBlockList(long long id, FILE *lbl_blk_db_fp);

lbl_blk_t** divideLabelBlocks(unsigned char *label);

void deleteLabelBlocks(long long id, FILE *lbl_blk_id_fp, FILE *lbl_blk_db_fp);

void commitUpdateLabelBlock(lbl_blk_t **list, lbl_blk_t **newlist,
		FILE *lbl_blk_id_fp, FILE *lbl_blk_db_fp);

void combineLabelBlocks(lbl_blk_t **list, int length);

void deallocLabelBlockList(lbl_blk_t **list);

void deallocLabelBlockPages(lbl_blk_page_t *pages);

#endif /* LBLBLKSTORE_H_ */
