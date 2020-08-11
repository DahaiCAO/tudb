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
 * tabtreestore.h
 *
 * Created on: 2020年8月2日
 * Author: Dahai CAO
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef TABTREESTORE_H_
#define TABTREESTORE_H_

// the B-Tree node and memory page are combined
typedef struct _btree_node_t {
	long long expiretime; // expiration time to mark this page, -1 by default, that means blank page
	//unsigned char *content; // buffer for page content, 100 time axis records.
	long long startNo;
	//long long start; // start byte on this page
	//long long end; // end byte on this page
	unsigned char dirty; // if dirty, then 1; otherwise, 0;
	int hit; // hit counting, 0 by default, hit once, plus 1;

	int num; /* 关键字个数 */
	int *key; /* 关键字：所占空间为(max+1) - 多出来的1个空间用于交换空间使用 */
	struct _btree_node_t **child; /* 子结点：所占空间为（max+2）- 多出来的1个空间用于交换空间使用 */
	struct _btree_node_t *parent; /* 父结点 */
} ta_btree_node_t;

typedef struct {
	int max; /* 单个结点最大关键字个数 - 阶m=max+1 */
	int min; /* 单个结点最小关键字个数 */
	int sidx; /* 分裂索引 = (max+1)/2 */
	ta_btree_node_t *root; /* B树根结点地址 */
} ta_btree_t;

ta_btree_node_t* readOneBTreePage(long long start, FILE *tadbfp);

int btree_create(ta_btree_t **_btree, int m, FILE *tadbfp);

int btree_split(ta_btree_t *btree, ta_btree_node_t *node);

ta_btree_node_t* btree_create_node(ta_btree_t *btree);

int _btree_insert(ta_btree_t *btree, ta_btree_node_t *node, int key,
		int idx);

int btree_insert(ta_btree_t *btree, int key);




#endif /* TABTREESTORE_H_ */
