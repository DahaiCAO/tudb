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
#include <locale.h>

#include "convert.h"
#include "macrodef.h"
#include "taidstore.h"
//#include "structepdef.h"
#include "lblidxstore.h"
#include "lbltknstore.h"
#include "init.h"

/*
 * main.c
 *
 * Created on: 2020年7月20日
 * Author: Dahai CAO
 */
// testing program for B tree for time axis DB
//int main(int argv, char **argc) {
//	setvbuf(stdout, NULL, _IONBF, 0);
//	char *taid = "D:/tudata/tustore.timeaxis.tdb.id";
//	FILE *taidfp = fopen(taid, "rb+");
//	char *tadb = "D:/tudata/tustore.timeaxis.tdb";
//	FILE *tadbfp = fopen(tadb, "rb+");
//
//	initIdDB(taid);
//	initTimeAxisDB(tadb);
//
//	caches = (id_cache_t*) malloc(sizeof(id_cache_t));
//	caches->taIds->nId = NULL;
//	caches->taIds->rId = NULL;
//
//	loadIds(taidfp);
//	listAllTaIds();
//	//fseek(tadbfp, 0, SEEK_SET); //
//	//fread(p, sizeof(unsigned char), LONG_LONG * 2, tadbfp);
//	ta_btree_t **_btree = (ta_btree_t**) calloc(1, sizeof(ta_btree_t));
//	btree_create(_btree, 9, NULL);
//	btree_insert(*_btree, 29); //1
//	btree_insert(*_btree, 40); //2
//	btree_insert(*_btree, 22); //3
//	btree_insert(*_btree, 32); //3
//	btree_insert(*_btree, 59); //4
//	btree_insert(*_btree, 99); //5
//	btree_insert(*_btree, 72); //6
//	btree_insert(*_btree, 8); //7
//	btree_insert(*_btree, 37); //8
//	btree_insert(*_btree, 58); //9
//	btree_insert(*_btree, 78); //10
//	btree_insert(*_btree, 10); //11
//	btree_insert(*_btree, 20); //12
//	btree_insert(*_btree, 48); //13
//	btree_insert(*_btree, 43); //14
//}

// test time axis DB
//int main(int argv, char **argc) {
//	setvbuf(stdout, NULL, _IONBF, 0);
//	const char *path = "D:/tudata/";
//	char *taid;
//	strcat(taid, path);
//	strcat(taid, "tustore.timeaxis.tdb.id");
//	FILE *taidfp = fopen(taid, "rb+");
//
//	char *tadb;
//	strcat(tadb, path);
//	strcat(tadb, "tustore.timeaxis.tdb");
//	FILE *tadbfp = fopen(tadb, "rb+");
//
//	initIdDB(taid);
//	initTimeAxisDB(tadb);
//	// initialize
//	caches = (id_caches_t*) malloc(sizeof(id_caches_t));
//	initIdCaches(caches);
//	loadIds(taidfp, caches->taIds);
//	loadIds(taidfp, caches->teIds);
//	listAllTaIds(caches->taIds);
//	listAllTaIds(caches->teIds);
//	timeaxispages = (ta_buf_t*) malloc(sizeof(ta_buf_t));
//	timeaxispages->pages = NULL;
//	initTaDBMemPages(timeaxispages, tadbfp);
//
//	// insert 13 time stamp sequentially
//	//1593783935
//	//1593783957
//	long long ts = 1593783935;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts, taidfp, tadbfp);
//	//showAllPages();
//	long long ts1 = 1593783957;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts1, taidfp, tadbfp);
//	//showAllPages();
//	long long ts2 = 1593783958;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts2, taidfp, tadbfp);
//	//showAllPages();
//	long long ts3 = 1593783959;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts3, taidfp, tadbfp);
//	//showAllPages();
//	long long ts4 = 1593783960;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts4, taidfp, tadbfp);
//	long long ts5 = 1593783963;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts5, taidfp, tadbfp);
//	long long ts6 = 1593783964;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts6, taidfp, tadbfp);
//	long long ts7 = 1593783965;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts7, taidfp, tadbfp);
//	long long ts8 = 1593783969;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts8, taidfp, tadbfp);
//	long long ts9 = 1593783970;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts9, taidfp, tadbfp);
//	showAllPages();
//	long long ts10 = 1593783972;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts10, taidfp, tadbfp);
//	showAllPages();
//	long long ts11 = 1593783974;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts11, taidfp, tadbfp);
//	showAllPages();
//
//	long long ts12 = 1593783975;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts12, taidfp, tadbfp);
//
//	long long ts13 = 1593783977;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts13, taidfp, tadbfp);
//	showAllPages();
//
//	// insert 2 time stamps before the first time stamp
//	long long ts14 = 1593783932;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts14, taidfp, tadbfp);
//	showAllPages();
//	long long ts15 = 1593783928;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts15, taidfp, tadbfp);
//	showAllPages();
//
//	// insert 2 time stamps in the middle
//	long long ts16 = 1593783961;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts16, taidfp, tadbfp);
//	showAllPages();
//	long long ts17 = 1593783973;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts17, taidfp, tadbfp);
//	showAllPages();
//
//	long long ts18 = 1593783976;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts18, taidfp, tadbfp);
//	showAllPages();
//
//	long long ts19 = 1593783931;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts19, taidfp, tadbfp);
//	showAllPages();
//
//	//
//	long long ts20 = 1593783931;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts20, taidfp, tadbfp);
//	showAllPages();
//
//	long long ts21 = 1593783977;	//(unsigned long) time(NULL);
//	insertEvolvedPoint(ts21, taidfp, tadbfp);
//	showAllPages();
//
//	// delete evolved point
//	long long ts22 = 1593783972;	//(unsigned long) time(NULL);
//	deleteEvolvedPoint(ts22, taidfp, tadbfp);
//	showAllPages();
//
//	listAllTaIds(caches->taIds);
//
//	long long ts23 = 1593783961;
//	long long id1 = queryEvolvedPoint(ts23, taidfp, tadbfp);
//	printf("--%lld\n", id1);
//
//	long long mints = 1593783969;
//	long long maxts = 1593783974;
//	idbuf_t *buf = (idbuf_t*) malloc(sizeof(idbuf_t));
//	buf->id = NULL_POINTER;
//	buf->nxt = NULL;
//	queryEvolvedPoints(mints, maxts, buf, taidfp, tadbfp);
//
//	fclose(taidfp);
//	fclose(tadbfp);
//
//	free(timeaxispages);
//	free(caches);
//
//}

// test label token store
int main(int argv, char **argc) {
	//setlocale(LC_ALL, "");
	setvbuf(stdout, NULL, _IONBF, 0);
	const char *d_path = "D:/tudata/";
//	char *taid;
//	strcat(taid, path);
//	strcat(taid,"tustore.timeaxis.tdb.id");
//	FILE *taidfp = fopen(taid, "rb+");
//
//	char *tadb;
//	strcat(tadb, path);
//	strcat(tadb, "tustore.timeaxis.tdb");
//	FILE *tadbfp = fopen(tadb, "rb+");
//
//	char *teid;
//	strcat(teid, path);
//	strcat(teid, "tustore.element.tdb.id");
//	FILE *teidfp = fopen(teid, "rb+");
//
//	char *tedb;
//	strcat(tedb, path);
//	strcat(tedb, "tustore.element.tdb");
//	FILE *tedbfp = fopen(tedb, "rb+");
//
//	char *labelsid;
//	strcat(labelsid, path);
//	strcat(labelsid, "tustore.element.tdb.labels.id");
//	FILE *labelsidfp = fopen(labelsid, "rb+");
//
//	char *labels;
//	strcat(labels, path);
//	strcat(labels, "tustore.element.tdb.labels");
//	FILE *labelsfp = fopen(labels, "rb+");

//	char *lbl_idx_id_path;
//	strcat(lbl_idx_id_path, d_path);
//	strcat(lbl_idx_id_path, "tustore.element.tdb.labelindex.id");
//	FILE *lbl_idx_id_fp = fopen(lbl_idx_id_path, "rb+");
//
//	char *lbl_idx_path;
//	strcat(lbl_idx_path, d_path);
//	strcat(lbl_idx_path, "tustore.element.tdb.labelindex");
//	FILE *lbl_idx_fp = fopen(lbl_idx_path, "rb+");

	char *lbl_tkn_id_path = (char*) calloc(256, sizeof(char));
	strcat(lbl_tkn_id_path, d_path);
	strcat(lbl_tkn_id_path, "tustore.element.tdb.labeltoken.id");
	FILE *lbl_tkn_id_fp = fopen(lbl_tkn_id_path, "rb+");

	char *lbl_tkn_path = (char*) calloc(256, sizeof(char));
	strcat(lbl_tkn_path, d_path);
	strcat(lbl_tkn_path, "tustore.element.tdb.labeltoken");
	FILE *lbl_tkn_fp = fopen(lbl_tkn_path, "rb+");

//	initIdDB(lbl_idx_id_path);
	initIdDB(lbl_tkn_id_path);
	// initTimeAxisDB(tadb);
	// initialize
	caches = (id_caches_t*) malloc(sizeof(id_caches_t));
	initIdCaches(caches);

	//loadIds(taidfp, caches->taIds);
	//loadIds(taidfp, caches->teIds);
	//loadIds(labelindexidfp, caches->lblidxIds);
	loadIds(lbl_tkn_fp, caches->lbltknIds);

	//listAllTaIds(caches->taIds);
	//listAllTaIds(caches->teIds);
	//listAllTaIds(caches->lblidxIds);
	listAllTaIds(caches->lbltknIds);

	lbl_tkn_pages = (lbl_tkn_page_t*) malloc(sizeof(lbl_tkn_page_t));
	initLabelTokenDBMemPages(lbl_tkn_pages, lbl_tkn_fp);
	char label[] = "Microsoft corporation 美国微软公司出品 版权所有";
	char *l = label;
	long long tknId = insertLabelToken(10, label, lbl_tkn_id_fp, lbl_tkn_fp);
	fclose(lbl_tkn_id_fp);
	fclose(lbl_tkn_fp);
	free(lbl_tkn_pages);
	free(caches);

}

/*
 // test time axis DB Id
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


