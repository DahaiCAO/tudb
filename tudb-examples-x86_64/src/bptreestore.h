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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/*
 * bptreestore.h
 *
 * Created on: 2020年10月20日
 * Author: Dahai CAO
 */

#ifndef BPTREESTORE_H_
#define BPTREESTORE_H_

// the B-Tree node and memory page are combined
typedef struct _bptree_node_t {
	/* 关键字的实际个数 */
	int num;
	/* 关键字：所占空间为(max+1) - 多出来的1个空间用于交换空间使用 */
	int *key;
	/* 子结点：所占空间为(max+2)- 多出来的1个空间用于交换空间使用 [m/2,m] */
	struct _bptree_node_t **child;
	struct _bptree_node_t *parent; /* 父结点 */
	struct _bptree_node_t *prev; /* 前一个结点 */
	struct _bptree_node_t *next; /* 后一个结点 */
	unsigned char leaf;
} ta_bptree_node_t;

typedef struct _bptree {
	int max; /* 单个结点最大关键字个数 - 阶m=max+1 */
	int min; /* 单个结点最小关键字个数    */
	int sidx; /* 分裂索引 = (max)/2 */
	/*叶子*/
	int sidxLeaf; /* 叶子分裂索引 = (max)/4 */
	ta_bptree_node_t *root; /* B树根结点地址 */
	ta_bptree_node_t *minLeaf;
	ta_bptree_node_t *maxLeaf;
	long long rtId;
	long long maxLf;
	long long minLf;
} ta_bptree_t;

void print_bptree(ta_bptree_t *btree);

void print_bptree_children(ta_bptree_node_t *node, int level);

int bptree_create(ta_bptree_t **_bptree, int m);

ta_bptree_node_t* bptree_create_node(ta_bptree_t *bptree);

// node is split as a left child node
int bptree_split(ta_bptree_t *bptree, ta_bptree_node_t *node);

int _bptree_insert(ta_bptree_t *bptree, ta_bptree_node_t *node, int key,
		int idx);

int bptree_insert(ta_bptree_t *bptree, int key);

//int _bptree_delete_nonleaf(ta_bptree_t *bptree, ta_bptree_node_t *node, int idx);

int _bptree_merge(ta_bptree_t *btree, ta_bptree_node_t *left,
		ta_bptree_node_t *right, int mid);

int bptree_merge(ta_bptree_t *bptree, ta_bptree_node_t *node);

int _bptree_delete_leaf(ta_bptree_t *bptree, ta_bptree_node_t *node, int idx);

int bptree_delete(ta_bptree_t *bptree, int key);

#endif /* BPTREESTORE_H_ */
