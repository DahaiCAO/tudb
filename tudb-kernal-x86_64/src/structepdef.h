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

#ifndef STRUCTEPDEF_H_
#define STRUCTEPDEF_H_

// time axis read buffer for update
typedef struct ta_page {
	long long expiretime; // expiration time to mark this page, -1 by default, that means blank page
	unsigned char *content; // buffer for page content, 100 time axis records.
	long long startNo;
	long long start; // start byte on this page
	long long end; // end byte on this page
	unsigned char dirty; // if dirty, then 1; otherwise, 0;
	int hit; // hit counting, 0 by default, hit once, plus 1;
	struct ta_page *prvpage;
	struct ta_page *nxtpage;
} ta_page_t;

typedef struct evolved_point {
	long long id; //
	long long prvTsId; // previous evolved point(time stamp) Id
	long long nxtTsId; // next evolved point(time stamp) Id
	long long time; // time stamp
	unsigned char *pos;
	unsigned char inuse; // 1: in use; othewise, 0
	ta_page_t *page;
} evolved_point_t;

typedef struct tadb_head {
	long long first; // address of the first time axis record
	unsigned char firstdirty;
	long long last; // address of the second time axis record
	unsigned char lastdirty;
} tadb_head_t;

typedef struct ta_buf {
	long long first; // address of the first time axis record
	long long last; // address of the second time axis record
	ta_page_t *pages; // 10 pages by default.
} ta_buf_t;

typedef struct idbuf {
	long long id;
	struct idbuf *nxt;
} idbuf_t;

#endif /* STRUCTEPDEF_H_ */

