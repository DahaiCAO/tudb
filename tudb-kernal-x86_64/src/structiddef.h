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

typedef struct iddef {
	long long id;
	struct iddef *nxt;
} id_t;

// the cache for new free and reused Ids
typedef struct id_cache {
	id_t *nId; // new Id array
	id_t *rId; // reused Id array
} id_cache_t;

typedef struct id_caches {
	id_cache_t *taIds;
	id_cache_t *teIds;
	id_cache_t *lblidxIds;
	id_cache_t *lblblkIds;
	id_cache_t *lblsIds;
	id_cache_t *keyidxIds;
	id_cache_t *keyblkIds;
	id_cache_t *valIdxIds;
	id_cache_t *valIds;
} id_caches_t;

id_caches_t *caches;

#endif /* STRUCTIDDEF_H_ */
