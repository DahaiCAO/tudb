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
	if (cache->h_rId == NULL) {
		loadReusedIds(id_fp, cache, id_length);
	}
	if (cache->h_rId != NULL) {
		id_t *p = cache->h_rId;
		if (p != NULL) {
			long long r = p->id;
			cache->h_rId = p->nxt; // head
			p->nxt = NULL;
			p->prv = NULL;
			free(p);
			p = NULL;
			if (cache->h_rId == NULL) {
				cache->t_rId = NULL;
			}
			return r;
		}
	}
	if (cache->h_nId == NULL) {
		loadNewIds(id_fp, cache, id_length);
	}
	id_t *head = cache->h_nId;
	if (head != NULL) {
		long long id = head->id;
		cache->h_nId = head->nxt; // head
		head->nxt = NULL;
		head->prv = NULL;
		free(head);
		head = NULL;
		if (cache->h_nId == NULL) {
			cache->t_nId = NULL;
		}
		return id;
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
	for (int i = 0; i < id_len; i++) {
		id_t *tail = cache->t_nId;
		if (tail != NULL) {
			tail->nxt = (id_t*) malloc(sizeof(id_t));
			tail->nxt->id = newid;
			tail->nxt->nxt = NULL;
			tail->nxt->prv = tail;
			cache->t_nId = tail->nxt;
			tail = NULL;
		} else {
			cache->h_nId = (id_t*) malloc(sizeof(id_t));
			cache->h_nId->id = newid;
			cache->h_nId->nxt = NULL;
			cache->h_nId->prv = NULL;
			cache->t_nId = cache->h_nId;
		}
		newid += 1;
	}
	// update the next free Id.
	longToByteArray(newid, ids); // convert
	fseek(id_fp, 0, SEEK_SET); // move file pointer to file end
	fwrite(ids, sizeof(unsigned char), LONG_LONG, id_fp);
}

/*
 * 按顺序读出，按相反顺序插入队列
 */
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
			unsigned char *buf = (unsigned char*) calloc(bc,
					sizeof(unsigned char));
			fread(buf, sizeof(unsigned char), bc, id_fp);
			for (int i = 0; i < id_len; i++) {
				long long reusedid = bytesLonglong(buf + i * LONG_LONG);
				// insert one at head of queue
				// 按顺序读出，按相反顺序插入队列
				if (cache->h_rId != NULL) {
					cache->h_rId->prv = (id_t*) malloc(sizeof(id_t));
					cache->h_rId->prv->id = reusedid;
					cache->h_rId->prv->prv = NULL;
					cache->h_rId->prv->nxt = cache->h_rId;
					cache->h_rId = cache->h_rId->prv;
				} else {
					cache->h_rId = (id_t*) malloc(sizeof(id_t));
					cache->h_rId->id = reusedid;
					cache->h_rId->nxt = NULL;
					cache->h_rId->prv = NULL;
					cache->t_rId = cache->h_rId;
				}
			}
			// clean all reused Ids in DB
			fseek(id_fp, p, SEEK_SET);
			memset(buf, 0, sizeof(unsigned char) * bc);
			fwrite(buf, sizeof(unsigned char), bc, id_fp);
			free(buf);
			buf = NULL;
			// update the second pointer
			fseek(id_fp, LONG_LONG, SEEK_SET);
			// last reused Id
			longToByteArray((p - LONG_LONG) / LONG_LONG, ids);
			fwrite(ids, sizeof(unsigned char), LONG_LONG, id_fp);
		} else {
			// all read
			fseek(id_fp, 2 * LONG_LONG, SEEK_SET);
			unsigned char *buf = (unsigned char*) calloc(rest,
					sizeof(unsigned char));
			fread(buf, sizeof(unsigned char), rest, id_fp);
			for (int i = 0; i < rest / LONG_LONG; i++) {
				long long reusedid = bytesLonglong(buf + i * LONG_LONG);
				// insert one at head of queue
				// 按顺序读出，按相反顺序插入队列
				if (cache->h_rId != NULL) {
					cache->h_rId->prv = (id_t*) malloc(sizeof(id_t));
					cache->h_rId->prv->id = reusedid;
					cache->h_rId->prv->prv = NULL;
					cache->h_rId->prv->nxt = cache->h_rId;
					cache->h_rId = cache->h_rId->prv;
				} else {
					cache->h_rId = (id_t*) malloc(sizeof(id_t));
					cache->h_rId->id = reusedid;
					cache->h_rId->nxt = NULL;
					cache->h_rId->prv = NULL;
					cache->t_rId = cache->h_rId;
				}
			}
			fseek(id_fp, 2 * LONG_LONG, SEEK_SET);		// clean all reused Ids
			memset(buf, 0, sizeof(unsigned char) * rest);
			fwrite(buf, sizeof(unsigned char), rest, id_fp);
			free(buf);
			buf = NULL;
			// reset reused Id at the 8th byte
			memset(ids, 0, LONG_LONG);
			fseek(id_fp, LONG_LONG, SEEK_SET);
			fwrite(ids, sizeof(unsigned char), LONG_LONG, id_fp);
		}
	}

}
// 将超过回收队列2倍以上的ID存储到DB中，
// 存储一个id_length的ID.
// 从队尾开始存起，队尾的ID作为存储缓存的低位ID
static int storeRest(id_cache_t *cache, size_t id_length, FILE *id_fp) {
	id_t *t = cache->t_rId;
	unsigned char *buf = calloc(id_length * LONG_LONG, sizeof(unsigned char));
	int count = 0;
	while (count < id_length) {
		longlongtoByteArray(t->id, buf + count * LONG_LONG);
		// deallocate the id memory
		cache->t_rId = t->prv;
		t->nxt = NULL;
		t->prv = NULL;
		free(t);
		t = cache->t_rId;
		count++;
	}
	// store the buf to DB
	unsigned char ids[LONG_LONG] = { 0 };
	fseek(id_fp, LONG_LONG, SEEK_SET);
	fread(ids, sizeof(unsigned char), LONG_LONG, id_fp);
	long long last = byteArrayToLong(ids); // get last Id position
	// calculate physical last ID position
	last = (last + 2) * LONG_LONG;
	fseek(id_fp, last, SEEK_SET);
	fwrite(buf, sizeof(unsigned char), id_length * LONG_LONG, id_fp);
	free(buf);
	buf = NULL;
	last += id_length * LONG_LONG;
	last = last / LONG_LONG - 1;
	longToByteArray(last, ids);
	// reset reused Id at the 8th byte
	fseek(id_fp, LONG_LONG, SEEK_SET);
	fwrite(ids, sizeof(unsigned char), LONG_LONG, id_fp);
	t = NULL;
}

void recycleOneId(long long id, id_cache_t *cache, size_t id_length,
		FILE *id_fp) {
	id_t *t = cache->t_rId;
	if (t != NULL) {
		t->nxt = (id_t*) malloc(sizeof(id_t));
		t->nxt->id = id;
		t->nxt->nxt = NULL;
		t->nxt->prv = t;
		cache->t_rId = t->nxt;
		// 下面注释是因为队头是在不断被取走，
		// 因此cache->h_rId在变化，下面的算法存在问题。
//		id_t *t = cache->h_rId;
//		int count = 0;
//		while (t != cache->t_rId) {
//			if (count <= 2 * id_length) {
//				count++;
//				t = t->nxt;
//			} else {
//				break;
//			}
//		}
//		if (count > 2 * id_length) {
//			storeRest(cache, id_length, id_fp);
//		}
		t = NULL;
	} else {
		cache->h_rId = (id_t*) malloc(sizeof(id_t));
		cache->h_rId->id = id;
		cache->h_rId->nxt = NULL;
		cache->h_rId->prv = NULL;
		cache->t_rId = cache->h_rId;
	}
}

// 分别将new ID和reused ID归还到DB，
// 确保ID资源不丢失。
// 这个方法肯定是在服务器关机时候调用
int returnCachedIDtoDB(id_cache_t *cache, FILE *id_fp) {
	id_t *h = cache->h_nId;
	int length = 0, count = 0;
	long long last = 0;
	unsigned char ids[LONG_LONG] = { 0 };

	// store new ID to DB
	longToByteArray(h->id, ids);
	fseek(id_fp, 0, SEEK_SET);
	fwrite(ids, sizeof(unsigned char), LONG_LONG, id_fp);

	// store reused ID to DB
	id_t *t = cache->t_rId;
	while (t != cache->h_rId) {
		if (t) {
			t = t->prv;
		}
		length++;
	}
	length++;		// 获取到总长度
	unsigned char *buf = calloc(length * LONG_LONG, sizeof(unsigned char));
	count = 0;
	t = cache->t_rId;
	if (t) {
		while (count < length) {
			longlongtoByteArray(t->id, buf + count * LONG_LONG);
			// deallocate the id memory
			t = t->prv;
			count++;
		}
	}
	// store the buf to DB
	fseek(id_fp, LONG_LONG, SEEK_SET);
	fread(ids, sizeof(unsigned char), LONG_LONG, id_fp);
	last = byteArrayToLong(ids); // get last Id position
	// calculate physical last ID position
	last = (last + 2) * LONG_LONG;
	fseek(id_fp, last, SEEK_SET);
	fwrite(buf, sizeof(unsigned char), length * LONG_LONG, id_fp);
	free(buf);
	buf = NULL;
	last += length * LONG_LONG;
	last = last / LONG_LONG - 1;
	longToByteArray(last, ids);
	// reset reused Id at the 8th byte
	fseek(id_fp, LONG_LONG, SEEK_SET);
	fwrite(ids, sizeof(unsigned char), LONG_LONG, id_fp);
	t = NULL;
	h = NULL;
	return 0;
}

void listAllIds(id_cache_t *cache) {
	int count = 0;
	id_t *p = cache->h_nId;
	while (p != NULL) {
		count++;
		p = p->nxt;
	}
	printf("DB size:%d\n", count);
	printf("------------\n");
	p = cache->h_nId;
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
	id_t *p1 = cache->h_rId;
	while (p1 != NULL) {
		count1++;
		p1 = p1->nxt;
	}
	printf("DB size:%d\n", count1);
	printf("------------\n");
	p1 = cache->h_rId;
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

void deallocIdCache(id_t *ids) {
	id_t *p;
	while (ids) {
		p = ids;
		ids = ids->nxt;
		p->nxt = NULL;
		p->prv = NULL;
		free(p);
	}
	p = NULL;
	ids = NULL;
}

void initIdCaches(id_caches_t *caches) {
	caches->taIdxIds = (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->taIdxIds->h_nId = NULL;
	caches->taIdxIds->h_rId = NULL;
	caches->taIdxIds->t_nId = NULL;
	caches->taIdxIds->t_rId = NULL;

	caches->teIds = (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->teIds->h_nId = NULL;
	caches->teIds->h_rId = NULL;
	caches->teIds->t_nId = NULL;
	caches->teIds->t_rId = NULL;

	caches->lblIdxIds = (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->lblIdxIds->h_nId = NULL;
	caches->lblIdxIds->h_rId = NULL;
	caches->lblIdxIds->t_nId = NULL;
	caches->lblIdxIds->t_rId = NULL;

	caches->lblBlkIds = (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->lblBlkIds->h_nId = NULL;
	caches->lblBlkIds->h_rId = NULL;
	caches->lblBlkIds->t_nId = NULL;
	caches->lblBlkIds->t_rId = NULL;

	caches->lblsIds = (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->lblsIds->h_nId = NULL;
	caches->lblsIds->h_rId = NULL;
	caches->lblsIds->t_nId = NULL;
	caches->lblsIds->t_rId = NULL;

	caches->keyIdxIds = (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->keyIdxIds->h_nId = NULL;
	caches->keyIdxIds->h_rId = NULL;
	caches->keyIdxIds->t_nId = NULL;
	caches->keyIdxIds->t_rId = NULL;

	caches->keyBlkIds = (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->keyBlkIds->h_nId = NULL;
	caches->keyBlkIds->h_rId = NULL;
	caches->keyBlkIds->t_nId = NULL;
	caches->keyBlkIds->t_rId = NULL;

	caches->valIdxIds = (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->valIdxIds->h_nId = NULL;
	caches->valIdxIds->h_rId = NULL;
	caches->valIdxIds->t_nId = NULL;
	caches->valIdxIds->t_rId = NULL;

	caches->valIds = (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->valIds->h_nId = NULL;
	caches->valIds->h_rId = NULL;
	caches->valIds->t_nId = NULL;
	caches->valIds->t_rId = NULL;

}

void deallocIdCaches(id_caches_t *caches) {
	deallocIdCache(caches->taIdxIds->h_nId);
	deallocIdCache(caches->taIdxIds->h_rId);
	caches->taIdxIds->t_nId = NULL;
	caches->taIdxIds->t_rId = NULL;
	free(caches->taIdxIds);

	deallocIdCache(caches->teIds->h_nId);
	deallocIdCache(caches->teIds->h_rId);
	caches->teIds->t_nId = NULL;
	caches->teIds->t_rId = NULL;
	free(caches->teIds);

	deallocIdCache(caches->lblIdxIds->h_nId);
	deallocIdCache(caches->lblIdxIds->h_rId);
	caches->lblIdxIds->t_nId = NULL;
	caches->lblIdxIds->t_rId = NULL;
	free(caches->lblIdxIds);

	deallocIdCache(caches->lblBlkIds->h_nId);
	deallocIdCache(caches->lblBlkIds->h_rId);
	caches->lblBlkIds->t_nId = NULL;
	caches->lblBlkIds->t_rId = NULL;
	free(caches->lblBlkIds);

	deallocIdCache(caches->lblsIds->h_nId);
	deallocIdCache(caches->lblsIds->h_rId);
	caches->lblsIds->t_nId = NULL;
	caches->lblsIds->t_rId = NULL;
	free(caches->lblsIds);

	deallocIdCache(caches->keyIdxIds->h_nId);
	deallocIdCache(caches->keyIdxIds->h_rId);
	caches->keyIdxIds->t_nId = NULL;
	caches->keyIdxIds->t_rId = NULL;
	free(caches->keyIdxIds);

	deallocIdCache(caches->keyBlkIds->h_nId);
	deallocIdCache(caches->keyBlkIds->h_rId);
	caches->keyBlkIds->t_nId = NULL;
	caches->keyBlkIds->t_rId = NULL;
	free(caches->keyBlkIds);

	deallocIdCache(caches->valIdxIds->h_nId);
	deallocIdCache(caches->valIdxIds->h_rId);
	caches->valIdxIds->t_nId = NULL;
	caches->valIdxIds->t_rId = NULL;
	free(caches->valIdxIds);

	deallocIdCache(caches->valIds->h_nId);
	deallocIdCache(caches->valIds->h_rId);
	caches->valIds->t_nId = NULL;
	caches->valIds->t_rId = NULL;
	free(caches->valIds);

	free(caches);
}
