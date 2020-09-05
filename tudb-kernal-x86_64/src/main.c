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
#include "lblidxstore.h"
#include "lbltknstore.h"
#include "init.h"
#include "tuidstore.h"

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

int cont_str(char *str) {
    int length = 0;
    while( *str++ != '\0' )
        length += 1;
    return length;
}

// test label token store
int main(int argv, char **argc) {
    setlocale(LC_ALL, "zh-CN.UTF-8");
	setvbuf(stdout, NULL, _IONBF, 0);
	const char *d_path = "D:/tudata/";
	ID_QUEUE_LENGTH = 25;
	LABEL_ID_QUEUE_LENGTH = 10;
	TIMEAXIS_ID_QUEUE_LENGTH = 10;
	LABEL_BLOCK_LENGTH = 64;
	LABEL_BUFFER_LENGTH = 256;
	LABEL_TOKEN_PAGE_RECORDS = 10;
	// label bytes length.
	lbl_tkn_record_bytes = LONG_LONG + 1 + LONG + LONG_LONG
			+ LABEL_BLOCK_LENGTH;
	lbl_tkn_page_bytes = lbl_tkn_record_bytes * LABEL_TOKEN_PAGE_RECORDS;

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

	char *lbl_tkn_path = (char*) calloc(256, sizeof(char));
	strcat(lbl_tkn_path, d_path);
	strcat(lbl_tkn_path, "tustore.element.tdb.labeltoken");

	// initialize
	caches = (id_caches_t*) malloc(sizeof(id_caches_t));
	initIdCaches(caches);
	//initIdDB(lbl_idx_id_path);
	initIdDB(lbl_tkn_id_path);
	// initTimeAxisDB(tadb);
	initDB(lbl_tkn_path);
	FILE *lbl_tkn_fp = fopen(lbl_tkn_path, "rb+");
	FILE *lbl_tkn_id_fp = fopen(lbl_tkn_id_path, "rb+");
	//loadAllIds(taidfp, caches->taIds);
	//loadAllIds(taidfp, caches->teIds);
	//loadAllIds(labelindexidfp, caches->lblidxIds);
	loadAllIds(lbl_tkn_id_fp, caches->lbltknIds, LABEL_ID_QUEUE_LENGTH);

	//listAllIds(caches->taIds);
	//listAllIds(caches->teIds);
	//listAllIds(caches->lblidxIds);
	listAllIds(caches->lbltknIds);

	initLabelTokenDBMemPages(lbl_tkn_pages, lbl_tkn_fp);
	unsigned char label[256] =
			"美利坚Microsoft corporation 美国yes微软公司jet出hit品版权所有cup I am so 美丽！";
	lbl_tkn_t **list = insertLabelToken(1, label, lbl_tkn_id_fp, lbl_tkn_fp);
	printf("id= %lld\n", (*list)->id);
	commitLabelToken(list, lbl_tkn_fp);
	fclose(lbl_tkn_id_fp);
	fclose(lbl_tkn_fp);
	free(lbl_tkn_pages);
	free(caches);
}


 // test time axis DB Id
/*int main(int argv, char **argc) {
	setvbuf(stdout, NULL, _IONBF, 0);
	ID_QUEUE_LENGTH = 5;
	// get new Id from next free IDs
	char *ta_id = "D:/tudata/tustore.timeaxis.tdb.id";
	FILE *ta_id_fp = fopen(ta_id, "rb+");
	// initialize
	initIdDB(ta_id);
	caches = (id_caches_t*) malloc(sizeof(id_caches_t));
	initIdCaches(caches);
	loadAllIds(ta_id_fp, caches->taIds);
	//listAllTaIds(caches->taIds);
	//recycleOneId(14);
	listAllIds(caches->taIds);

	long long d = getOneId(ta_id_fp, caches->taIds);
	printf("%lld\n", d);
	d = getOneId(ta_id_fp, caches->taIds);
	printf("%lld\n", d);
	d = getOneId(ta_id_fp, caches->taIds);
	printf("%lld\n", d);
	d = getOneId(ta_id_fp, caches->taIds);
	printf("%lld\n", d);
	d = getOneId(ta_id_fp, caches->taIds);
	printf("%lld\n", d);
	listAllTaIds(caches->taIds);
	d = getOneId(ta_id_fp, caches->taIds);
	printf("%lld\n", d);
	long long id = getOneId(ta_id_fp, caches->taIds);
	printf("%lld\n", id);
	id = getOneId(ta_id_fp, caches->taIds);
	printf("%lld\n", id);
	id = getOneId(ta_id_fp, caches->taIds);
	printf("%lld\n", id);
	id = getOneId(ta_id_fp, caches->taIds);
	printf("%lld\n", id);
	listAllTaIds(caches->taIds);
	id = getOneId(ta_id_fp, caches->taIds);
	printf("%lld\n", id);
*/
	// test recycle one id
	//	 recycleId(ta_id_fp, 4);
	//	 recycleId(ta_id_fp, 7);
	//	 recycleId(ta_id_fp, 2);
	//	 recycleId(ta_id_fp, 3);
	//	 recycleId(ta_id_fp, 11);
	//	 recycleId(ta_id_fp, 9);
	//	 recycleId(ta_id_fp, 5);
	//	 recycleId(ta_id_fp, 13);
	//	 recycleId(ta_id_fp, 17);
	//	 recycleId(ta_id_fp, 26);
	//
	//	readAllTaIds(ta_id_fp);
//	fclose(ta_id_fp);
//
//	free(caches);
//	printf("%s\n", "End");
//
//}

//int main(int argv, char **argc) {
//	char s[20] = "中国dhdhdh";
//	char s1[5] = "caodh";
//	char buf[100];
//	//char buf2[10];
//	memcpy(buf, s1, strlen(s1));
//	memcpy(buf+5, s, strlen(s));
//	printf("%s\n", buf);
//	char label1[256] = {0};
//	code_convert("utf-8", "gbk", label, 256, label1, 256);
//  printf("%s\n", label);
//	int len = cont_str(label);
//	char *s = "中国";
//	char buf[10];
//	u2g(s, strlen(s), buf, sizeof(buf));
//	char buf2[10];
//	g2u(buf, strlen(buf), buf2, sizeof(buf2));
//  printf("%s\n", "end");
//}
