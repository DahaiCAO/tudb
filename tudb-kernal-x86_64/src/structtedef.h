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
 * structtedef.h
 *
 * This file is to define the Tu element struct
 *
 * Created on: 2020年8月11日
 * Author: Dahai CAO
 */

#ifndef STRUCTTEDEF_H_
#define STRUCTTEDEF_H_

typedef struct te_page {
	long long expiretime; // expiration time to mark this page, -1 by default, that means blank page
	unsigned char *content; // buffer for page content.
	long long startNo;
	long long start; // start byte on this page
	long long end; // end byte on this page
	unsigned char dirty; // if dirty, then 1; otherwise, 0;
	int hit; // hit counting, 0 by default, hit once, plus 1;
	struct te_page *prvpage;
	struct te_page *nxtpage;
} te_page_t;

typedef struct tu_element {
	long long id; // pointer in DB
	long long taid; //time axis Id, it is for evolution
	long long nxtRelId; // previous Tu relationship Id
	long long nxtPropId; // next property Id
	long long prvTeId; // previous Tu element Id, it is for evolution, -2 by default
	long long nxtTeId; // next Tu element Id, it is for evolution, -2 by default
	long long lblId; // label Id
	unsigned char *pos;
	unsigned char inuse; // 1: in use; othewise, 0
	unsigned char extra; // for extra use
	te_page_t *page;
} tu_element_t;
#endif /* STRUCTTEDEF_H_ */
