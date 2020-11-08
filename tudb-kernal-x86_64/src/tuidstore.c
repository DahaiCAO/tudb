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

#include "tuidstore.h"

/*
 * Tu object Id store
 *
 * Created on: 2020年6月16日
 * Author: Dahai CAO
 */
long long getOneId(FILE *id_fp, id_cache_t *cache, size_t id_length) {
	if (cache->rId == NULL) {
		loadReusedIds(id_fp, cache, id_length);
	}
	if (cache->rId != NULL) {
		id_t *p = cache->rId;
		if (p != NULL) {
			long long r = p->id;
			cache->rId = p->nxt;
			p = NULL;
			return r;
		}
	}
	if (cache->nId == NULL) {
		loadNewIds(id_fp, cache, id_length);
	}
	id_t *p = cache->nId;
	if (p != NULL) {
		long long r = p->id;
		cache->nId = p->nxt;
		p = NULL;
		return r;
	}
	return 0;
}

void loadAllIds(FILE *id_fp, id_cache_t *cache, size_t id_length) {
	loadNewIds(id_fp, cache, id_length);
	loadReusedIds(id_fp, cache, id_length);
}

void loadNewIds(FILE *id_fp, id_cache_t *cache, size_t id_length) {
	size_t id_len = ID_QUEUE_LENGTH;
	if (id_length > 0) {
		id_len = id_length;
	}
	// step 1: load new Ids
	unsigned char ids[LONG_LONG] = { 0 };
	// fetch a new Id
	fseek(id_fp, 0, SEEK_SET); // move file pointer to file head
	fread(ids, sizeof(unsigned char), LONG_LONG, id_fp); // read the first bytes
	long long newid = byteArrayToLong(ids); // convert array to long long
	for (int i = 0; i < ID_QUEUE_LENGTH; i++) {
		id_t *p = cache->nId;
		if (p != NULL) {
			while (p->nxt != NULL) {
				p = p->nxt;
			}
			p->nxt = (id_t*) malloc(sizeof(id_t));
			p->nxt->id = newid;
			p->nxt->nxt = NULL;
			p = NULL;
		} else {
			cache->nId = (id_t*) malloc(sizeof(id_t));
			cache->nId->id = newid;
			cache->nId->nxt = NULL;
		}
		newid += 1;
	}
	// update the next free Id.
	longToByteArray(newid, ids); // convert
	fseek(id_fp, 0, SEEK_SET); // move file pointer to file end
	fwrite(ids, sizeof(unsigned char), LONG_LONG, id_fp);
}

void loadReusedIds(FILE *id_fp, id_cache_t *cache, size_t id_length) {
	size_t id_len = ID_QUEUE_LENGTH;
	if (id_length > 0) {
		id_len = id_length;
	}
	// step 2: load reused Ids
	unsigned char ids[LONG_LONG] = { 0 };
	fseek(id_fp, LONG_LONG, SEEK_SET);
	fread(ids, sizeof(unsigned char), LONG_LONG, id_fp);
	long long s0 = byteArrayToLong(ids);
	if (s0 != 0) { // there reused Id
		// 16 means 2 * LONG_LONG Bytes for long long
		long long rest = (s0 + 1) * LONG_LONG - 2 * LONG_LONG;
		long bc = LONG_LONG * id_len;		// byte capacity to fetch
		if (rest > bc) {
			// only read LONG_LONG * c bytes
			// p is position of byte
			long long p = (s0 + 1) * LONG_LONG - bc;
			fseek(id_fp, p, SEEK_SET);
			unsigned char *buf = (unsigned char*) malloc(
					sizeof(unsigned char) * bc);
			memset(buf, 0, sizeof(unsigned char) * bc);
			fread(buf, sizeof(unsigned char), bc, id_fp);
			for (int i = 0; i < id_len; i++) {
				memset(ids, 0, LONG_LONG);
				for (int j = 0; j < LONG_LONG; j++) {
					ids[j] = buf[i * LONG_LONG + j];
				}
				long long d = byteArrayToLong(ids);
				// insert one at end of queue
				id_t *p = cache->rId;
				if (p != NULL) {
					while (p->nxt != NULL) {
						p = p->nxt;
					}
					p->nxt = (id_t*) malloc(sizeof(id_t));
					p->nxt->id = d;
					p->nxt->nxt = NULL;
					p = NULL;
				} else {
					cache->rId = (id_t*) malloc(sizeof(id_t));
					cache->rId->id = d;
					cache->rId->nxt = NULL;
				}
			}
			// clean all reused Ids in DB
			fseek(id_fp, p, SEEK_SET);
			memset(buf, 0, sizeof(unsigned char) * bc);
			fwrite(buf, sizeof(unsigned char), bc, id_fp);
			free(buf);
			// update the second pointer
			fseek(id_fp, LONG_LONG, SEEK_SET);
			// last reused Id
			longToByteArray((p - LONG_LONG) / LONG_LONG, ids);
			fwrite(ids, sizeof(unsigned char), LONG_LONG, id_fp);
		} else {
			// all read
			fseek(id_fp, 2 * LONG_LONG, SEEK_SET);
			unsigned char *buf = (unsigned char*) malloc(
					sizeof(unsigned char) * rest);
			memset(buf, 0, sizeof(unsigned char) * rest);
			fread(buf, sizeof(unsigned char), rest, id_fp);
			for (int i = 0; i < rest / LONG_LONG; i++) {
				memset(ids, 0, LONG_LONG);
				for (int j = 0; j < LONG_LONG; j++) {
					ids[j] = buf[i * LONG_LONG + j];
				}
				long long d = byteArrayToLong(ids);
				// insert one at end of queue
				id_t *p = cache->rId;
				if (p != NULL) {
					while (p->nxt != NULL) {
						p = p->nxt;
					}
					p->nxt = (id_t*) malloc(sizeof(id_t));
					p->nxt->id = d;
					p->nxt->nxt = NULL;
					p = NULL;
				} else {
					cache->rId = (id_t*) malloc(sizeof(id_t));
					cache->rId->id = d;
					cache->rId->nxt = NULL;
				}
			}
			fseek(id_fp, 2 * LONG_LONG, SEEK_SET);		// clean all reused Ids
			memset(buf, 0, sizeof(unsigned char) * rest);
			fwrite(buf, sizeof(unsigned char), rest, id_fp);
			free(buf);
			// reset reused Id at the 8th byte
			memset(ids, 0, LONG_LONG);
			fseek(id_fp, LONG_LONG, SEEK_SET);
			fwrite(ids, sizeof(unsigned char), LONG_LONG, id_fp);
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
		p = NULL;
	} else {
		cache->rId = (id_t*) malloc(sizeof(id_t));
		cache->rId->id = id;
		cache->rId->nxt = NULL;
	}
}

void listAllIds(id_cache_t *cache) {
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
	p = NULL;
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
	p1 = NULL;
	printf("------------\n");
	printf("Reused Id queue length:%d\n", i1);
}

int returnIdtoDB(id_caches_t * caches) {
	return 0;
}

void deallocIdCache(id_t *ids) {
	id_t *p;
	while (ids) {
		p = ids;
		ids = ids->nxt;
		free(p);
		p = NULL;
	}
	ids = NULL;
}

void initIdCaches(id_caches_t * caches) {
	caches->taIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->taIds->nId = NULL;
	caches->taIds->rId = NULL;
	caches->teIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->teIds->nId = NULL;
	caches->teIds->rId = NULL;
	caches->lblidxIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->lblidxIds->nId = NULL;
	caches->lblidxIds->rId = NULL;
	caches->lblblkIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->lblblkIds->nId = NULL;
	caches->lblblkIds->rId = NULL;
	caches->lblsIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->lblsIds->nId = NULL;
	caches->lblsIds->rId = NULL;
	caches->keyidxIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->keyidxIds->nId = NULL;
	caches->keyidxIds->rId = NULL;
	caches->keyblkIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->keyblkIds->nId = NULL;
	caches->keyblkIds->rId = NULL;
	caches->valIdxIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->valIdxIds->nId = NULL;
	caches->valIdxIds->rId = NULL;
	caches->valIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->valIds->nId = NULL;
	caches->valIds->rId = NULL;

}

void deallocIdCaches(id_caches_t * caches) {
	deallocIdCache(caches->taIds->nId);
	deallocIdCache(caches->taIds->rId);
	free(caches->taIds);
	deallocIdCache(caches->teIds->nId);
	deallocIdCache(caches->teIds->rId);
	free(caches->teIds);
	deallocIdCache(caches->lblidxIds->nId);
	deallocIdCache(caches->lblidxIds->rId);
	free(caches->lblidxIds);
	deallocIdCache(caches->lblblkIds->nId);
	deallocIdCache(caches->lblblkIds->rId);
	free(caches->lblblkIds);
	deallocIdCache(caches->lblsIds->nId);
	deallocIdCache(caches->lblsIds->rId);
	free(caches->lblsIds);
	deallocIdCache(caches->keyidxIds->nId);
	deallocIdCache(caches->keyidxIds->rId);
	free(caches->keyidxIds);
	deallocIdCache(caches->keyblkIds->nId);
	deallocIdCache(caches->keyblkIds->rId);
	free(caches->keyblkIds);
	free(caches);
}
