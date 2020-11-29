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
#include <math.h>

#include "tuidstore.h"
#include "utf8convert.h"
#include "structkeyidxdef.h"
#include "macrodef.h"
/*
 * keyidxstore.h
 *
 * Created on: 2020年9月29日
 * Author: Dahai CAO
 */

#ifndef KEYIDXSTORE_H_
#define KEYIDXSTORE_H_

// key index pages by default.
key_idx_page_t *key_idx_pages;

// bytes in one key index record
size_t key_idx_bas_record_bytes;
size_t key_idx_chk_record_bytes;

// bytes in one page with key index records
size_t key_idx_bas_page_bytes;
size_t key_idx_chk_page_bytes;

// page length of the base array and the check array
size_t ARRAY_PAGE_SIZE;

void initKeyIndexDBMemPages(key_idx_page_t *pages, FILE *key_idx_bas_fp,
		FILE *key_idx_chk_fp);

void build(unsigned char *word, long long tuIdxId, FILE *key_idx_bas_fp,
		FILE *key_idx_chk_fp);

long long match(char *keyWord, FILE *key_idx_bas_fp, FILE *key_idx_chk_fp);

void deallocKeyIndexPages(key_idx_page_t *pages);

#endif /* KEYIDXSTORE_H_ */
