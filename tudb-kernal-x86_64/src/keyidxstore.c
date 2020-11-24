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
		long long start, long long start_no, FILE *key_idx_bas_fp,
		FILE *key_idx_chk_fp) {
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
	int c;
	if ((c = fgetc(key_idx_chk_fp)) != EOF) {
		fseek(key_idx_chk_fp, start, SEEK_SET);
		fread(chk, sizeof(unsigned char), key_idx_chk_page_bytes,
				key_idx_chk_fp); // read one page
	}

	key_idx_page_t *p = (key_idx_page_t*) malloc(sizeof(key_idx_page_t));
	p->base = (dat_idx_nd_t*) calloc(ARRAY_PAGE_SIZE, sizeof(dat_idx_nd_t)); // base array
	p->check = (long long*) calloc(ARRAY_PAGE_SIZE, sizeof(long long)); // check array
	p->dirty = 0;
	p->start = start;
	p->expiretime = 10; // 10 minutes, by default, will be completed later.
	p->hit = 0;
	p->nxtpage = NULL;
	p->prvpage = NULL;
	p->baseContent = bas;
	p->checkContent = chk;
	// parsing the base array and check array...
	for (int i = 0; i < ARRAY_PAGE_SIZE; i++) { // parse base array
		p->base[i] = (dat_idx_nd_t*) malloc(sizeof(dat_idx_nd_t*));
		p->base[i]->id = i;
		p->base[i]->transferRatio = bytesLonglong(
				p->baseContent + i * key_idx_bas_record_bytes); // parse transfer ratio
		p->base[i]->key = bytesLonglong(
				p->baseContent + i * key_idx_bas_record_bytes + LONG_LONG); // parse content
		p->base[i]->leaf = bytesLonglong(
				p->baseContent + i * key_idx_bas_record_bytes + LONG_LONG + 1); // parse leaf
		p->base[i]->tuIdxId = bytesLonglong(
				p->baseContent + i * key_idx_bas_record_bytes + LONG_LONG + 2); // parse Tu index Id
	}
	for (int i = 0; i < ARRAY_PAGE_SIZE; i++) { // parse check array
		p->check[i] = (long long*) malloc(sizeof(long long*));
		p->check[i] = bytesLonglong(
				p->checkContent + i * key_idx_chk_record_bytes); // parse check
	}

	key_idx_page_t *pp = pages;
	if (pp != NULL) {
		while (pp->nxtpage != NULL) {
			pp = pp->nxtpage;
		}
		pp->nxtpage = p;
		p->prvpage = pp;
	} else {
		key_idx_pages = p;
	}
	return p;
}

void initKeyIndexDBMemPages(key_idx_page_t *pages, FILE *key_idx_bas_fp,
		FILE *key_idx_chk_fp) {
	// read the first page from 0 pointer.
	readOneKeyIndexPage(pages, 0LL, 0LL, key_idx_bas_fp, key_idx_chk_fp);
}

static int transfer(long long startState, unsigned char offset) {
	long long l = (long long) offset;
	return 0; //abs(base[startState]->transferRatio)+offset; //状态转移
}

dat_idx_nd_t* insert(long long startState, unsigned char offset, bool isLeaf,
		long long tuIdxId) {
	long long l = (long long) offset;
	startState = transfer(startState, l);
	//dat_idx_nd_t *idx = (dat_idx_nd_t*) malloc(sizeof(dat_idx_nd_t));
	return NULL;//idx;
}

void build(unsigned char *word, long long tuIdxId) {
	// firstly query
	// if not existing, then insert the word;
	size_t len = strlen((const char*) word);
	long long startState = 0;
	unsigned char *tmpbuf = (unsigned char*) calloc(len, sizeof(unsigned char));
	convert2Utf8((char*) word, (char*) tmpbuf, len);
	insert(startState, tmpbuf[0], (len == 1), 0);
	for (int i = 1; i < len; i++) {
		startState = transfer(startState, tmpbuf[i - 1]);
		insert(startState, tmpbuf[i], (len == i + 1), 0);
	}

}

//key_idx_t* insertKeyIndex(long long ta_id, long long firstBlkId, int length) {
//	key_idx_t *idx = (key_idx_t*) malloc(sizeof(key_idx_t));
//	idx->id = NULL_POINTER;
//	idx->useCount = 1;
//	idx->keyBlkId = firstBlkId;
//	idx->length = length;
//	idx->taId = ta_id;
//	idx->page = NULL;
//	return idx;
//}
//
//long long commitKeyIndex(key_idx_t *idx, FILE *key_idx_db_fp,
//		FILE *key_idx_id_fp) {
//	long long id = getOneId(key_idx_id_fp, caches->keyIdxIds,
//			KEY_ID_QUEUE_LENGTH);
//	idx->id = id;
//	key_idx_page_t *ps = key_idx_pages;
//	bool found = false;
//	unsigned char *pos;
//	while (!found) {
//		while (ps != NULL) {
//			if (ps->startNo <= idx->id
//					&& idx->id < ps->startNo + KEY_INDEX_PAGE_RECORDS) {
//				pos = ps->content
//						+ (idx->id - ps->startNo) * key_idx_record_bytes;
//				unsigned char ta_ids[LONG_LONG] = { 0 };
//				longToByteArray(idx->taId, ta_ids);			// ta Id
//				unsigned char length[LONG] = { 0 };
//				integer2Bytes(idx->length, length); // length
//				unsigned char count[LONG_LONG] = { 0 };
//				longToByteArray(idx->useCount, count); // label counting
//				unsigned char blkIds[LONG_LONG] = { 0 };
//				longToByteArray(idx->keyBlkId, blkIds);
//				memcpy(pos, ta_ids, LONG_LONG);
//				memcpy(pos + LONG_LONG, blkIds, LONG_LONG);
//				memcpy(pos + LONG_LONG + LONG_LONG, length, LONG);
//				memcpy(pos + LONG_LONG + LONG_LONG + LONG, count,
//				LONG_LONG);
//				fseek(key_idx_db_fp, idx->id * key_idx_record_bytes,
//				SEEK_SET); //
//				fwrite(pos, sizeof(unsigned char), key_idx_record_bytes,
//						key_idx_db_fp);
//				found = true;
//				pos = NULL;
//				break;
//			}
//			ps = ps->nxtpage;
//		}
//		if (!found) {
//			// read a new page
//			long long pagenum = (idx->id * key_idx_record_bytes)
//					/ key_idx_page_bytes;
//			readOneKeyIndexPage(key_idx_pages, pagenum * key_idx_page_bytes,
//					pagenum * KEY_INDEX_PAGE_RECORDS, key_idx_db_fp);
//			continue;
//		} else {
//			ps = NULL;
//			break;
//		}
//	}
//	idx->page = NULL;
//	free(idx);
//	return id;
//}

void deallocKeyIndexPages(key_idx_page_t *pages) {
	key_idx_page_t *p;
	while (pages) {
		p = pages;
		pages = pages->nxtpage;
		//free(p->content);
		//free(p->content);
		p->prvpage = NULL;
		p->nxtpage = NULL;
		free(p);
		p = NULL;
	}
	pages = NULL;
}
