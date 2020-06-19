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
 * globalvars.h
 *
 * Created on: 2020年6月19日
 * Author: Dahai CAO
 */

#ifndef STRUCTDEF_H_
#define STRUCTDEF_H_

#define BUFFERSIZE 8L //
#define PAGESIZE 2500L // read page size.
#define EPSIZE 25L // evolved point size on time axis (25 bytes)
#define LONG_LONG 8L

typedef struct iddef {
	long long id;
	struct iddef *nxt;
} id_t;

// the cache for new free and reused Ids
typedef struct id_cache {
	id_t *nId;// new Id array
	id_t *rId;// reused Id array
} id_cache_t;

typedef struct evolved_point {
	unsigned char inUse; // if in use, 0: not in use, otherwise, 1: yes.
	long long prvTsId; // previous evolved point(time stamp) Id
	long long nxtTsId; // next evolved point(time stamp) Id
	long long time; // time stamp
} evolved_point_t;

// time axis read buffer for update
typedef struct ta_page {
	long long expiretime; // expiration time to mark this page, -1 by default, that means blank page
	unsigned char *content; // buffer for page content, 100 time axis records.
	long long firstTsId; // first evolved point(time stamp) Id in this page
	long long nextTsId; // next evolved point(time stamp) Id in this page
	unsigned char duty; // if duty, then 1; otherwise, 0;
	int hited; // hit counting, 0 by default, hit once, plus 1;
} ta_page_t;

typedef struct ta_book {
	ta_page_t *pages; // 10 page by default.
} ta_book_t;

// memory page manager
typedef struct tu_lib {
	ta_book_t *tabook; //
} tu_lib;

id_cache_t *cache;

#endif /* STRUCTDEF_H_ */




