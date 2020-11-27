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
#include "keyidxstore.h"

/*
 * keyidxstore.c
 *
 * Created on: 2020年9月29日
 * Author: Dahai CAO
 */
static key_idx_page_t* readOneKeyIndexPage(key_idx_page_t *pages,
		long long start, FILE *key_idx_bas_fp, FILE *key_idx_chk_fp) {
	unsigned char *bas = (unsigned char*) calloc(key_idx_bas_page_bytes,
			sizeof(unsigned char));
	unsigned char *chk = (unsigned char*) calloc(key_idx_chk_page_bytes,
			sizeof(unsigned char));

	fseek(key_idx_bas_fp, start, SEEK_SET);
	int c;
	if ((c = fgetc(key_idx_bas_fp)) != EOF) {
		fseek(key_idx_bas_fp, start, SEEK_SET);
		fread(bas, sizeof(unsigned char), key_idx_bas_page_bytes,
				key_idx_bas_fp); // read one page
	}

	fseek(key_idx_chk_fp, start, SEEK_SET);
	if ((c = fgetc(key_idx_chk_fp)) != EOF) {
		fseek(key_idx_chk_fp, start, SEEK_SET);
		fread(chk, sizeof(unsigned char), key_idx_chk_page_bytes,
				key_idx_chk_fp); // read one page
	}

	key_idx_page_t *p = (key_idx_page_t*) malloc(sizeof(key_idx_page_t));
	p->base = (dat_idx_nd_t**) calloc(ARRAY_PAGE_SIZE, sizeof(dat_idx_nd_t*)); // base array
	p->check = (long long*) calloc(ARRAY_PAGE_SIZE, sizeof(long long)); // check array
	p->dirty = 0;
	p->start = start;
	p->expiretime = 10; // 10 minutes, by default, will be completed later.
	p->hit = 0;
	p->nxtPage = NULL;
	p->prvPage = NULL;
	p->baseContent = bas;
	p->checkContent = chk;
	// parsing the base array and check array...
	for (int i = 0; i < ARRAY_PAGE_SIZE; i++) { // parse base array
		p->base[i] = (dat_idx_nd_t*) malloc(sizeof(dat_idx_nd_t*));
		p->base[i]->id = i;
		p->base[i]->transferRatio = bytesLonglong(
				p->baseContent + i * key_idx_bas_record_bytes); // parse transfer ratio
		p->base[i]->symbol = bytesLonglong(
				p->baseContent + i * key_idx_bas_record_bytes + LONG_LONG); // parse content
		p->base[i]->leaf = bytesLonglong(
				p->baseContent + i * key_idx_bas_record_bytes + LONG_LONG + 1); // parse leaf
		p->base[i]->tuIdxId = bytesLonglong(
				p->baseContent + i * key_idx_bas_record_bytes + LONG_LONG + 2); // parse Tu index Id
	}
	for (int i = 0; i < ARRAY_PAGE_SIZE; i++) { // parse check array
		p->check[i] = bytesLonglong(
				p->checkContent + i * key_idx_chk_record_bytes); // parse check
	}

	key_idx_page_t *pp = pages;
	if (pp != NULL) {
		while (pp->nxtPage != NULL) {
			pp = pp->nxtPage;
		}
		pp->nxtPage = p;
		p->prvPage = pp;
	} else {
		key_idx_pages = p;
	}
	return p;
}

void initKeyIndexDBMemPages(key_idx_page_t *pages, FILE *key_idx_bas_fp,
		FILE *key_idx_chk_fp) {
	// read the first page from 0 pointer.
	readOneKeyIndexPage(pages, 0LL, key_idx_bas_fp, key_idx_chk_fp);
}

static key_idx_page_t* search(long long start, FILE *key_idx_bas_fp,
		FILE *key_idx_chk_fp) {
	key_idx_page_t *p = key_idx_pages;
	int found = 0;
	while (p != NULL) {
		if (p->start == start) {
			found = 1;
		} else
			p = p->nxtPage;
	}
	if (found == 0) { // not found
		p = readOneKeyIndexPage(key_idx_pages, start, key_idx_bas_fp,
				key_idx_chk_fp);
	}
	return p;
}

static long long transfer(long long startState, unsigned char offset,
		FILE *key_idx_bas_fp, FILE *key_idx_chk_fp) {
	long long l = (long long) offset;
	long a = startState / ARRAY_PAGE_SIZE;
	long b = startState % ARRAY_PAGE_SIZE;
	key_idx_page_t *p = search(a, key_idx_bas_fp, key_idx_chk_fp);
	long long endState = abs(p->base[b]->transferRatio) + l; //状态转移
	//return abs(base[startState]->transferRatio)+offset;
	p = NULL;
	return endState;
}

dat_idx_nd_t* insert(long long startState, unsigned char offset, bool isLeaf,
		long long tuIdxId, FILE *key_idx_bas_fp, FILE *key_idx_chk_fp) {
	long long l = (long long) offset;
	long long endState = transfer(startState, l, key_idx_bas_fp,
			key_idx_chk_fp);
	key_idx_page_t *p1 = search(endState, key_idx_bas_fp, key_idx_chk_fp);
	key_idx_page_t *p2 = search(startState, key_idx_bas_fp, key_idx_chk_fp);
	long long l1 = endState % ARRAY_PAGE_SIZE;
	long long l2 = startState % ARRAY_PAGE_SIZE;
	if (p1->base[l1]->transferRatio != 0 && p1->check[l1] != startState) { //已被占用
		do {
			l1 += 1;
		} while (p1->base[l1]->transferRatio != 0);
		p2 = search(startState, key_idx_bas_fp, key_idx_chk_fp);
		p2->base[l2]->transferRatio = endState - offset; //改变父节点转移基数
	}

	if (isLeaf) {
		p1->base[l1]->transferRatio = (-1)
				* abs(p2->base[startState]->transferRatio); //叶子节点转移基数标识为父节点转移基数的相反数
		p1->base[l1]->leaf = 1;
		p1->base[l1]->symbol = offset; // offset is symbol
		p1->base[l1]->tuIdxId = tuIdxId; //为叶子节点时需要记录下该词在字典中的索引号
	} else {
		if (p1->base[l1]->transferRatio == 0) { //未有节点经过
			p1->base[l1]->transferRatio = abs(
					p2->base[startState]->transferRatio); //非叶子节点的转移基数一定为正
		}
	}
	p1->check[l1] = startState; //check中记录当前状态的父状态
	dat_idx_nd_t *c = p1->base[l1];
	p1 = NULL;
	p2 = NULL;
	return c;
}

void build(unsigned char *word, long long tuIdxId, FILE *key_idx_bas_fp,
		FILE *key_idx_chk_fp) {
	// firstly query
	// if not existing, then insert the word;
	size_t len = strlen((const char*) word);
	long long startState = 0;
	unsigned char *tmpbuf = (unsigned char*) calloc(len, sizeof(unsigned char));
	convert2Utf8((char*) word, (char*) tmpbuf, len);
	insert(startState, tmpbuf[0], (len == 1), tuIdxId, key_idx_bas_fp,
			key_idx_chk_fp);
	for (int i = 1; i < len; i++) {
		startState = transfer(startState, tmpbuf[i - 1], key_idx_bas_fp,
				key_idx_chk_fp);
		insert(startState, tmpbuf[i], (len == i + 1), tuIdxId, key_idx_bas_fp,
				key_idx_chk_fp);
	}

}

long long match(char *keyWord, FILE *key_idx_bas_fp, FILE *key_idx_chk_fp) {
	long long startState, endState;
	long long result;
	size_t len = strlen((const char*) keyWord);
	unsigned char *tmpbuf = (unsigned char*) calloc(len, sizeof(unsigned char));
	convert2Utf8((char*) keyWord, (char*) tmpbuf, len);
	for (int i = 0; i < len; i++) {
		startState = 0;
		for (int j = i; j < len; j++) {
			endState = transfer(startState, tmpbuf[j], key_idx_bas_fp,
					key_idx_chk_fp);
			key_idx_page_t *p1 = search(startState, key_idx_bas_fp,
					key_idx_chk_fp);
			//节点存在于 Trie 树上
			if (p1->base[endState]->transferRatio != 0
					&& p1->check[endState] == startState) {

				if (p1->base[endState]->leaf == 1) {

					result = p1->base[endState]->tuIdxId;
					printf("tuIdxId = %lld\n", result);

					startState = endState;
				}
			} else {
				break;
			}
		}
	}
	return result;
}

// store all dirty pages.
long long commitKeyIndex(key_idx_page_t *key_idx_pages, FILE *key_idx_bas_fp,
		FILE *key_idx_chk_fp) {
	key_idx_page_t *p = key_idx_pages;
	while (p != NULL) {
		if (p->dirty == 1) {

			// update memory page
			memset(p->baseContent, 0,
					key_idx_bas_page_bytes * sizeof(unsigned char));
			unsigned char *chkBytes = (unsigned char*) calloc(
					key_idx_chk_page_bytes, sizeof(unsigned char));
			for (int i = 0; i < ARRAY_PAGE_SIZE; i++) {
				longlongtoByteArray(-2, chkBytes + i * LONG_LONG);
			}
			memcpy(p->checkContent, chkBytes, sizeof(unsigned char));
			free(chkBytes);
			chkBytes = NULL;
			for (int i = 0; i < ARRAY_PAGE_SIZE; i++) {
				longlongtoByteArray(p->base[i]->transferRatio,
						p->baseContent + i * key_idx_bas_record_bytes);
				p->baseContent + i * key_idx_bas_record_bytes + 1 =
						p->base[i]->symbol;
				p->baseContent + i * key_idx_bas_record_bytes + 2 =
						p->base[i]->leaf;
				longlongtoByteArray(p->base[i]->tuIdxId,
						p->baseContent + 3 + i * key_idx_bas_record_bytes);
				longlongtoByteArray(p->check[i],
						p->checkContent + i * key_idx_chk_record_bytes);
			}
			// write to db
			fseek(key_idx_bas_fp, p->start * key_idx_bas_page_bytes, SEEK_SET);
			fwrite(p->baseContent, sizeof(unsigned char),
					key_idx_bas_page_bytes, key_idx_bas_fp);
			fseek(key_idx_chk_fp, p->start * key_idx_chk_page_bytes, SEEK_SET);
			fwrite(p->checkContent, sizeof(unsigned char),
					key_idx_chk_page_bytes, key_idx_chk_fp);
			p->dirty = 0;
		}
		p = p->nxtPage;
	}
	return 0;
}

void deallocKeyIndexPages(key_idx_page_t *pages) {
	key_idx_page_t *p;
	while (pages) {
		p = pages;
		pages = pages->nxtPage;
		free(p->baseContent);
		free(p->checkContent);
		free(p->base);
		free(p->check);
		p->prvPage = NULL;
		p->nxtPage = NULL;
		free(p);
		p = NULL;
	}
	pages = NULL;
}
