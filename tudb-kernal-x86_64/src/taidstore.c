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

#include "taidstore.h"

/*
 * taidstore.c
 *
 * Created on: 2020年6月16日
 * Author: Dahai CAO
 */
long long getOneId(id_cache_t *cache) {
	if (cache->rId != NULL) {
		id_t *p = cache->rId;
		if (p != NULL) {
			while (p->nxt != NULL) {
				if (p->nxt->nxt == NULL) {
					break;
				}
				p = p->nxt;
			}
		}
		long long r = p->id;
		p = NULL;
		return r;
	} else if (cache->nId != NULL) {
		id_t *p = cache->nId;
		if (p != NULL) {
			long long r = p->id;
			cache->nId = p->nxt;
			p = NULL;
			return r;
		} else {
			p = NULL;
			return 0;
		}
	}
	return 0;

}

void loadIds(FILE *idfp, id_cache_t *cache) {
	int c = 25; // cache reused Id capacity. 100 by default.

	// step 1: load new Ids
	unsigned char ids[LONG_LONG] = { 0 };
	// fetch a new Id
	fseek(idfp, 0, SEEK_SET); // move file pointer to file head
	fread(ids, sizeof(unsigned char), LONG_LONG, idfp); // read the first bytes
	long long newid = ByteArrayToLong(ids); // convert array to long long
	for (int i = 0; i < c; i++) {
		id_t *p = cache->nId;
		if (p != NULL) {
			while (p->nxt != NULL) {
				p = p->nxt;
			}
			p->nxt = (id_t*) malloc(sizeof(id_t));
			p->nxt->id = newid;
			p->nxt->nxt = NULL;
		} else {
			cache->nId = (id_t*) malloc(sizeof(id_t));
			cache->nId->id = newid;
			cache->nId->nxt = NULL;
		}
		newid += 1;
	}
	// update the next free Id.
	LongToByteArray(newid, ids); // convert
	fseek(idfp, 0, SEEK_SET); // move file pointer to file end
	fwrite(ids, sizeof(unsigned char), LONG_LONG, idfp);

	// step 2: load reused Ids
	fseek(idfp, LONG_LONG, SEEK_SET);
	fread(ids, sizeof(unsigned char), LONG_LONG, idfp);

	long long s0 = ByteArrayToLong(ids);
	if (s0 != 0) { // there reused Id
		// 16 means 2 * LONG_LONG Bytes for long long
		long long rest = (s0 + 1) * LONG_LONG - 2 * LONG_LONG;
		long bc = LONG_LONG * c;		// byte capacity to fetch
		if (rest > LONG_LONG * c) {
			// only read LONG_LONG * c bytes
			// p is position of byte
			long long p = (s0 + 1) * LONG_LONG - bc;
			fseek(idfp, p, SEEK_SET);
			unsigned char *buf = (unsigned char*) malloc(
					sizeof(unsigned char) * bc);
			memset(buf, 0, sizeof(unsigned char) * bc);
			fread(buf, sizeof(unsigned char), bc, idfp);
			for (int i = 0; i < c; i++) {
				memset(ids, 0, LONG_LONG);
				for (int j = 0; j < LONG_LONG; j++) {
					ids[j] = buf[i * LONG_LONG + j];
				}
				long long d = ByteArrayToLong(ids);
				// insert one at end of queue
				id_t *p = cache->rId;
				if (p != NULL) {
					while (p->nxt != NULL) {
						p = p->nxt;
					}
					p->nxt = (id_t*) malloc(sizeof(id_t));
					p->nxt->id = d;
					p->nxt->nxt = NULL;
				} else {
					cache->rId = (id_t*) malloc(sizeof(id_t));
					cache->rId->id = d;
					cache->rId->nxt = NULL;
				}
			}
			// clean all reused Ids in DB
			fseek(idfp, p, SEEK_SET);
			memset(buf, 0, sizeof(unsigned char) * bc);
			fwrite(buf, sizeof(unsigned char), bc, idfp);
			free(buf);
			// update the second pointer
			fseek(idfp, LONG_LONG, SEEK_SET);
			// last reused Id
			LongToByteArray((p - LONG_LONG) / LONG_LONG, ids);
			fwrite(ids, sizeof(unsigned char), LONG_LONG, idfp);
		} else {
			// all read
			fseek(idfp, 2 * LONG_LONG, SEEK_SET);
			unsigned char *buf = (unsigned char*) malloc(
					sizeof(unsigned char) * rest);
			memset(buf, 0, sizeof(unsigned char) * rest);
			fread(buf, sizeof(unsigned char), rest, idfp);
			for (int i = 0; i < rest / LONG_LONG; i++) {
				memset(ids, 0, LONG_LONG);
				for (int j = 0; j < LONG_LONG; j++) {
					ids[j] = buf[i * LONG_LONG + j];
				}
				long long d = ByteArrayToLong(ids);
				// insert one at end of queue
				id_t *p = cache->rId;
				if (p != NULL) {
					while (p->nxt != NULL) {
						p = p->nxt;
					}
					p->nxt = (id_t*) malloc(sizeof(id_t));
					p->nxt->id = d;
					p->nxt->nxt = NULL;
				} else {
					cache->rId = (id_t*) malloc(sizeof(id_t));
					cache->rId->id = d;
					cache->rId->nxt = NULL;
				}
			}
			fseek(idfp, 2 * LONG_LONG, SEEK_SET);		// clean all reused Ids
			memset(buf, 0, sizeof(unsigned char) * rest);
			fwrite(buf, sizeof(unsigned char), rest, idfp);
			free(buf);
			// reset reused Id at 8th byte
			fseek(idfp, LONG_LONG, SEEK_SET);
			memset(ids, 0, LONG_LONG);
			fwrite(ids, sizeof(unsigned char), LONG_LONG, idfp);
		}
	}
}

void recycleOneId(long long id, id_cache_t *cache) {
	id_t *p = cache->rId;
	if (p != NULL) {
		while (p->nxt != NULL) {
			p = p->nxt;
		}
		p->nxt = (id_t*) malloc(sizeof(id_t));
		p->nxt->id = id;
		p->nxt->nxt = NULL;
	} else {
		cache->rId = (id_t*) malloc(sizeof(id_t));
		cache->rId->id = id;
		cache->rId->nxt = NULL;
	}
}

void listAllTaIds(id_cache_t *cache) {
	int count = 0;
	id_t *p = cache->nId;
	while (p != NULL) {
		count++;
		p = p->nxt;
	}
	printf("DB size:%d\n", count);
	printf("------------\n");
	p = cache->nId;
	int i = 0;
	while (p != NULL) {
		printf("%d, %lld\n", i, p->id);
		p = p->nxt;
		i++;
	}
	printf("------------\n");
	printf("New Id queue length:%d\n", i);

	int count1 = 0;
	id_t *p1 = cache->rId;
	while (p1 != NULL) {
		count1++;
		p1 = p1->nxt;
	}
	printf("DB size:%d\n", count1);
	printf("------------\n");
	p1 = cache->rId;
	int i1 = 0;
	while (p1 != NULL) {
		printf("%d, %lld\n", i1, p1->id);
		p1 = p1->nxt;
		i1++;
	}
	printf("------------\n");
	printf("Reused Id queue length:%d\n", i1);
}



/*long long getId(FILE *idfp) {
	unsigned char ids[LONG_LONG] = { 0 };
	unsigned char zero[LONG_LONG] = { 0 };
	// get one id from id DB, update new id to db
	fseek(idfp, LONG_LONG, SEEK_SET);
	fread(ids, sizeof(unsigned char), LONG_LONG, idfp);
	long long s0 = ByteArrayToLong(ids);
	if (s0 == 0) { // no reused Id
		// fetch a new Id
		fseek(idfp, 0, SEEK_SET); // move file pointer to file head
		fread(ids, sizeof(unsigned char), LONG_LONG, idfp); // read the first bytes
		long long newid = ByteArrayToLong(ids); // convert array to long long
		// update the next free Id.
		LongToByteArray((newid + 1), ids); // convert
		fseek(idfp, 0L, SEEK_SET); // move file pointer to file end
		fwrite(ids, sizeof(unsigned char), LONG_LONG, idfp);
		return newid;
	} else if (s0 == 2) { // there is one reused Id
		fseek(idfp, LONG_LONG * 2, SEEK_SET); // move file pointer to the 16th byte
		fread(ids, sizeof(unsigned char), LONG_LONG, idfp); // read a byte array
		long long reusedId = ByteArrayToLong(ids);
		// remove and clear the reused Id
		fseek(idfp, LONG_LONG * 2, SEEK_SET);
		fwrite(zero, sizeof(unsigned char), LONG_LONG, idfp);
		// remove and clear the second long long
		fseek(idfp, LONG_LONG, SEEK_SET);
		fwrite(zero, sizeof(unsigned char), LONG_LONG, idfp);
		return reusedId;
	} else if (s0 > 2) { // there are more than one reused Ids
		// get one reused Id
		fseek(idfp, s0 * LONG_LONG, SEEK_SET);
		fread(ids, sizeof(unsigned char), LONG_LONG, idfp);
		long long reusedId = ByteArrayToLong(ids);
		// remove and clear the reused Id
		fseek(idfp, s0 * LONG_LONG, SEEK_SET);
		fwrite(zero, sizeof(unsigned char), LONG_LONG, idfp);
		// update the second long long
		fseek(idfp, LONG_LONG, SEEK_SET);
		LongToByteArray((s0 - 1), ids);
		fwrite(ids, sizeof(unsigned char), LONG_LONG, idfp);
		return reusedId;
	}
	return 0;
}

void recycleId(FILE *idfp, long long id) {
	unsigned char ids[LONG_LONG] = { 0L };
	unsigned char buf[LONG_LONG] = { 0L };
	fseek(idfp, LONG_LONG, SEEK_SET);
	fread(ids, sizeof(unsigned char), LONG_LONG, idfp);
	long long s0 = ByteArrayToLong(ids);
	if (s0 == 0L) {
		LongToByteArray(id, buf); // convert
		fseek(idfp, LONG_LONG * 2, SEEK_SET); // move file pointer from file head
		fwrite(buf, sizeof(unsigned char), LONG_LONG, idfp); // write

		LongToByteArray(2L, buf); // convert
		fseek(idfp, LONG_LONG, SEEK_SET); // move file pointer the 8th byte
		// update the second long long
		fwrite(buf, sizeof(unsigned char), LONG_LONG, idfp);
	} else {
		LongToByteArray(id, buf);
		fseek(idfp, (s0 + 1) * LONG_LONG, SEEK_SET);
		fwrite(buf, sizeof(unsigned char), LONG_LONG, idfp);

		LongToByteArray((s0 + 1), buf);
		fseek(idfp, LONG_LONG, SEEK_SET); // move file pointer the 8th byte
		// update the second long long
		fwrite(buf, sizeof(unsigned char), LONG_LONG, idfp);
	}
}

void showAllTaIds(FILE *idfp) {
	unsigned char buf[LONG_LONG] = { 0 };
	fseek(idfp, 0L, SEEK_END);
	long sz = ftell(idfp);
	printf("Db size:%ld bytes\n", sz);
	printf("------------\n");
	int i = 0;
	while (!feof(idfp) && (i * LONG_LONG) < sz) { // pageable to read
		fseek(idfp, i * LONG_LONG, SEEK_SET);
		memset(buf, 0, LONG_LONG);
		fread(buf, sizeof(unsigned char), LONG_LONG, idfp);
		long long s0 = ByteArrayToLong(buf);
		// If you are on windows and using MingW, GCC uses the win32 runtime,
		// where printf needs %I64d for a 64 bit integer.
		// (and %I64u for an unsigned 64 bit integer)
		//printf("%I64d\n", s0);
		printf("%d, %lld\n", i, s0);
		i++;
	}
	printf("------------\n");
	printf("Reused Id queue length:%d\n", (i - 2));
}*/
