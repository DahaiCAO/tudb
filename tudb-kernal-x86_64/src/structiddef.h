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

/*
 * structiddef.h
 *
 * Created on: 2020年6月21日
 * Author: Dahai CAO
 */

#ifndef STRUCTIDDEF_H_
#define STRUCTIDDEF_H_

// double linked list
typedef struct iddef {
	long long id;
	struct iddef *nxt;
	struct iddef *prv;
} id_t;

// the cache for new free and reused Id queue
// this is a First In First Out structure (FIFO)
typedef struct id_cache {
	id_t *h_nId; // head of new Id queue
	id_t *t_nId; // tail of new Id queue
	id_t *h_rId; // head of reused Id queue
	id_t *t_rId; // tail of reused Id queue
} id_cache_t;

// Id cache structure
typedef struct id_caches {
	id_cache_t *taIdxIds; // time axis index Id queue
	id_cache_t *teIds; // Tu element Id queue
	id_cache_t *lblIdxIds; // label index Id queue
	id_cache_t *lblBlkIds; // label block Id queue
	id_cache_t *lblsIds; // label array Id queue (double linked list)
	id_cache_t *keyIdxIds; // key index Id queue
	id_cache_t *keyBlkIds; // key block Id queue
	id_cache_t *valIdxIds; // value index Id queue
	id_cache_t *valIds; // value Id queue
} id_caches_t;

id_caches_t *caches; // global Id caches

#endif /* STRUCTIDDEF_H_ */
