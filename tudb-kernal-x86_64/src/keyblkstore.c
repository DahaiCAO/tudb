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
#include "keyblkstore.h"
/*
 * keyblkstore.c
 *
 * Created on: 2020年9月27日
 * Author: Dahai CAO
 */

/* read one key block page
 * start the start pointer in memory
 * start_no the start record id in this page
 * key_blk_db_fp key block DB file
 */
static key_blk_page_t* readOneKeyBlockPage(key_blk_page_t *pages,
		long long start, long long start_no, FILE *key_blk_db_fp) {
	unsigned char *page = (unsigned char*) malloc(
			sizeof(unsigned char) * key_blk_page_bytes);
	memset(page, 0, sizeof(unsigned char) * key_blk_page_bytes);
	fseek(key_blk_db_fp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(key_blk_db_fp)) != EOF) {
		fseek(key_blk_db_fp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), key_blk_page_bytes, key_blk_db_fp); // read one page
	}

	key_blk_page_t *p = (key_blk_page_t*) malloc(sizeof(key_blk_page_t));
	p->dirty = 0;
	p->expiretime = KEY_BLOCK_PAGE_EXPIRE_TIME; // 10 minutes
	p->startNo = start_no;
	p->hit = 0;
	p->nxtpage = NULL;
	p->prvpage = NULL;
	p->content = page;
	p->start = start;
	p->end = start + key_blk_page_bytes;

	key_blk_page_t *pp = pages;
	if (pp != NULL) {
		while (pp->nxtpage != NULL) {
			pp = pp->nxtpage;
		}
		pp->nxtpage = p;
		p->prvpage = pp;
		pp = NULL;
	} else {
		key_blk_pages = p;
	}
	return p;
}

void initKeyBlockDBMemPages(key_blk_page_t *pages, FILE *key_blk_db_fp) {
	// read the first page from 0 pointer.
	readOneKeyBlockPage(pages, 0LL, 0LL, key_blk_db_fp);
}

void commitKeyBlocks(long long ta_id, key_blk_t **list, FILE *key_blk_db_fp,
		FILE *key_blk_id_fp) {
	key_blk_t **t = list;
	// assign a ID to every token block
	int j = 0;
	t = list;
	while (*(t + j) != NULL) {
		(*(t + j))->id = getOneId(key_blk_id_fp, caches->keyblkIds,
				KEY_ID_QUEUE_LENGTH);
		if (j > 0) {
			(*(t + j - 1))->nxtBlkId = (*(t + j))->id;
		}
		j++;
	}
	t = list;
	j = 0;
	// one by one to store label tokens
	while (*(t + j) != NULL) {
		// search a label token space for storing label token
		key_blk_page_t *ps = key_blk_pages;
		bool found = false;
		unsigned char *pos;
		while (!found) {
			while (ps != NULL) {
				if (ps->startNo <= (*(t + j))->id
						&& (*(t + j))->id
								< ps->startNo + KEY_BLOCK_PAGE_RECORDS) {
					pos = ps->content
							+ ((*(t + j))->id - ps->startNo)
									* key_blk_record_bytes;
					// convert label token block to byte array
					unsigned char ta_ids[LONG_LONG] = { 0 };
					longToByteArray(ta_id, ta_ids);			// ta Id
					unsigned char length[LONG] = { 0 };
					integer2Bytes((*(t + j))->len, length);
					unsigned char nblockId[LONG_LONG] = { 0 };
					longToByteArray((*(t + j))->nxtBlkId, nblockId);// next block Id
					// store label token block
					memcpy(pos, ta_ids, LONG_LONG);
					unsigned char inuse[1] = { (*(t + j))->inUse };
					memcpy(pos + LONG_LONG, inuse, 1LL);
					memcpy(pos + LONG_LONG + 1, length, LONG);
					memcpy(pos + LONG_LONG + 1 + LONG, nblockId,
					LONG_LONG);
					memcpy(pos + LONG_LONG + 1 + LONG + LONG_LONG,
							(*(t + j))->blkContent, (*(t + j))->len);
					// update to DB
					fseek(key_blk_db_fp, (*(t + j))->id * key_blk_record_bytes,
					SEEK_SET); //
					fwrite(pos, sizeof(unsigned char), key_blk_record_bytes,
							key_blk_db_fp);
					found = true;
					pos = NULL;
					break;
				}
				ps = ps->nxtpage;
			}
			if (!found) {
				// read a new page
				long long pagenum = ((*(t + j))->id * key_blk_record_bytes)
						/ key_blk_page_bytes;
				readOneKeyBlockPage(key_blk_pages, pagenum * key_blk_page_bytes,
						pagenum * KEY_BLOCK_PAGE_RECORDS, key_blk_db_fp);
				continue;
			} else {
				ps = NULL;
				break;
			}
		}
		j++;
	}
	t = NULL;
}

key_blk_t** divideKeyBlocks(unsigned char *key) {
	key_blk_t **list;
	unsigned char *tmpbuf = (unsigned char*) calloc(KEY_BUFFER_LENGTH,
			sizeof(unsigned char));
	convert2Utf8((char*) key, (char*) tmpbuf, KEY_BUFFER_LENGTH);
	size_t l = strlen((const char*) tmpbuf);
	if (l > KEY_BLOCK_LENGTH) {
		size_t s = l / KEY_BLOCK_LENGTH;
		size_t y = l % KEY_BLOCK_LENGTH;
		key_blk_t **p;
		if (y > 0) {
			list = (key_blk_t**) calloc(s + 2, sizeof(key_blk_t*)); // one more for 0x0
		} else {
			list = (key_blk_t**) calloc(s + 1, sizeof(key_blk_t*)); // one more for 0x0
		}
		p = list;
		for (int i = 0; i < s; i++) {
			unsigned char *buf = (unsigned char*) calloc(KEY_BLOCK_LENGTH,
					sizeof(unsigned char));
			memcpy(buf, tmpbuf + i * KEY_BLOCK_LENGTH, KEY_BLOCK_LENGTH);
			key_blk_t *tkn = (key_blk_t*) malloc(sizeof(key_blk_t));
			tkn->blkContent = buf;
			tkn->id = NULL_POINTER;
			tkn->inUse = 1;
			tkn->len = KEY_BLOCK_LENGTH;
			tkn->page = NULL;
			tkn->taId = 0;
			tkn->nxtBlkId = NULL_POINTER;
			*p = tkn;
			p = p + 1;
			buf = NULL;
			tkn = NULL;
		}
		if (y > 0) {
			unsigned char *buf = (unsigned char*) calloc(KEY_BLOCK_LENGTH,
					sizeof(unsigned char));
			memcpy(buf, tmpbuf + s * KEY_BLOCK_LENGTH, y);
			key_blk_t *tkn = (key_blk_t*) malloc(sizeof(key_blk_t));
			tkn->blkContent = buf;
			tkn->id = NULL_POINTER;
			tkn->inUse = 1;
			tkn->len = y;
			tkn->page = NULL;
			tkn->taId = 0;
			tkn->nxtBlkId = NULL_POINTER;
			*p = tkn;
			buf = NULL;
			tkn = NULL;
		}
		p = NULL;
	} else {
		list = (key_blk_t**) calloc(1, sizeof(key_blk_t));
		key_blk_t *tkn = (key_blk_t*) malloc(sizeof(key_blk_t));
		tkn->blkContent = tmpbuf;
		tkn->id = NULL_POINTER;
		tkn->inUse = 0x1;
		tkn->len = strlen((const char*) tmpbuf);
		tkn->page = NULL;
		tkn->taId = 0;
		tkn->nxtBlkId = NULL_POINTER;
		*list = tkn;
		tkn = NULL;
	}
	tmpbuf = NULL;
	return list;
}

key_blk_t** searchKeyBlockList(long long id, FILE *key_blk_db_fp) {
	key_blk_t **list = NULL;
	key_blk_page_t *ps = key_blk_pages;
	int i = 1;
	unsigned char *pos;
	long long tId = id;
	while (tId != NULL_POINTER) {
		while (ps != NULL) {
			if (ps->startNo <= tId
					&& tId < ps->startNo + KEY_BLOCK_PAGE_RECORDS) {
				pos = ps->content + (tId - ps->startNo) * key_blk_record_bytes;
				key_blk_t *tkn = (key_blk_t*) malloc(sizeof(key_blk_t));
				unsigned char *buf = (unsigned char*) calloc(
						key_blk_record_bytes, sizeof(unsigned char));
				memcpy(buf, pos, key_blk_record_bytes);
				tkn->page = ps;
				tkn->id = tId;
				unsigned char ta_ids[LONG_LONG] = { 0 };
				memcpy(ta_ids, buf, LONG_LONG);
				tkn->taId = byteArrayToLong(ta_ids); // ta id

				tkn->inUse = *(buf + LONG_LONG); // in use

				unsigned char length[LONG] = { 0 };
				memcpy(length, buf + LONG_LONG + 1, LONG);
				tkn->len = bytes2Integer(length); // block length

				unsigned char nblockId[LONG_LONG] = { 0 };
				memcpy(nblockId, (buf + LONG_LONG + 1 + LONG), LONG_LONG);
				tkn->nxtBlkId = byteArrayToLong(nblockId); // next block id

				unsigned char *blockContent = (unsigned char*) calloc(
						KEY_BLOCK_LENGTH, sizeof(unsigned char)); // block content
				memcpy(blockContent, buf + LONG_LONG + 1 + LONG + LONG_LONG,
						tkn->len);
				tkn->blkContent = blockContent;
				list = (key_blk_t**) realloc(list, sizeof(key_blk_t*) * i);
				*(list + i - 1) = tkn;
				free(buf);
				buf = NULL;
				i++;
				tId = tkn->nxtBlkId;
				tkn = NULL;
				blockContent = NULL;
				if (tId == NULL_POINTER) {
					break;
				} else {
					continue;
				}
			}
			ps = ps->nxtpage;
		}
		if (tId != NULL_POINTER) {
			// read a new page
			long long pagenum = (tId * key_blk_record_bytes)
					/ key_blk_page_bytes;
			readOneKeyBlockPage(key_blk_pages, pagenum * key_blk_page_bytes,
					pagenum * KEY_BLOCK_PAGE_RECORDS, key_blk_db_fp);
			continue;
		} else {
			break;
		}
	}
	list = (key_blk_t**) realloc(list, sizeof(key_blk_t*) * i);
	*(list + i - 1) = 0x0; // add one zero after the last element.
	ps = NULL;
	pos = NULL;
	return list;
}

unsigned char* findKey(long long id, FILE *key_blk_db_fp) {
	key_blk_t **list = searchKeyBlockList(id, key_blk_db_fp);
	// combine the key blocks to one key string.
	int j = 0;
	key_blk_t **p = list;
	int l = 0;
	while (*(p + j)) { // calculate total key string length
		l = l + (*(p + j))->len;
		j++;
	}
	unsigned char *ct = (unsigned char*) calloc(l + 1, sizeof(unsigned char));
	int k = 0;
	j = 0;
	p = list;
	while (*(p + j)) { // put all block content into one buffer
		unsigned char *ch = (*(p + j))->blkContent;
		for (int i = 0; i < (*(p + j))->len; i++) {
			ct[k] = *(ch + i);
			k++;
		}
		j++;
	}
	p = NULL;
	deallocKeyBlockList(list);
	return ct;
}

void deleteKey(long long id, FILE *key_blk_db_fp) {
	key_blk_t **list = searchKeyBlockList(id, key_blk_db_fp);
	int j = 0;
	key_blk_t **p = list;
	while (*(p + j)) {
		unsigned char *pos = ((*(p + j))->page)->content
				+ ((*(p + j))->id - ((*(p + j))->page)->startNo)
						* key_blk_record_bytes;
		*(pos + LONG_LONG) = 0x0;
		recycleOneId((*(p + j))->id, caches->keyblkIds);
		// update to DB
		fseek(key_blk_db_fp, (*(p + j))->id * key_blk_record_bytes + LONG_LONG,
		SEEK_SET);
		unsigned char inuse[1] = { 0x0 };
		fwrite(inuse, sizeof(unsigned char), 1, key_blk_db_fp);
		pos = NULL;
		j++;
	}
	deallocKeyBlockList(list);
	p = NULL;
	list = NULL;
}

// this update operation is used to update and override new key to old key
void commitUpdateKey(key_blk_t **list, key_blk_t **newlist, FILE *key_blk_id_fp,
		FILE *key_blk_db_fp) {
	// c is old list length
	// j is new list length
	key_blk_t **l = list;
	key_blk_t **t = newlist;
	int c = 0;
	int j = 0;
	// get the new list length;
	while (*(t + j) != NULL) {
		j++;
	}
	while (*(l + c) != NULL) {
		c++;
	}
	//printf("%d\n", j);
	int k = 0;
	if (c < j) { // the new list is longer than the old list
		while (*(t + k) != NULL) {
			if (k < c) {
				(*(t + k))->id = (*(l + k))->id;
				(*(t + k))->taId = (*(l + k))->taId;
				(*(t + k))->nxtBlkId = (*(l + k))->nxtBlkId;
			} else {
				(*(t + k))->id = getOneId(key_blk_id_fp, caches->keyblkIds,
						KEY_ID_QUEUE_LENGTH);
				if (k >= c) {
					(*(t + k - 1))->nxtBlkId = (*(t + k))->id;
				}
			}
			k++;
		}
	} else if (c > j) { // the new list is shorter than the old list
		while (k < c) {
			if (*(t + k) != NULL) {
				(*(t + k))->id = (*(l + k))->id;
				(*(t + k))->taId = (*(l + k))->taId;
				(*(t + k))->nxtBlkId = (*(l + k))->nxtBlkId;
			} else {
				(*(l + k))->inUse = 0x0;
				if (k == j) {
					(*(t + k - 1))->nxtBlkId = NULL_POINTER;
				}
			}
			k++;
		}
	} else if (c == j) { // the new list is equal to old list
		while (*(t + k) != NULL) {
			if (*(t + k) != NULL) {
				(*(t + k))->id = (*(l + k))->id;
				(*(t + k))->taId = (*(l + k))->taId;
				(*(t + k))->nxtBlkId = (*(l + k))->nxtBlkId;
			}
			k++;
		}
	}
	// store new list to DB
	k = 0;
	// one by one to store label tokens
	while (*(t + k) != NULL) {
		// search a label token space for storing label token
		key_blk_page_t *ps = key_blk_pages;
		bool found = false;
		unsigned char *pos;
		while (!found) {
			while (ps != NULL) {
				if (ps->startNo <= (*(t + k))->id
						&& (*(t + k))->id
								< ps->startNo + KEY_BLOCK_PAGE_RECORDS) {
					pos = ps->content
							+ ((*(t + k))->id - ps->startNo)
									* key_blk_record_bytes;
					unsigned char ta_ids[LONG_LONG] = { 0 };
					longToByteArray((*(t + k))->taId, ta_ids);			// ta Id
					memcpy(pos, ta_ids, LONG_LONG);

					unsigned char inuse[1] = { (*(t + k))->inUse };
					memcpy(pos + LONG_LONG, inuse, 1);		// in use flag

					unsigned char length[LONG] = { 0 };
					integer2Bytes((*(t + k))->len, length);
					memcpy(pos + LONG_LONG + 1, length, LONG); // block length

					unsigned char nblockId[LONG_LONG] = { 0 };
					longToByteArray((*(t + k))->nxtBlkId, nblockId); // next block Id
					memcpy(pos + LONG_LONG + 1 + LONG, nblockId,
					LONG_LONG);

					memcpy(pos + LONG_LONG + 1 + LONG + LONG_LONG,
							(*(t + k))->blkContent, (*(t + k))->len); // block content
					// update to DB
					fseek(key_blk_db_fp, (*(t + k))->id * key_blk_record_bytes,
					SEEK_SET); //
					fwrite(pos, sizeof(unsigned char), key_blk_record_bytes,
							key_blk_db_fp);
					pos = NULL;
					found = true;
					break;
				}
				ps = ps->nxtpage;
			}
			if (!found) {
				// read a new page
				long long pagenum = ((*(t + k))->id * key_blk_record_bytes)
						/ key_blk_page_bytes;
				readOneKeyBlockPage(key_blk_pages, pagenum * key_blk_page_bytes,
						pagenum * KEY_BLOCK_PAGE_RECORDS, key_blk_db_fp);
				continue;
			} else {
				ps = NULL;
				break;
			}
		}
		k++;
	}
	if (c > j) { // the new list is shorter than the old list
		while (k < c) { // each element in list can be found in memory pages.
			unsigned char *pos = ((*(l + k))->page)->content
					+ ((*(l + k))->id - ((*(l + k))->page)->startNo)
							* key_blk_record_bytes;
			*(pos + LONG_LONG) = 0x0;
			recycleOneId((*(l + k))->id, caches->keyblkIds);
			// update to DB
			fseek(key_blk_db_fp,
					(*(l + k))->id * key_blk_record_bytes + LONG_LONG,
					SEEK_SET);
			unsigned char inuse[1] = { 0x0 };
			fwrite(inuse, sizeof(unsigned char), 1, key_blk_db_fp);
			pos = NULL;
			k++;
		}
	}
	l = NULL;
	t = NULL;
}

void combineKeyBlocks(key_blk_t **list, int length) {
	int j = 0;
	key_blk_t **p = list;
	unsigned char *ct = (unsigned char*) calloc(length + 1,
			sizeof(unsigned char));
	int k = 0;
	while (*(p + j)) {
		unsigned char *ch = (*(p + j))->blkContent;
		for (int i = 0; i < (*(p + j))->len; i++) {
			ct[k] = *(ch + i);
			k++;
		}
		j++;
	}
	printf("%s\n", ct);
	free(ct);
	ct = NULL;
	p = NULL;
}

// deallocate key block list
void deallocKeyBlockList(key_blk_t **list) {
	int j = 0;
	while (*(list + j)) {
		free((void *)(*(list + j))->blkContent);
		(*(list + j))->blkContent = NULL;
		(*(list + j))->page = NULL;
		free(*(list + j));
		j++;
	}
	free(list);
	list = NULL;
}

// deallocate key block memory pages
void deallocKeyBlockPages(key_blk_page_t *pages) {
	key_blk_page_t *p;
	while (pages) {
		p = pages;
		pages = pages->nxtpage;
		free(p->content);
		p->prvpage = NULL;
		p->nxtpage = NULL;
		free(p);
		p = NULL;
	}
	pages = NULL;
}

