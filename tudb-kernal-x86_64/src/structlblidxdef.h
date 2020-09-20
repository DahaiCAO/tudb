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
 * structlblidxdef.h
 *
 * Created on: 2020年8月13日
 * Author: Dahai CAO
 */

#ifndef STRUCTLBLIDXDEF_H_
#define STRUCTLBLIDXDEF_H_

typedef struct label_index_page {
	long long expiretime; // expiration time to mark this page, -1 by default, that means blank page
	unsigned char *content; // buffer for page content.
	long long startNo;
	long long start; // start byte on this page
	long long end; // end byte on this page
	unsigned char dirty; // if dirty, then 1; otherwise, 0;
	int hit; // hit counting, 0 by default, hit once, plus 1;
	struct label_index_page *prvpage;
	struct label_index_page *nxtpage;
} lbl_idx_page_t;

typedef struct label_index {
	long long id; // pointer in DB
	long long lblCount; // It counts the usage of the label token in whole Tu DB
	long long lblTknId; //
	int codingType;
	int length;
	long long taId;
	lbl_idx_page_t *page;
} lbl_idx_t;

#endif /* STRUCTLBLIDXDEF_H_ */
