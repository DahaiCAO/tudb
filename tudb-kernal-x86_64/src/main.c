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
#include "taidstore.h"
#include "structepdef.h"

/*
 * main.c
 *
 * Created on: 2020年7月20日
 * Author: Dahai CAO
 */
int main(int argv, char **argc) {
	setvbuf(stdout, NULL, _IONBF, 0);

	char *taid = "D:/tudata/tustore.timeaxis.tdb.id";
	FILE *taidfp = fopen(taid, "rb+");
	char *tadb = "D:/tudata/tustore.timeaxis.tdb";
	FILE *tadbfp = fopen(tadb, "rb+");

	char *teid = "D:/tudata/tustore.element.tdb.id";
	FILE *teidfp = fopen(teid, "rb+");
	char *tedb = "D:/tudata/tustore.element.tdb";
	FILE *tedbfp = fopen(tedb, "rb+");

	initIdDB(taid);
	initIdDB(teid);
// initTimeAxisDB(tadb);

// initialize
	cache = (id_cache_t*) malloc(sizeof(id_cache_t));
	cache->nId = NULL;
	cache->rId = NULL;

	loadIds(taidfp);
	listAllTaIds();

	timeaxispages = (ta_buf_t*) malloc(sizeof(ta_buf_t));
	timeaxispages->pages = NULL;
	unsigned char p[LONG_LONG * 2] = { 0 };
	unsigned char first[LONG_LONG] = { 0 };
	unsigned char last[LONG_LONG] = { 0 };
	fseek(tadbfp, 0, SEEK_SET); //
	fread(p, sizeof(unsigned char), LONG_LONG * 2, tadbfp);
	for (int i = 0; i < LONG_LONG; i++) {
		first[i] = p[i];
	}
	for (int i = LONG_LONG; i < 2 * LONG_LONG; i++) {
		last[i] = p[i];
	}
	long long firstId = ByteArrayToLong(first);
	long long lastId = ByteArrayToLong(last);
	timeaxispages->first = firstId;
	timeaxispages->last = lastId;
	if (timeaxispages->pages == NULL) {
		readOnePage(16LL, 0, tadbfp);
	}

	// insert 13 time stamp sequentially
	//1593783935
	//1593783957
	long long ts = 1593783935;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts, taidfp, tadbfp);
	//showAllPages();
	long long ts1 = 1593783957;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts1, taidfp, tadbfp);
	//showAllPages();
	long long ts2 = 1593783958;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts2, taidfp, tadbfp);
	//showAllPages();
	long long ts3 = 1593783959;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts3, taidfp, tadbfp);
	//showAllPages();
	long long ts4 = 1593783960;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts4, taidfp, tadbfp);
	long long ts5 = 1593783963;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts5, taidfp, tadbfp);
	long long ts6 = 1593783964;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts6, taidfp, tadbfp);
	long long ts7 = 1593783965;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts7, taidfp, tadbfp);
	long long ts8 = 1593783969;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts8, taidfp, tadbfp);
	long long ts9 = 1593783970;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts9, taidfp, tadbfp);
	showAllPages();
	long long ts10 = 1593783972;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts10, taidfp, tadbfp);
	showAllPages();
	long long ts11 = 1593783974;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts11, taidfp, tadbfp);
	showAllPages();

	long long ts12 = 1593783975;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts12, taidfp, tadbfp);

	long long ts13 = 1593783977;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts13, taidfp, tadbfp);
	showAllPages();

	// insert 2 time stamps before the first time stamp
	long long ts14 = 1593783932;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts14, taidfp, tadbfp);
	showAllPages();
	long long ts15 = 1593783928;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts15, taidfp, tadbfp);
	showAllPages();

	// insert 2 time stamps in the middle
	long long ts16 = 1593783961;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts16, taidfp, tadbfp);
	showAllPages();
	long long ts17 = 1593783973;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts17, taidfp, tadbfp);
	showAllPages();

	long long ts18 = 1593783976;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts18, taidfp, tadbfp);
	showAllPages();

	long long ts19 = 1593783931;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts19, taidfp, tadbfp);
	showAllPages();

	//
	long long ts20 = 1593783931;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts20, taidfp, tadbfp);
	showAllPages();

	long long ts21 = 1593783977;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts21, taidfp, tadbfp);
	showAllPages();

	// delete evolved point
	long long ts22 = 1593783972;	//(unsigned long) time(NULL);
	deleteEvolvedPoint(ts22, taidfp, tadbfp);
	showAllPages();

	listAllTaIds();

	long long ts23 = 1593783961;
	long long id1 = queryEvolvedPoint(ts23, taidfp, tadbfp);
	printf("--%lld\n", id1);

	long long mints = 1593783969;
	long long maxts = 1593783974;
	idbuf_t *buf = (idbuf_t*) malloc(sizeof(idbuf_t));
	buf->id = NULL_POINTER;
	buf->nxt = NULL;
	queryEvolvedPoints(mints, maxts, buf, taidfp, tadbfp);

	fclose(taidfp);
	fclose(tadbfp);

	free(timeaxispages);
	free(cache);

}

/*
 int main(int argv, char **argc) {
 setvbuf(stdout, NULL, _IONBF, 0);

 // get new Id from next free IDs
 char *taid = "D:/tudata/tustore.timeaxis.tdb.id";
 FILE *taidfp = fopen(taid, "rb+");

 // initialize
 cache = (id_cache_t*) malloc(sizeof(id_cache_t));
 cache->nId = NULL;
 cache->rId = NULL;


 loadIds(taidfp);

 listAllTaIds();

 recycleOneId(14);

 listAllTaIds();

 long long d = getOneId();
 printf("%lld\n", d);
 d = getOneId();
 printf("%lld\n", d);
 d = getOneId();
 printf("%lld\n", d);

 // test to get one id.
 //	long long id = getId(taidfp);
 //	 printf("%lld\n", id);
 //	 id = getId(taidfp);
 //	 printf("%lld\n", id);
 //	 id = getId(taidfp);
 //	 printf("%lld\n", id);
 //	 id = getId(taidfp);
 //	 printf("%lld\n", id);
 //	 id = getId(taidfp);
 //	 printf("%lld\n", id);

 // test recycle one id
 //	 recycleId(taidfp, 4);
 //	 recycleId(taidfp, 7);
 //	 recycleId(taidfp, 2);
 //	 recycleId(taidfp, 3);
 //	 recycleId(taidfp, 11);
 //	 recycleId(taidfp, 9);
 //	 recycleId(taidfp, 5);
 //	 recycleId(taidfp, 13);
 //	 recycleId(taidfp, 17);
 //	 recycleId(taidfp, 26);
 //
 //	readAllTaIds(taidfp);
 fclose(taidfp);

 free(cache);
 printf("End");

 }*/
