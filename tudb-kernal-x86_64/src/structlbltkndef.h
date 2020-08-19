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
 * structlbltkndef.h
 *
 * Created on: 2020年8月13日
 * Author: Dahai CAO
 */

#ifndef STRUCTLBLTKNDEF_H_
#define STRUCTLBLTKNDEF_H_

typedef struct label_token_page {
	long long expiretime; // expiration time to mark this page, -1 by default, that means blank page
	unsigned char *content; // buffer for page content.
	long long startNo;
	long long start; // start byte on this page
	long long end; // end byte on this page
	unsigned char dirty; // if dirty, then 1; otherwise, 0;
	int hit; // hit counting, 0 by default, hit once, plus 1;
	struct label_token_page *prvpage;
	struct label_token_page *nxtpage;
} lbl_tkn_page_t;

typedef struct label_token {
	long long id; // pointer in DB
	long long taId; //time axis Id, it is for evolution
	unsigned char inUse; // 1: in use; otherwise, 0
	int len;
	struct label_token *nxtBlkId; // next block
	char *blkContent;
	lbl_tkn_page_t *page;
} lbl_tkn_t;

#endif /* STRUCTLBLTKNDEF_H_ */
