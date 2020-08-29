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

size_t LABEL_BLOCK_LENGTH;
size_t LABEL_BUFFER_LENGTH;

lbl_tkn_page_t* readOneLabelTokenPage(lbl_tkn_page_t *pages, long long start, long long start_no,
		FILE *lbl_tkn_db_fp);

void initLabelTokenDBMemPages(lbl_tkn_page_t *pages, FILE *lbl_tkn_db_fp);

int code_convert(char *from_charset, char *to_charset, char *inbuf,
		size_t inlen, char *outbuf, size_t outlen);

bool check_gb2312(const char *str, size_t length);

int u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

void convert2Utf8(char *fromstr, char *tostr, size_t length);

void commitLabelToken();

long long insertLabelToken(long long ta_id, char *label, lbl_tkn_t **list, FILE *lbl_tkn_id_fp,
		FILE *lbl_tkn_fp);
#endif /* LBLTKNSTORE_H_ */
