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
 * structlblsdef.h
 *
 * Created on: 2020年9月20日
 * Author: Dahai CAO
 */

#ifndef STRUCTLBLSDEF_H_
#define STRUCTLBLSDEF_H_

typedef struct labels_page {
	long long expiretime; // expiration time to mark this page, -1 by default, that means blank page
	unsigned char *content; // buffer for page content.
	long long startNo;
	long long start; // start byte on this page
	long long end; // end byte on this page
	unsigned char dirty; // if dirty, then 1; otherwise, 0;
	int hit; // hit counting, 0 by default, hit once, plus 1;
	struct labels_page *prvpage;
	struct labels_page *nxtpage;
} lbls_page_t;

typedef struct labels {
	long long id; // pointer in DB
	long long nxtLblsId; // It counts the usage of the label token in whole Tu DB
	long long prvLblsId; //
	long long taId;
	unsigned char inuse;
	long long lblIdxId; //s
	lbls_page_t *page;
} lbls_t;

#endif /* STRUCTLBLSDEF_H_ */
