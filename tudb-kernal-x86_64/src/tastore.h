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

#include "convert.h"
#include "macrodef.h"
#include "tuidstore.h"
#include "structepdef.h"
/*
 * text.h
 *
 * Created on: 2020年6月11日
 * Author: Dahai CAO
 */

#ifndef TASTORE_H_
#define TASTORE_H_

// time axis memory pages
ta_buf_t *tm_axis_pages;
// time axis record byte array length
size_t tm_axis_record_bytes;
// time axis memory page records, configurable in .conf file
size_t TIME_AXIS_PAGE_RECORDS;
// time axis page byte array length
size_t tm_axis_page_bytes;
// start points in database of time axis
long long tm_axis_db_start_byte;
// time axis page expiration time (minutes), configurable in .conf file
int TIME_AXIS_PAGE_EXPIRE_TIME;

// read one time axis page
ta_page_t* readOnePage(long long start, long long startNo, FILE *tadbfp);

void initTaDBMemPages(ta_buf_t *pages, FILE *tadbfp);

void EpToByteArray(evolved_point_t *ep, int len, unsigned char buffer[]);

unsigned char parseInUse(unsigned char *p);

long long parsePrvId(unsigned char *p);

long long parseNxtId(unsigned char *p);

long long parseStamp(unsigned char *p);

void putInUse(evolved_point_t *ep);

void putPrvId(evolved_point_t *ep);

void putNxtId(evolved_point_t *ep);

void putStamp(evolved_point_t *ep);
// search one Id in all memory pages
ta_page_t* findPage(long long id);
// show all records in the memory pages
void showAllPages();
// search one page that contain the specified Id.
ta_page_t* searchPage(long long id, int recordbytes, long long startbyte,
		int pagererecords, int pagebytes, FILE *tadbfp);
// query one evolved point by specified time stamp (ts)
void searchforQueryEP(long long ts, unsigned char *currpos, ta_page_t *currpage,
		evolved_point_t *tmp, evolved_point_t *previous, evolved_point_t *next,
		FILE *tadbfp);

// query one or more evolved points between specified minimum time stamp (mints)
// maximum time stamp (maxts), mints should be less than maxts.
void searchforQueryBetween(long long mints, long long maxts, idbuf_t *buf,
		unsigned char *currpos, ta_page_t *currpage, evolved_point_t *tmp,
		evolved_point_t *previous, evolved_point_t *next, FILE *tadbfp);

// search a time stamp in memory pages for inserting action
void searchforInsert(long long ts, unsigned char *currpos, ta_page_t *currpage,
		evolved_point_t *tmp, evolved_point_t *previous, evolved_point_t *next,
		tadb_head_t *h, FILE *tadbfp, FILE *taidfp);

void searchforDelete(long long ts, unsigned char *currpos, ta_page_t *currpage,
		evolved_point_t *tmp, evolved_point_t *previous, evolved_point_t *next,
		tadb_head_t *h, FILE *tadbfp);

long long commitInsert(long long ts, evolved_point_t *p, evolved_point_t *t,
		evolved_point_t *n, tadb_head_t *h, int recordbytes,
		long long startbyte, int pagererecords, int pagebytes, FILE *tadbfp);

long long commitDelete(long long ts, evolved_point_t *p, evolved_point_t *t,
		evolved_point_t *n, tadb_head_t *h, int recordbytes,
		long long startbyte, int pagererecords, int pagebytes, FILE *tadbfp);

void showAllTimeAxis(FILE *tadbfp);

// query one set of IDs between minimum time stamp and maximum time stamp
void queryEvolvedPoints(long long mints, long long maxts, idbuf_t *buf,
		FILE *taidfp, FILE *tadbfp);

// query one evolved point ID
long long queryEvolvedPoint(long long ts, FILE *taidfp, FILE *tadbfp);

// delete one evolved point
long long deleteEvolvedPoint(long long ts, FILE *taidfp, FILE *tadbfp);

long long insertEvolvedPoint(long long ts, FILE *ta_id_fp, FILE *ta_db_fp);

void deallocEvolvedPoint(evolved_point_t *p);

void deallocTimeAxisBuf(idbuf_t *head);

void deallocTimeAxisPages(ta_buf_t *pages);


#endif /* TEXT_H_ */
