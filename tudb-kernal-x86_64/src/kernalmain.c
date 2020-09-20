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
#include "tuidstore.h"
#include "tastore.h"

/*
 * main.c
 *
 * Created on: 2020年7月20日
 * Author: Dahai CAO
 */

// initialize DB
void initIdDB(char *path) {
	if ((access(path, F_OK)) == -1) {
		FILE *ta_id_fp = fopen(path, "wb+");
		// initializes Id DB
		unsigned char zero[LONG_LONG] = { 0L };
		fseek(ta_id_fp, 0L, SEEK_SET); // move file pointer to file head
		fwrite(zero, sizeof(unsigned char), LONG_LONG, ta_id_fp);
		//fseek(taidfp, LONG_LONG, SEEK_SET);  // move file pointer to 8th byte
		fwrite(zero, sizeof(unsigned char), LONG_LONG, ta_id_fp);
		fclose(ta_id_fp);
		ta_id_fp = NULL;
	}
}

void initIds(FILE *id_fp) {
	if (id_fp != NULL) {
		// initializes Id DB
		unsigned char nIds[LONG_LONG] = { 0L };
		//unsigned char lastrIds[LONG_LONG] = { 0L };
		//unsigned char rIds[LONG_LONG] = { 0L };
		long long nId = 2LL;
		//long long lastrId = 0LL;
		//long long rId = 0;
		LongToByteArray(nId, nIds); // convert
		//LongToByteArray(lastrId, lastrIds); // convert
		//LongToByteArray(rId, rIds); // convert
		fseek(id_fp, 0, SEEK_SET); // move file pointer to file end
		fwrite(nIds, sizeof(unsigned char), LONG_LONG, id_fp);
		//fwrite(lastrIds, sizeof(unsigned char), LONG_LONG, id_fp);
		//fwrite(rIds, sizeof(unsigned char), LONG_LONG, id_fp);
	}
}

// create and initialize new Tu time axis DB file
void initTimeAxisDB(char *path) {
	if ((access(path, F_OK)) == -1) {
		FILE *tadbfp = fopen(path, "wb+");
		// initializes Id DB
		unsigned char n2[LONG_LONG] = { 0L };
		LongToByteArray(NULL_POINTER, n2); // convert
		fseek(tadbfp, 0L, SEEK_SET); // move file pointer to file head
		fwrite(n2, sizeof(unsigned char), LONG_LONG, tadbfp);
		//fseek(tadbfp, LONG_LONG, SEEK_SET);  // move file pointer to 8th byte
		fwrite(n2, sizeof(unsigned char), LONG_LONG, tadbfp);
		fclose(tadbfp);
		tadbfp = NULL;
	}
}

// create and initialize new Tu DB file
void initDB(char *path) {
	if ((access(path, F_OK)) == -1) {
		// create a new Tu DB file
		FILE *tudbfp = fopen(path, "wb+");
		fclose(tudbfp);
		tudbfp = NULL;
	}
}



void init() {
	ID_QUEUE_LENGTH = 25;
	LABEL_ID_QUEUE_LENGTH = 10;
	TIMEAXIS_ID_QUEUE_LENGTH = 10;

	// time axis record byte array length
	tm_axis_record_bytes = 3 * LONG_LONG + 1;
	// time axis memory page records, configurable in .conf file
	TIME_AXIS_PAGE_RECORDS = 10;
	// time axis page byte array length
	tm_axis_page_bytes = tm_axis_record_bytes * TIME_AXIS_PAGE_RECORDS;
	// time axis DB start points in database
	tm_axis_db_start_byte = 16LL;
	// time axis page expiration time (minutes), configurable in .conf file
	TIME_AXIS_PAGE_EXPIRE_TIME = 10;


	// label token block size
	LABEL_BLOCK_LENGTH = 64;
	// label token buffer size
	LABEL_BUFFER_LENGTH = 256;
	// label token page size in memory
	LABEL_TOKEN_PAGE_RECORDS = 10;
	// label token page's expire time in memory
	LABEL_TOKEN_PAGE_EXPIRE_TIME = 10;
	// records in one page with label index records, configurable in .conf file
	LABEL_INDEX_PAGE_RECORDS = 10;
	// label index page's expire time in memory
	LABEL_INDEX_PAGE_EXPIRE_TIME = 10;

	// label bytes length.
	lbl_tkn_record_bytes = LONG_LONG + 1 + LONG + LONG_LONG
			+ LABEL_BLOCK_LENGTH;
	lbl_tkn_page_bytes = lbl_tkn_record_bytes * LABEL_TOKEN_PAGE_RECORDS;
	// bytes in one label index record
	lbl_idx_record_bytes = 4 * LONG_LONG;
	// bytes in one page with label index records
	lbl_idx_page_bytes = lbl_idx_record_bytes * LABEL_INDEX_PAGE_RECORDS;

}



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
    setlocale(LC_ALL, "zh-CN.UTF-8");
	setvbuf(stdout, NULL, _IONBF, 0);

	const char *d_path = "D:/tudata/";
	init();

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
	char *lbls_id_path = (char*) calloc(256, sizeof(char));
	strcat(lbls_id_path, d_path);
	strcat(lbls_id_path, "tustore.element.labels.tdb.id");

	char *lbls_db_path = (char*) calloc(256, sizeof(char));
	strcat(lbls_db_path, d_path);
	strcat(lbls_db_path, "tustore.element.labels.tdb");

	char *lbl_idx_id_path = (char*) calloc(256, sizeof(char));
	strcat(lbl_idx_id_path, d_path);
	strcat(lbl_idx_id_path, "tustore.label.index.tdb.id");

	char *lbl_idx_db_path = (char*) calloc(256, sizeof(char));
	strcat(lbl_idx_db_path, d_path);
	strcat(lbl_idx_db_path, "tustore.label.index.tdb");

	char *lbl_tkn_id_path = (char*) calloc(256, sizeof(char));
	strcat(lbl_tkn_id_path, d_path);
	strcat(lbl_tkn_id_path, "tustore.label.token.tdb.id");

	char *lbl_tkn_db_path = (char*) calloc(256, sizeof(char));
	strcat(lbl_tkn_db_path, d_path);
	strcat(lbl_tkn_db_path, "tustore.label.token.tdb");

	// initialize
	caches = (id_caches_t*) malloc(sizeof(id_caches_t));
	initIdCaches(caches);
	initIdDB(lbl_idx_id_path);
	initIdDB(lbl_tkn_id_path);

	// initTimeAxisDB(tadb);
	initDB(lbl_idx_db_path);
	initDB(lbl_tkn_db_path);
	initDB(lbls_db_path);

	FILE *lbls_id_fp = fopen(lbls_id_path, "rb+");
	FILE *lbl_idx_id_fp = fopen(lbl_idx_id_path, "rb+");
	FILE *lbl_tkn_id_fp = fopen(lbl_tkn_id_path, "rb+");

	FILE *lbls_db_fp = fopen(lbls_db_path, "rb+");
	FILE *lbl_idx_db_fp = fopen(lbl_idx_db_path, "rb+");
	FILE *lbl_tkn_db_fp = fopen(lbl_tkn_db_path, "rb+");
	//initIds(lbl_tkn_id_fp);

	//loadAllIds(taidfp, caches->taIds);
	//loadAllIds(taidfp, caches->teIds);
	loadAllIds(lbl_idx_id_fp, caches->lblidxIds);
	loadAllIds(lbl_tkn_id_fp, caches->lbltknIds, LABEL_ID_QUEUE_LENGTH);

	//listAllIds(caches->taIds);
	//listAllIds(caches->teIds);
	listAllIds(caches->lblidxIds);
	listAllIds(caches->lbltknIds);

	initLabelIndexDBMemPages(lbl_idx_pages, lbl_idx_db_fp);
	initLabelTokenDBMemPages(lbl_tkn_pages, lbl_tkn_db_fp);

	// -- insert operation
//	unsigned char label[256] =
//			"美利坚Microsoft corporation 美国yes微软公司jet出hit品版权所有cup I am so 美丽！";
//	lbl_tkn_t **list = divideLabelTokens(label);
//	printf("id= %lld\n", (*list)->id);
//	commitLabelToken(1, list, lbl_tkn_fp, lbl_tkn_id_fp);
//	listAllIds(caches->lbltknIds);
//	// -- query operation
//	unsigned char *slabel = findLabelToken(0, lbl_tkn_fp);
//	printf("%s\n", slabel);
//	// -- deletion operation
//	deleteLabelToken(0, lbl_tkn_fp);
//	listAllIds(caches->lbltknIds);
	// -- update operation
//	unsigned char label2[256] ="美利坚Microsoft corporation 美国yes微软公司 华盛顿施普林格springer景观大道venue，北大街社区中心的地下室中的冰箱冷冻室里的小盒子中";
//	lbl_tkn_t **list11 = searchLabelTokenList(0, lbl_tkn_fp);
//	lbl_tkn_t **newlist = divideLabelTokens(label2);
//	int k = 0; // i means realloc times
//	while (*(list11 + k)) { // calculate label string length
//		printf("num = %d\n", k);
//		k++;
//	}
//	// combine the label blocks to one label.
//	commitUpdateLabelToken(list11, newlist, lbl_tkn_id_fp, lbl_tkn_fp);
//	listAllIds(caches->lbltknIds);
//	unsigned char *slabel1 = findLabelToken(0, lbl_tkn_fp);
//	printf("%s\n", slabel1);
//
//	deallocLabelTokenPages(lbl_tkn_pages);




	deallocIdCaches(caches);
	fclose(lbl_tkn_id_fp);
	fclose(lbl_tkn_db_fp);
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
//	setvbuf(stdout, NULL, _IONBF, 0);
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
//	int in = 65536888;
//	unsigned char s[4] = {0};
//	Integer2Bytes(in, s);
//	int out = Bytes2Integer(s);
//  printf("%d\n", out);
//	unsigned char s1[8] = {0};
//	long long in1 = 2898767637766;
//	LongToByteArray(in1, s1);
//	long long out1 = ByteArrayToLong(s1);
//	printf("%lld\n", out1);

//	int i;
//	int *pn = (int*) malloc(5 * sizeof(int));
//	printf("%p\n", pn);
//	for (i = 0; i < 5; i++)
//		scanf("%d", &pn[i]);
//	pn = (int*) realloc(pn, 10 * sizeof(int));
//	printf("%p\n", pn);
//	for (i = 0; i < 5; i++)
//		printf("%3d", pn[i]);
//	printf("\n");
//	free(pn);

	// pointer testing
//	int i = 1;
//	int * list=NULL;
//	while(i<10) {
//		list = (int*) realloc(list, i* sizeof(int));
//		*(list + i - 1) = i;
//		i++;
//	}
//	printf("i = %d\n", i);
//	i = 0;
//	while (*(list + i)) { // calculate label string length
//		printf("num = %d\n", *(list + i));
//		i++;
//	}

	// double pointer testing
//	int i = 1;
//	int **list = NULL;
//	while (i < 10) {
//		list = (int**) realloc(list, i * sizeof(int*));
//		int *p = (int *)malloc(sizeof(int));
//		*p = i;
//		*(list + i - 1) = p;
//		i++;
//	}
//	(list + i - 1) = (int**) realloc(list, i * sizeof(int*)); // add a zero after the last element
//	*(list + i - 1) = 0x0;
//	printf("i = %d\n", i);
//	i = 0;
//	while (*(list + i)) { // calculate label string length
//		printf("num = %d\n", **(list + i));
//		i++;
//	}
//	// free double pointer
//	int j = 0;
//	while (*(list + j)) {
//		free(*(list + j));
//		j++;
//	}
//	realloc(list, 0);
//	list = NULL;
//	return 0;
//}


