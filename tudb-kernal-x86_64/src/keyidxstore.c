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

//void printTrie() {
//		printf("%s", "idx");
//		for (int i = 0; i < ARRAY_SIZE; i++) {
//			if (base[i].getTransferRatio() != BASE_NULL) {
//				System.out.printf("%7d\t", i);
//			}
//		}
//		System.out.println();
//		System.out.printf("%5s", "char");
//		for (int i = 0; i < ARRAY_SIZE; i++) {
//			if (base[i].getTransferRatio() != BASE_NULL) {
//				System.out.printf("%7c\t", base[i].getLabel());
//			}
//		}
//		System.out.println();
//		System.out.printf("%5s", "base");
//		for (int i = 0; i < ARRAY_SIZE; i++) {
//			if (base[i].getTransferRatio() != BASE_NULL) {
//				System.out.printf("%7d\t", base[i].getTransferRatio());
//			}
//		}
//		System.out.println();
//		System.out.printf("%5s", "check");
//		for (int i = 0; i < ARRAY_SIZE; i++) {
//			if (base[i].getTransferRatio() != BASE_NULL) {
//				System.out.printf("%7d\t", check[i]);
//			}
//		}
//		System.out.println();
//		System.out.printf("%5s", "leaf");
//		for (int i = 0; i < ARRAY_SIZE; i++) {
//			if (base[i].getTransferRatio() != BASE_NULL) {
//				System.out.printf("%7s\t", base[i].isLeaf() ? "是" : "否");
//			}
//		}
//		System.out.println();
//		System.out.printf("%5s", "idx");
//		for (int i = 0; i < ARRAY_SIZE; i++) {
//			if (base[i].getTransferRatio() != BASE_NULL) {
//				System.out.printf("%7d\t", base[i].getValue());
//			}
//		}
//		System.out.println();
//	}
/*
 * pages is the buffer of all pages.
 * start is page number, start = 0, ... , n-1.
 * key_idx_bas_fp is the database file of the key index base array
 * key_idx_chk_fp is the database file of the key index check array
 */
static key_idx_page_t* readOneKeyIndexPage(key_idx_page_t *pages,
		long long start, FILE *key_idx_bas_fp, FILE *key_idx_chk_fp) {
	unsigned char *bas = (unsigned char*) calloc(key_idx_bas_page_bytes,
			sizeof(unsigned char));
	unsigned char *chk = (unsigned char*) calloc(key_idx_chk_page_bytes,
			sizeof(unsigned char));

	fseek(key_idx_bas_fp, start * key_idx_bas_page_bytes, SEEK_SET);
	int c;
	if ((c = fgetc(key_idx_bas_fp)) != EOF) {
		fseek(key_idx_bas_fp, start * key_idx_bas_page_bytes, SEEK_SET);
		fread(bas, sizeof(unsigned char), key_idx_bas_page_bytes,
				key_idx_bas_fp); // read one page
	}

	fseek(key_idx_chk_fp, start * key_idx_chk_page_bytes, SEEK_SET);
	if ((c = fgetc(key_idx_chk_fp)) != EOF) {
		fseek(key_idx_chk_fp, start * key_idx_chk_page_bytes, SEEK_SET);
		fread(chk, sizeof(unsigned char), key_idx_chk_page_bytes,
				key_idx_chk_fp); // read one page
	}

	key_idx_page_t *p = (key_idx_page_t*) malloc(sizeof(key_idx_page_t));
	p->base = (dat_idx_nd_t**) calloc(ARRAY_PAGE_SIZE, sizeof(dat_idx_nd_t*)); // base array
	p->check = (long long*) calloc(ARRAY_PAGE_SIZE, sizeof(long long)); // check array
	p->dirty = 1;
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
		// parse check array
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
		if (p->start == start) { // start is page number, start = 0, ..., n-1.
			found = 1;
			break;
		} else
			p = p->nxtPage;
	}
	if (found == 0) { // not found
		p = readOneKeyIndexPage(key_idx_pages, start, key_idx_bas_fp,
				key_idx_chk_fp);
	}
	return p;
}

// char_code is UTF-8 code of char actually.
static void transfer(cur_stat_page_t *start, unsigned char char_code,
		FILE *key_idx_bas_fp, FILE *key_idx_chk_fp) {
	// s(startState) 是当前状态下标 c(offset) 是输入字符的数值（或编码）。
	// base[s] + c = t
	long long endState = abs(start->page->base[start->offset]->transferRatio)
			+ (long long) char_code; //状态转移
	//return abs(base[startState]->transferRatio)+offset;
	long long a = endState / ARRAY_PAGE_SIZE; // a is page number, a = 0,..., n-1
	long long b = endState % ARRAY_PAGE_SIZE;
	key_idx_page_t *p = search(a, key_idx_bas_fp, key_idx_chk_fp);
	p->dirty = 1;
	p->hit++;
	p->expiretime = -1;
	end->page = p;
	end->offset = b;
	end->state = endState;
	p = NULL;
}

/*
 * s_pg is the page where start state is.
 */
static int insert(unsigned char char_code, bool isLeaf, long long tuIdxId,
		FILE *key_idx_bas_fp, FILE *key_idx_chk_fp) {
	// s 是当前状态下标 c 是输入字符的数值（或编码）。
	// base[s] + c = t
	// e_pg is end state
	transfer(start, char_code, key_idx_bas_fp, key_idx_chk_fp);

	// 处理冲突
	// base[t] != 0 and check[t] != s
	long long endState = 0;
	// if (base[endState].getTransferRatio() != BASE_NULL && check[endState] != startState) { //已被占用
	if (end->page->base[end->offset]->transferRatio != 0
			&& end->page->check[end->offset] != start->state) { //已被占用
		do {
			// t += 1
			endState = end->state + 1;
			long long a = endState / ARRAY_PAGE_SIZE; // a is page number, a = 0,..., n-1
			long long b = endState % ARRAY_PAGE_SIZE;
			key_idx_page_t *p = search(a, key_idx_bas_fp, key_idx_chk_fp);
			end->page = p;
			end->offset = b;
			end->state = endState;
		} while (end->page->base[end->offset]->transferRatio != 0);
		//} while (base[endState].getTransferRatio() != BASE_NULL);
		// base[s] = t - c
		start->page->base[start->offset]->transferRatio = endState - char_code; //改变父节点转移基数
		//base[startState].setTransferRatio(endState - offset); //改变父节点转移基数
	}

	// base[t] = base[base[s] + c] = base[s](上一次的转移基数) or -1*base[s](上一次的转移基数的相反数);
	if (isLeaf) { // 找到最末位节点。
		end->page->base[end->offset]->transferRatio = (-1)
				* abs(start->page->base[start->offset]->transferRatio); //叶子节点转移基数标识为父节点转移基数的相反数
		end->page->base[end->offset]->leaf = 1;
		end->page->base[end->offset]->symbol = char_code; // char_code is utf-8 code of symbol
		end->page->base[end->offset]->tuIdxId = tuIdxId; //为叶子节点时需要记录下该词在字典中的索引号
	} else {
		if (end->page->base[end->offset]->transferRatio == 0) { //未有节点占用过
			end->page->base[end->offset]->transferRatio = abs(
					start->page->base[start->offset]->transferRatio); //非叶子节点的转移基数一定为正
		}
	}
	// check[t] =  s
	end->page->check[end->offset] = start->state; //check中记录当前状态的父状态
	return 0;
}

void build(char *word, long long tuIdxId, FILE *key_idx_bas_fp,
		FILE *key_idx_chk_fp) {
	// firstly query
	long long idxId = match(word, key_idx_bas_fp, key_idx_chk_fp);
	if (idxId > 0) { // that means it existed in DB
		// ...
		return;
	}
	// if not existing, then insert the word;
	size_t len = strlen((const char*) word);
	if (!start)
		start = (cur_stat_page_t*) malloc(sizeof(cur_stat_page_t));
	if (!end)
		end = (cur_stat_page_t*) malloc(sizeof(cur_stat_page_t));
	start->offset = 0;
	start->state = 0;
	start->page = key_idx_pages; // the first page
	unsigned char *tmpbuf = (unsigned char*) calloc(len, sizeof(unsigned char));
	convert2Utf8((char*) word, (char*) tmpbuf, len);
	insert(tmpbuf[0], (len == 1), tuIdxId, key_idx_bas_fp, key_idx_chk_fp);
	for (int i = 1; i < len; i++) {
		transfer(start, tmpbuf[i - 1], key_idx_bas_fp, key_idx_chk_fp);
		start->page = end->page;
		start->offset = end->offset;
		start->state = end->state;
		insert(tmpbuf[i], (len == i + 1), tuIdxId, key_idx_bas_fp,
				key_idx_chk_fp);
	}
	start->page = NULL;
	start->offset = 0;
	start->state = 0;
	end->page = NULL;
	end->offset = 0;
	end->state = 0;
	free(tmpbuf);
	tmpbuf = NULL;
	free(start);
	start = NULL;
	free(end);
	end = NULL;
}

/*
 * this function only matches single key word for query.
 * not for matching multiple key words.
 */
long long match(char *keyWord, FILE *key_idx_bas_fp, FILE *key_idx_chk_fp) {
	long long result = -2LL;
	size_t len = strlen((const char*) keyWord);
	unsigned char *tmp = (unsigned char*) calloc(len, sizeof(unsigned char));
	convert2Utf8((char*) keyWord, (char*) tmp, len);
	if (!start)
		start = (cur_stat_page_t*) malloc(sizeof(cur_stat_page_t));
	if (!end)
		end = (cur_stat_page_t*) malloc(sizeof(cur_stat_page_t));
	start->offset = 0;
	start->state = 0;
	start->page = key_idx_pages; // the first page
	//for (int i = 0; i < len; i++) {
	for (int j = 0; j < len; j++) {
		//for (int j = i; j < len; j++) {
		transfer(start, tmp[j], key_idx_bas_fp, key_idx_chk_fp);
		//节点存在于 Trie 树上
		if (end->page->base[end->offset]->transferRatio != 0
				&& end->page->check[end->offset] == start->state) {
			if (end->page->base[end->offset]->leaf == 1) {
				result = end->page->base[end->offset]->tuIdxId;
				break;
			}
			start->page = end->page;
			start->offset = end->offset;
			start->state = end->state;
		} else {
			break;
		}
	}
	//}
	free(tmp);
	tmp = NULL;
	start->page = NULL;
	start->offset = 0;
	start->state = 0;
	end->page = NULL;
	end->offset = 0;
	end->state = 0;
	free(start);
	free(end);
	start = NULL;
	end = NULL;
	return result;
}

// store all dirty pages.
long long commitKeyIndexes(key_idx_page_t *key_idx_pages, FILE *key_idx_bas_fp,
		FILE *key_idx_chk_fp) {
	key_idx_page_t *p = key_idx_pages;
	while (p != NULL) {
		if (p->dirty == 1) {
			// initialize memory page
			memset(p->baseContent, 0,
					key_idx_bas_page_bytes * sizeof(unsigned char));
			memset(p->checkContent, 0,
					key_idx_chk_page_bytes * sizeof(unsigned char));
			unsigned char *basBytes = (unsigned char*) calloc(
					key_idx_bas_page_bytes, sizeof(unsigned char));
			unsigned char *chkBytes = (unsigned char*) calloc(
					key_idx_chk_page_bytes, sizeof(unsigned char));
			if (p == key_idx_pages) {	// p is the first page
				longlongtoByteArray(1, basBytes);// the first base transfer ratio is 1
				longlongtoByteArray(-1, chkBytes);		// the first check is -1
				for (int i = 1; i < ARRAY_PAGE_SIZE; i++) {
					longlongtoByteArray(-2, chkBytes + i * LONG_LONG);
				}
			} else {
				for (int i = 0; i < ARRAY_PAGE_SIZE; i++) {
					longlongtoByteArray(-2, chkBytes + i * LONG_LONG);
				}
			}
			memcpy(p->baseContent, basBytes, sizeof(unsigned char));
			memcpy(p->checkContent, chkBytes, sizeof(unsigned char));
			free(chkBytes);
			chkBytes = NULL;
			free(basBytes);
			basBytes = NULL;
			// update memory page
			for (int i = 0; i < ARRAY_PAGE_SIZE; i++) {
				longlongtoByteArray(p->base[i]->transferRatio,
						p->baseContent + i * key_idx_bas_record_bytes);
				*(p->baseContent + i * key_idx_bas_record_bytes + LONG_LONG) =
						p->base[i]->symbol;
				*(p->baseContent + i * key_idx_bas_record_bytes + LONG_LONG + 1) =
						p->base[i]->leaf;
				longlongtoByteArray(p->base[i]->tuIdxId,
						p->baseContent + i * key_idx_bas_record_bytes
								+ LONG_LONG + 2);
				longlongtoByteArray(p->check[i],
						p->checkContent + i * key_idx_chk_record_bytes);
			}
			// write to DB
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
	p = NULL;
	return 0;
}

void deallocKeyIndexPages(key_idx_page_t *pages) {
	key_idx_page_t *p;
	while (pages) {
		p = pages;
		pages = pages->nxtPage;
		free(p->baseContent);
		p->baseContent = NULL;
		free(p->checkContent);
		p->checkContent = NULL;
		free(p->base);
		p->base = NULL;
		free(p->check);
		p->check = NULL;
		p->prvPage = NULL;
		p->nxtPage = NULL;
		free(p);
		p = NULL;
	}
	pages = NULL;
}
