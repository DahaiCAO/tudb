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
#include "lbltknstore.h"

/*
 * lbltknstore.c
 *
 * Created on: 2020年8月13日
 * Author: Dahai CAO
 */

/* read one label token page
 * start the start pointer in memory
 * start_no the start record id in this page
 * lbl_tkn_db_fp label token DB file
 */
static lbl_tkn_page_t* readOneLabelTokenPage(lbl_tkn_page_t *pages, long long start,
		long long start_no, FILE *lbl_tkn_db_fp) {
	unsigned char *page = (unsigned char*) malloc(
			sizeof(unsigned char) * lbl_tkn_page_bytes);
	memset(page, 0, sizeof(unsigned char) * lbl_tkn_page_bytes);
	fseek(lbl_tkn_db_fp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(lbl_tkn_db_fp)) != EOF) {
		fseek(lbl_tkn_db_fp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), lbl_tkn_page_bytes, lbl_tkn_db_fp); // read one page
	}

	lbl_tkn_page_t *p = (lbl_tkn_page_t*) malloc(sizeof(lbl_tkn_page_t));
	p->dirty = 0;
	p->expiretime = LABEL_TOKEN_PAGE_EXPIRE_TIME; // 10 minutes
	p->startNo = start_no;
	p->hit = 0;
	p->nxtpage = NULL;
	p->prvpage = NULL;
	p->content = page;
	p->start = start;
	p->end = start + lbl_tkn_page_bytes;

	lbl_tkn_page_t *pp = pages;
	if (pp != NULL) {
		while (pp->nxtpage != NULL) {
			pp = pp->nxtpage;
		}
		pp->nxtpage = p;
		p->prvpage = pp;
		pp = NULL;
	} else {
		lbl_tkn_pages = p;
	}
	return p;
}

void initLabelTokenDBMemPages(lbl_tkn_page_t *pages, FILE *lbl_tkn_db_fp) {
	// read the first page from 0 pointer.
	readOneLabelTokenPage(pages, 0LL, 0LL, lbl_tkn_db_fp);
}


/**
 * Local variables are automatically freed when the function ends,
 * you don't need to free them by yourself. You only free dynamically
 * allocated memory (e.g using malloc) as it's allocated on the heap:
 * char *arr = malloc(3 * sizeof(char));
 * strcpy(arr, "bo");
 * // ...
 * free(arr);
 * refer to: https://en.wikipedia.org/wiki/C_dynamic_memory_allocation
 */
void commitLabelToken(long long ta_id, lbl_tkn_t **list, FILE *lbl_tkn_db_fp,
		FILE *lbl_tkn_id_fp) {
	lbl_tkn_t **t = list;
	// assign a ID to every token block
	int j = 0;
	t = list;
	while (*(t + j) != NULL) {
		(*(t + j))->id = getOneId(lbl_tkn_id_fp, caches->lbltknIds,
				LABEL_ID_QUEUE_LENGTH);
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
		lbl_tkn_page_t *ps = lbl_tkn_pages;
		bool found = false;
		unsigned char *pos;
		while (!found) {
			while (ps != NULL) {
				if (ps->startNo <= (*(t + j))->id
						&& (*(t + j))->id
								< ps->startNo + LABEL_TOKEN_PAGE_RECORDS) {
					pos = ps->content
							+ ((*(t + j))->id - ps->startNo)
									* lbl_tkn_record_bytes;
					// convert label token block to byte array
					unsigned char ta_ids[LONG_LONG] = { 0 };
					LongToByteArray(ta_id, ta_ids);			// ta Id
					unsigned char length[LONG] = { 0 };
					Integer2Bytes((*(t + j))->len, length);
					unsigned char nblockId[LONG_LONG] = { 0 };
					LongToByteArray((*(t + j))->nxtBlkId, nblockId);// next block Id
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
					fseek(lbl_tkn_db_fp, (*(t + j))->id * lbl_tkn_record_bytes,
					SEEK_SET); //
					fwrite(pos, sizeof(unsigned char), lbl_tkn_record_bytes,
							lbl_tkn_db_fp);
					found = true;
					pos = NULL;
					break;
				}
				ps = ps->nxtpage;
			}
			if (!found) {
				// read a new page
				long long pagenum = ((*(t + j))->id * lbl_tkn_record_bytes)
						/ lbl_tkn_page_bytes;
				readOneLabelTokenPage(lbl_tkn_pages,
						pagenum * lbl_tkn_page_bytes,
						pagenum * LABEL_TOKEN_PAGE_RECORDS, lbl_tkn_db_fp);
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

lbl_tkn_t** divideLabelTokens(unsigned char *label) {
	lbl_tkn_t **list;
	unsigned char *tmpbuf = (unsigned char*) calloc(LABEL_BUFFER_LENGTH,
			sizeof(unsigned char));
	convert2Utf8((char*) label, (char*) tmpbuf, LABEL_BUFFER_LENGTH);
	size_t l = strlen((const char*) tmpbuf);
	if (l > LABEL_BLOCK_LENGTH) {
		size_t s = l / LABEL_BLOCK_LENGTH;
		size_t y = l % LABEL_BLOCK_LENGTH;
		lbl_tkn_t **p;
		if (y > 0) {
			list = (lbl_tkn_t**) calloc(s + 2, sizeof(lbl_tkn_t*));// one more for 0x0
		} else {
			list = (lbl_tkn_t**) calloc(s + 1, sizeof(lbl_tkn_t*));// one more for 0x0
		}
		p = list;
		for (int i = 0; i < s; i++) {
			unsigned char *buf = (unsigned char*) calloc(LABEL_BLOCK_LENGTH,
					sizeof(unsigned char));
			memcpy(buf, tmpbuf + i * LABEL_BLOCK_LENGTH, LABEL_BLOCK_LENGTH);
			lbl_tkn_t *tkn = (lbl_tkn_t*) malloc(sizeof(lbl_tkn_t));
			tkn->blkContent = buf;
			tkn->id = NULL_POINTER;
			tkn->inUse = 1;
			tkn->len = LABEL_BLOCK_LENGTH;
			tkn->page = NULL;
			tkn->taId = 0;
			tkn->nxtBlkId = NULL_POINTER;
			*p = tkn;
			p = p + 1;
			buf = NULL;
			tkn = NULL;
		}
		if (y > 0) {
			unsigned char *buf = (unsigned char*) calloc(LABEL_BLOCK_LENGTH,
					sizeof(unsigned char));
			memcpy(buf, tmpbuf + s * LABEL_BLOCK_LENGTH, y);
			lbl_tkn_t *tkn = (lbl_tkn_t*) malloc(sizeof(lbl_tkn_t));
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
		list = (lbl_tkn_t**) calloc(1, sizeof(lbl_tkn_t));
		lbl_tkn_t *tkn = (lbl_tkn_t*) malloc(sizeof(lbl_tkn_t));
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

lbl_tkn_t ** searchLabelTokenList(long long id, FILE *lbl_tkn_db_fp) {
	lbl_tkn_t **list = NULL;
	lbl_tkn_page_t *ps = lbl_tkn_pages;
	int i = 1;
	unsigned char *pos;
	long long tId = id;
	while (tId != NULL_POINTER) {
		while (ps != NULL) {
			if (ps->startNo <= tId
					&& tId < ps->startNo + LABEL_TOKEN_PAGE_RECORDS) {
				pos = ps->content + (tId - ps->startNo) * lbl_tkn_record_bytes;
				lbl_tkn_t *tkn = (lbl_tkn_t*) malloc(sizeof(lbl_tkn_t));
				unsigned char *buf = (unsigned char*) calloc(
						lbl_tkn_record_bytes, sizeof(unsigned char));
				memcpy(buf, pos, lbl_tkn_record_bytes);
				tkn->page = ps;
				tkn->id = tId;
				unsigned char ta_ids[LONG_LONG] = { 0 };
				memcpy(ta_ids, buf, LONG_LONG);
				tkn->taId = ByteArrayToLong(ta_ids); // ta id

				tkn->inUse = *(buf + LONG_LONG); // in use

				unsigned char length[LONG] = { 0 };
				memcpy(length, buf + LONG_LONG + 1, LONG);
				tkn->len = Bytes2Integer(length); // block length

				unsigned char nblockId[LONG_LONG] = { 0 };
				memcpy(nblockId, (buf + LONG_LONG + 1 + LONG), LONG_LONG);
				tkn->nxtBlkId = ByteArrayToLong(nblockId); // next block id

				unsigned char *blockContent = (unsigned char*) calloc(
						LABEL_BLOCK_LENGTH, sizeof(unsigned char)); // block content
				memcpy(blockContent, buf + LONG_LONG + 1 + LONG + LONG_LONG,
						tkn->len);
				tkn->blkContent = blockContent;
				list = (lbl_tkn_t **)realloc(list, sizeof(lbl_tkn_t*) * i);
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
			long long pagenum = (tId * lbl_tkn_record_bytes)
					/ lbl_tkn_page_bytes;
			readOneLabelTokenPage(lbl_tkn_pages, pagenum * lbl_tkn_page_bytes,
					pagenum * LABEL_TOKEN_PAGE_RECORDS, lbl_tkn_db_fp);
			continue;
		} else {
			break;
		}
	}
	list = (lbl_tkn_t **)realloc(list, sizeof(lbl_tkn_t*) * i);
	*(list + i - 1) = 0x0;// add one zero after the last element.
	ps = NULL;
	pos = NULL;
	return list;
}

unsigned char* findLabelToken(long long id, FILE *lbl_tkn_db_fp) {
	lbl_tkn_t **list = searchLabelTokenList(id, lbl_tkn_db_fp);
	// combine the label blocks to one label string.
	int j = 0;
	lbl_tkn_t **p = list;
	int l = 0;
	while (*(p + j)) { // calculate total label string length
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
	deallocLabelTokenList(list);
	return ct;
}

void deleteLabelToken(long long id, FILE *lbl_tkn_db_fp) {
	lbl_tkn_t **list = searchLabelTokenList(id, lbl_tkn_db_fp);
	int j = 0;
	lbl_tkn_t **p = list;
	while (*(p + j)) {
		unsigned char *pos = ((*(p + j))->page)->content
				+ ((*(p + j))->id - ((*(p + j))->page)->startNo)
						* lbl_tkn_record_bytes;
		*(pos + LONG_LONG) = 0x0;
		recycleOneId((*(p + j))->id, caches->lbltknIds);
		// update to DB
		fseek(lbl_tkn_db_fp, (*(p + j))->id * lbl_tkn_record_bytes + LONG_LONG,
				SEEK_SET);
		unsigned char inuse[1] = { 0x0 };
		fwrite(inuse, sizeof(unsigned char), 1, lbl_tkn_db_fp);
		pos = NULL;
		j++;
	}
	deallocLabelTokenList(list);
	p = NULL;
	list = NULL;
}

// this update operation is used to update and override new label to old label
void commitUpdateLabelToken(lbl_tkn_t **list, lbl_tkn_t **newlist,
		FILE *lbl_tkn_id_fp, FILE *lbl_tkn_db_fp) {
	// c is old list length
	// j is new list length
	lbl_tkn_t **l = list;
	lbl_tkn_t **t = newlist;
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
				(*(t + k))->id = getOneId(lbl_tkn_id_fp, caches->lbltknIds,
						LABEL_ID_QUEUE_LENGTH);
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
		lbl_tkn_page_t *ps = lbl_tkn_pages;
		bool found = false;
		unsigned char *pos;
		while (!found) {
			while (ps != NULL) {
				if (ps->startNo <= (*(t + k))->id
						&& (*(t + k))->id
								< ps->startNo + LABEL_TOKEN_PAGE_RECORDS) {
					pos = ps->content
							+ ((*(t + k))->id - ps->startNo)
									* lbl_tkn_record_bytes;
					unsigned char ta_ids[LONG_LONG] = { 0 };
					LongToByteArray((*(t + k))->taId, ta_ids);			// ta Id
					memcpy(pos, ta_ids, LONG_LONG);

					unsigned char inuse[1] = { (*(t + k))->inUse };
					memcpy(pos + LONG_LONG, inuse, 1);		// in use flag

					unsigned char length[LONG] = { 0 };
					Integer2Bytes((*(t + k))->len, length);
					memcpy(pos + LONG_LONG + 1, length, LONG); // block length

					unsigned char nblockId[LONG_LONG] = { 0 };
					LongToByteArray((*(t + k))->nxtBlkId, nblockId); // next block Id
					memcpy(pos + LONG_LONG + 1 + LONG, nblockId,
					LONG_LONG);

					memcpy(pos + LONG_LONG + 1 + LONG + LONG_LONG,
							(*(t + k))->blkContent, (*(t + k))->len); // block content
					// update to DB
					fseek(lbl_tkn_db_fp, (*(t + k))->id * lbl_tkn_record_bytes,
					SEEK_SET); //
					fwrite(pos, sizeof(unsigned char), lbl_tkn_record_bytes,
							lbl_tkn_db_fp);
					pos = NULL;
					found = true;
					break;
				}
				ps = ps->nxtpage;
			}
			if (!found) {
				// read a new page
				long long pagenum = ((*(t + k))->id * lbl_tkn_record_bytes)
						/ lbl_tkn_page_bytes;
				readOneLabelTokenPage(lbl_tkn_pages,
						pagenum * lbl_tkn_page_bytes,
						pagenum * LABEL_TOKEN_PAGE_RECORDS, lbl_tkn_db_fp);
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
							* lbl_tkn_record_bytes;
			*(pos + LONG_LONG) = 0x0;
			recycleOneId((*(l + k))->id, caches->lbltknIds);
			// update to DB
			fseek(lbl_tkn_db_fp,
					(*(l + k))->id * lbl_tkn_record_bytes + LONG_LONG,
					SEEK_SET);
			unsigned char inuse[1] = { 0x0 };
			fwrite(inuse, sizeof(unsigned char), 1, lbl_tkn_db_fp);
			pos = NULL;
			k++;
		}
	}
	l = NULL;
	t = NULL;
}

void combineLabelTokens(lbl_tkn_t **list, int length) {
	int j = 0;
	lbl_tkn_t ** p = list;
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

// deallocate label token list
void deallocLabelTokenList(lbl_tkn_t **list) {
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

// deallocate label token memory pages
void deallocLabelTokenPages(lbl_tkn_page_t *pages) {
	lbl_tkn_page_t *p;
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
