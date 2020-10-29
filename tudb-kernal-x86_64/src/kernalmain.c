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

#include "macrodef.h"
#include "convert.h"
#include "tuidstore.h"
#include "tabptreeidx.h"
#include "lblblkstore.h"
#include "lblidxstore.h"
#include "lblsstore.h"
#include "keyidxstore.h"
#include "keyblkstore.h"
#include "valstore.h"

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

void initTaIndexDB(char *path) {
	if ((access(path, F_OK)) == -1) {
		FILE *ta_idx_fp = fopen(path, "wb+");
		unsigned char *zeroBytes = (unsigned char*) calloc(3 * LONG_LONG,
				sizeof(unsigned char));
		unsigned char *rootBytes = (unsigned char*) calloc(
				ta_bptree_idx_node_bytes, sizeof(unsigned char));
		*(rootBytes + LONG) = 0X1;
		fseek(ta_idx_fp, 0L, SEEK_SET);
		fwrite(zeroBytes, sizeof(unsigned char), 3 * LONG_LONG, ta_idx_fp);
		fwrite(rootBytes, sizeof(unsigned char), ta_bptree_idx_node_bytes,
				ta_idx_fp);
		free(zeroBytes);
		free(rootBytes);
		fclose(ta_idx_fp);
		ta_idx_fp = NULL;
		zeroBytes = NULL;
		rootBytes = NULL;
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

//long long teLabelStore(long long ta_id, char *labels[], FILE *lbls_db_fp,
//		FILE *lbls_id_fp, FILE *lbl_idx_db_fp, FILE *lbl_idx_id_fp,
//		FILE *lbl_blk_db_fp, FILE *lbl_blk_id_fp) {
//	int c = 0;
//	while (labels[c]) {
//		c++;
//	}
//	long long *idList = (long long*) calloc(c + 1, sizeof(long long));
//	int i = 0;
//	while (labels[i]) {
//		char *label = labels[i];
//		// insert into label token DB
//		lbl_blk_t **list = divideLabelBlocks((unsigned char*) label);
//		commitLabelBlocks(ta_id, list, lbl_blk_db_fp, lbl_blk_id_fp);
//		// insert into label index DB
//		lbl_idx_t *idx = insertLabelIndex(ta_id, list[0]->id, strlen(labels[i]));
//		long long idxId = commitLabelIndex(idx, lbl_idx_db_fp, lbl_idx_id_fp);
//		idList[i] = idxId;
//		deallocLabelBlockList(list);
//		label = NULL;
//		i++;
//	}
//	// insert into labels DB
//	lbls_t **lbls = insertLabels(idList, c);
//	commitLabels(ta_id, lbls, lbls_db_fp, lbls_id_fp);
//	return lbls[0]->id;
//}

//int main(int argv, char **argc) {
//    int i;
//    BPlusTree T;
//    T = Initialize();
//    clock_t c1 = clock();
//    i = 10000000;
//    while (i > 0)
//        T = Insert(T, i--);
//    i = 5000001;
//    while (i < 10000000)
//        T = Insert(T, i++);
//    i = 10000000;
//    while (i > 100)
//        T = Remove(T, i--);
//    Travel(T);
//    Destroy(T);
//    clock_t c2 = clock();
//    printf("\n用时： %lu秒\n",(c2 - c1)/CLOCKS_PER_SEC);
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

//void labelTokenOperationTest() {
// -- insert operation
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
//}

void initConf() {
	ID_QUEUE_LENGTH = 25;
	LABEL_ID_QUEUE_LENGTH = 10;
	KEY_ID_QUEUE_LENGTH = 10;
	TIMEAXIS_ID_QUEUE_LENGTH = 10;
	ID_INDEX_QUEUE_LENGTH = 10;

	// m is order. it is 3 at least.
	TA_BPLUS_TREE_ORDER = 5;
	/* node size (bytes) is page size , record size as well.
	 * that is, there are length, m-1 keys, m-1 data/m children, 1 previous, 1 next, totally,
	 * here, to keep all nodes are same, we design m data/m children, data store more than 1 B.
	 * LONG	+ 1 + (TA_BPLUS_TREE_ORDER - 1) * LONG_LONG + TA_BPLUS_TREE_ORDER * LONG_LONG + 2 * LONG_LONG;
	 * e.g., if order is 5, 93 bytes
	 */
	ta_bptree_idx_leng_leaf_bytes = LONG + 1;
	ta_bptree_idx_keys_bytes = (TA_BPLUS_TREE_ORDER - 1) * LONG_LONG;
	ta_bptree_idx_children_bytes = (TA_BPLUS_TREE_ORDER) * LONG_LONG;
	ta_bptree_idx_node_bytes = ta_bptree_idx_leng_leaf_bytes
			+ ta_bptree_idx_keys_bytes + ta_bptree_idx_children_bytes
			+ 2 * LONG_LONG;
	start_pointer = 3 * LONG_LONG;

	// label block size
	LABEL_BLOCK_LENGTH = 64;
	// label block buffer size
	LABEL_BUFFER_LENGTH = 256;
	// label token page size in memory
	LABEL_BLOCK_PAGE_RECORDS = 10;
	// label block page's expire time in memory
	LABEL_BLOCK_PAGE_EXPIRE_TIME = 10;
	// records in one page with label index records, configurable in .conf file
	LABEL_INDEX_PAGE_RECORDS = 10;
	// label index page's expire time in memory
	LABEL_INDEX_PAGE_EXPIRE_TIME = 10;
	// records in one page with labels records, configurable in .conf file
	LABELS_PAGE_RECORDS = 10;
	// labels page's expire time in memory
	LABELS_PAGE_EXPIRE_TIME = 10;

	// label bytes length.
	lbl_blk_record_bytes = LONG_LONG + 1 + LONG + LONG_LONG
			+ LABEL_BLOCK_LENGTH;
	lbl_blk_page_bytes = lbl_blk_record_bytes * LABEL_BLOCK_PAGE_RECORDS;
	// bytes in one label index record
	lbl_idx_record_bytes = 3 * LONG_LONG + LONG;
	// bytes in one page with label index records
	lbl_idx_page_bytes = lbl_idx_record_bytes * LABEL_INDEX_PAGE_RECORDS;
	// bytes in one labels record
	lbls_record_bytes = LONG_LONG + 1 + LONG_LONG + LONG_LONG + LONG_LONG;
	// bytes in one page with labels records
	lbls_page_bytes = lbls_record_bytes * LABELS_PAGE_RECORDS;

	// key (property name) block bytes length.
	key_blk_record_bytes = LONG_LONG + 1 + LONG + LONG_LONG + KEY_BLOCK_LENGTH;
	// bytes of one key (property name) block page
	key_blk_page_bytes = key_blk_record_bytes * KEY_BLOCK_PAGE_RECORDS;
	// bytes in one key (property name) index record
	key_idx_record_bytes = 3 * LONG_LONG + LONG;
	// bytes in one page with key index records
	key_idx_page_bytes = key_idx_record_bytes * KEY_INDEX_PAGE_RECORDS;
}

// test label token store
int main(int argv, char **argc) {
	setlocale(LC_ALL, "zh-CN.UTF-8");
	setvbuf(stdout, NULL, _IONBF, 0);

	const char *d_path = "D:/tudata/";
	initConf();

	char *ta_id_path = (char*) calloc(256, sizeof(char));
	strcat(ta_id_path, d_path);
	strcat(ta_id_path, "tustore.timeaxis.idx.id");

	char *ta_db_path = (char*) calloc(256, sizeof(char));
	strcat(ta_db_path, d_path);
	strcat(ta_db_path, "tustore.timeaxis.idx");
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
	// label array
	char *lbls_id_path = (char*) calloc(256, sizeof(char));
	strcat(lbls_id_path, d_path);
	strcat(lbls_id_path, "tustore.element.labels.tdb.id");

	char *lbls_db_path = (char*) calloc(256, sizeof(char));
	strcat(lbls_db_path, d_path);
	strcat(lbls_db_path, "tustore.element.labels.tdb");

	// label name
	char *lbl_idx_id_path = (char*) calloc(256, sizeof(char));
	strcat(lbl_idx_id_path, d_path);
	strcat(lbl_idx_id_path, "tustore.label.idx.id");

	char *lbl_idx_db_path = (char*) calloc(256, sizeof(char));
	strcat(lbl_idx_db_path, d_path);
	strcat(lbl_idx_db_path, "tustore.label.idx");

	char *lbl_blk_id_path = (char*) calloc(256, sizeof(char));
	strcat(lbl_blk_id_path, d_path);
	strcat(lbl_blk_id_path, "tustore.labelblock.tdb.id");

	char *lbl_blk_db_path = (char*) calloc(256, sizeof(char));
	strcat(lbl_blk_db_path, d_path);
	strcat(lbl_blk_db_path, "tustore.labelblock.tdb");
	// property name
	char *key_idx_id_path = (char*) calloc(256, sizeof(char));
	strcat(key_idx_id_path, d_path);
	strcat(key_idx_id_path, "tustore.property.key.idx.id");

	char *key_idx_db_path = (char*) calloc(256, sizeof(char));
	strcat(key_idx_db_path, d_path);
	strcat(key_idx_db_path, "tustore.property.key.idx");

	char *key_blk_id_path = (char*) calloc(256, sizeof(char));
	strcat(key_blk_id_path, d_path);
	strcat(key_blk_id_path, "tustore.property.keyblock.tdb.id");

	char *key_blk_db_path = (char*) calloc(256, sizeof(char));
	strcat(key_blk_db_path, d_path);
	strcat(key_blk_db_path, "tustore.property.keyblock.tdb");

	char *val_idx_id_path = (char*) calloc(256, sizeof(char));
	strcat(val_idx_id_path, d_path);
	strcat(val_idx_id_path, "tustore.property.value.idx.id");

	char *val_idx_db_path = (char*) calloc(256, sizeof(char));
	strcat(val_idx_db_path, d_path);
	strcat(val_idx_db_path, "tustore.property.value.idx");

	char *val_id_path = (char*) calloc(256, sizeof(char));
	strcat(val_id_path, d_path);
	strcat(val_id_path, "tustore.property.value.tdb.id");

	char *val_db_path = (char*) calloc(256, sizeof(char));
	strcat(val_db_path, d_path);
	strcat(val_db_path, "tustore.property.value.tdb");

	// initialize
	caches = (id_caches_t*) malloc(sizeof(id_caches_t));
	initIdCaches(caches);

	// initialized Id DB
	initIdDB(ta_id_path);
	initIdDB(lbls_id_path);
	initIdDB(lbl_idx_id_path);
	initIdDB(lbl_blk_id_path);
	initIdDB(key_idx_id_path);
	initIdDB(key_blk_id_path);
	// initialized DB
	initTaIndexDB(ta_db_path);
	initDB(lbls_db_path);
	initDB(lbl_idx_db_path);
	initDB(lbl_blk_db_path);
	initDB(key_idx_db_path);
	initDB(key_blk_db_path);

	FILE *ta_id_fp = fopen(ta_id_path, "rb+");
	FILE *ta_db_fp = fopen(ta_db_path, "rb+");

	FILE *lbls_id_fp = fopen(lbls_id_path, "rb+");
	FILE *lbl_idx_id_fp = fopen(lbl_idx_id_path, "rb+");
	FILE *lbl_blk_id_fp = fopen(lbl_blk_id_path, "rb+");
	FILE *key_idx_id_fp = fopen(key_idx_id_path, "rb+");
	FILE *key_blk_id_fp = fopen(key_blk_id_path, "rb+");

	FILE *lbls_db_fp = fopen(lbls_db_path, "rb+");
	FILE *lbl_idx_db_fp = fopen(lbl_idx_db_path, "rb+");
	FILE *lbl_blk_db_fp = fopen(lbl_blk_db_path, "rb+");
	FILE *key_idx_db_fp = fopen(key_idx_db_path, "rb+");
	FILE *key_blk_db_fp = fopen(key_blk_db_path, "rb+");

	FILE *val_idx_id_fp = fopen(val_idx_id_path, "rb+");
	FILE *val_idx_db_fp = fopen(val_idx_db_path, "rb+");
	FILE *val_id_fp = fopen(val_id_path, "rb+");
	FILE *val_db_fp = fopen(val_db_path, "rb+");

	loadAllIds(ta_id_fp, caches->taIds, TIMEAXIS_ID_QUEUE_LENGTH);
	//loadAllIds(taidfp, caches->teIds);
	loadAllIds(lbls_id_fp, caches->lblsIds, LABEL_ID_QUEUE_LENGTH);
	loadAllIds(lbl_idx_id_fp, caches->lblidxIds, LABEL_ID_QUEUE_LENGTH);
	loadAllIds(lbl_blk_id_fp, caches->lblblkIds, LABEL_ID_QUEUE_LENGTH);
	loadAllIds(key_idx_id_fp, caches->keyidxIds, KEY_ID_QUEUE_LENGTH);
	loadAllIds(key_blk_id_fp, caches->keyblkIds, KEY_ID_QUEUE_LENGTH);
	loadAllIds(val_idx_id_fp, caches->valIdxIds, VALUE_ID_QUEUE_LENGTH);
	loadAllIds(val_id_fp, caches->valIds, VALUE_ID_QUEUE_LENGTH);

	listAllIds(caches->taIds);
	//listAllIds(caches->teIds);
	//listAllIds(caches->prpIds);
	listAllIds(caches->lblsIds);
	listAllIds(caches->lblidxIds);
	listAllIds(caches->lblblkIds);
	listAllIds(caches->keyidxIds);
	listAllIds(caches->keyblkIds);
	listAllIds(caches->valIdxIds);
	listAllIds(caches->valIds);

	ta_idx = taIndexRootCreate(TA_BPLUS_TREE_ORDER);
	initTaIndexMemPages(ta_idx, ta_db_fp, ta_id_fp);
//	initLabelsDBMemPages(lbls_pages, lbls_db_fp);
//	initLabelIndexDBMemPages(lbl_idx_pages, lbl_idx_db_fp);
//	initLabelBlockDBMemPages(lbl_blk_pages, lbl_blk_db_fp);
//	initKeyIndexDBMemPages(key_idx_pages, key_idx_db_fp);
//	initKeyBlockDBMemPages(key_blk_pages, key_blk_db_fp);
//	initKeyBlockDBMemPages(key_blk_pages, key_blk_db_fp);
//	initValueDBMemPages(val_pages, val_db_fp);

//	long long ts = 1593783935;	//(unsigned long) time(NULL);
//	// -- insert operation
//	long long ta_id = insertEvolvedPoint(ts, ta_id_fp, ta_db_fp);
//	char *labels[] = { "大学生", "人", "Doctor", "Master硕士", "科学家Scientist" };
//	long long id = teLabelStore(ta_id, labels, lbls_db_fp, lbls_id_fp,
//			lbl_idx_db_fp, lbl_idx_id_fp, lbl_blk_db_fp, lbl_blk_id_fp);

	deallocLabelsPages(lbls_pages);
	deallocLabelIndexPages(lbl_idx_pages);
	deallocLabelBlockPages(lbl_blk_pages);
	deallocKeyIndexPages(key_idx_pages);
	deallocKeyBlockPages(key_blk_pages);
	//deallocTimeAxisPages(ta_idx);
	deallocIdCaches(caches);

	free(lbls_id_path);
	free(lbls_db_path);
	free(lbl_idx_id_path);
	free(lbl_idx_db_path);
	free(lbl_blk_id_path);
	free(lbl_blk_db_path);
	free(key_idx_id_path);
	free(key_idx_db_path);
	free(key_blk_id_path);
	free(key_blk_db_path);
	free(ta_id_path);
	free(ta_db_path);

	fclose(lbls_db_fp);
	fclose(lbls_id_fp);
	fclose(lbl_idx_id_fp);
	fclose(lbl_idx_db_fp);
	fclose(lbl_blk_id_fp);
	fclose(lbl_blk_db_fp);
	fclose(key_blk_id_fp);
	fclose(key_blk_db_fp);
	fclose(key_idx_id_fp);
	fclose(key_idx_db_fp);

	fclose(ta_id_fp);
	fclose(ta_db_fp);

	lbls_id_path = NULL;
	lbls_db_path = NULL;
	lbl_idx_id_path = NULL;
	lbl_idx_db_path = NULL;
	lbl_blk_id_path = NULL;
	lbl_blk_db_path = NULL;
	key_idx_id_path = NULL;
	key_idx_db_path = NULL;
	key_blk_id_path = NULL;
	key_blk_db_path = NULL;

	ta_id_path = NULL;
	ta_db_path = NULL;

	lbls_db_fp = NULL;
	lbls_id_fp = NULL;
	lbl_idx_id_fp = NULL;
	lbl_idx_db_fp = NULL;
	lbl_blk_id_fp = NULL;
	lbl_blk_db_fp = NULL;
	key_idx_id_fp = NULL;
	key_idx_db_fp = NULL;
	key_blk_id_fp = NULL;
	key_blk_db_fp = NULL;

	ta_id_fp = NULL;
	ta_db_fp = NULL;
	return 0;
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
// -- insert operation
//	char *labels[] =
//			{ "大学生", "人", "Doctor", "Master硕士", "科学家Scientist" };
//	long long **idList;
//	int i = 0;
//	while (labels[i]) {
//		char *cur = labels[i];
//		//int l1 = strlen(**cur);
//		int l2 = strlen(labels[i]);
//		cur = NULL;
//		i++;
//	}
//
//	return 0;
//}
