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
 * structkeyidxdef.h
 *
 * Created on: 2020年9月27日
 * Author: Dahai CAO
 */

#ifndef STRUCTKEYIDXDEF_H_
#define STRUCTKEYIDXDEF_H_

// Double-Array Trie tree node of property name (key) index
typedef struct datire_tree_index_nd {
	long long id; // pointer in DB
	long long transferRatio; // 转移基数
	unsigned char symbol; // symbol character. it is a UTF-8 code.
	unsigned char leaf; //是否为叶子节点 ，1, leaf, otherwise, 0.
	long long tuIdxId; // The first Tu element Id of a Id list.
} dat_idx_nd_t;

typedef struct key_idex_page {
	long long expiretime; // expiration time to mark this page, -1 by default, that means blank page
	int hit; // hit counting, 0 by default, hit once, plus 1;
	dat_idx_nd_t **base; // base array
	long long start; // page start number
	long long *check; // check array
	unsigned char *baseContent; // buffer for base array
	unsigned char *checkContent; // buffer for check array
	unsigned char dirty; // if dirty, then 1; otherwise, 0;
	struct key_idex_page *prvPage;
	struct key_idex_page *nxtPage;
} key_idx_page_t;

// current state page
typedef struct currrent_state_page {
	long long curstat; // current state
	key_idx_page_t *curpge; // the page where current state is
} cur_stat_page_t;

#endif /* STRUCTKEYIDXDEF_H_ */
