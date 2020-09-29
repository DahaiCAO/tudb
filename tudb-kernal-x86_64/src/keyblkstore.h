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

#include "utf8convert.h"
#include "tuidstore.h"
#include "macrodef.h"
#include "convert.h"
#include "structkeyblkdef.h"
/*
 * keytknstore.h
 *
 * Created on: 2020年9月27日
 * Author: Dahai CAO
 */

#ifndef KEYTKNSTORE_H_
#define KEYTKNSTORE_H_

key_blk_page_t *key_blk_pages;
// bytes in one key block record
size_t key_blk_record_bytes;
// records in one page with key block records, configurable in .conf file
size_t KEY_BLOCK_PAGE_RECORDS;
// bytes in one page with key block records
size_t key_blk_page_bytes;
// key block length, configurable in .conf file
size_t KEY_BLOCK_LENGTH;
// key buffer length, configurable in .conf file
size_t KEY_BUFFER_LENGTH;
// key block page's expire time in memory
int KEY_BLOCK_PAGE_EXPIRE_TIME;

void initKeyBlockDBMemPages(key_blk_page_t *pages, FILE *key_blk_db_fp);

void commitKeyBlocks(long long ta_id, key_blk_t **list, FILE *key_blk_db_fp,
		FILE *key_blk_id_fp);

key_blk_t** divideKeyBlocks(unsigned char *key);

key_blk_t** searchKeyBlockList(long long id, FILE *key_blk_db_fp);

unsigned char* findKey(long long id, FILE *key_blk_db_fp);

void deleteKey(long long id, FILE *key_blk_db_fp);

// this update operation is used to update and override new key to old key
void commitUpdateKey(key_blk_t **list, key_blk_t **newlist, FILE *key_blk_id_fp,
		FILE *key_blk_db_fp);

void combineKeyBlocks(key_blk_t **list, int length);

// deallocate key block list
void deallocKeyBlockList(key_blk_t **list);

// deallocate key block memory pages
void deallocKeyBlockPages(key_blk_page_t *pages);

#endif /* KEYTKNSTORE_H_ */
