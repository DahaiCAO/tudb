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
 * tabptreeidx.h
 *
 * Created on: 2020年10月18日
 * Author: Dahai CAO
 */

#ifndef TABPTREEIDX_H_
#define TABPTREEIDX_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "macrodef.h"
#include "tuidstore.h"
#include "macrodef.h"

/*
 * this is time axis b+ tree index data structure.
 *
 * Created on: 2020年10月18日
 * Author: Dahai CAO
 */

long TA_BPLUS_TREE_M;
size_t tm_axis_bptree_node_bytes;
// time axis page expiration time (minutes), configurable in .conf file
int TIME_AXIS_NODE_EXPIRE_TIME;

// this structure describes non-cluster B+ tree index node for time axis index, one page is one node
typedef struct ta_bptree_index_node {
	long long id;
	long long expiretime; // expiration time to mark this page, -1 by default, that means blank page
	int dirty; // if dirty, then 1; otherwise, 0;
	int hit; // hit counting, 0 by default, hit once, plus 1;
	unsigned char *content; // buffer for page content. including keys Id and children ids
	int num; /* 关键字个数 */
	//long long *ts; /* 关键字：所占空间为(max+1) - 多出来的1个空间用于交换空间使用 */
	//long long *tuIdxId; /*  */
	//long long *childId; /*  */
	//long long *prv; /*  */
	//long long *nxt; /*  */
	struct ta_bptree_index_node **child; /* 子结点：所占空间为（max+2）- 多出来的1个空间用于交换空间使用 */
	struct ta_bptree_index_node *parent; /* 父结点 */
	struct ta_bptree_index_node *nxt; /* 下一个叶子结点 */
	struct ta_bptree_index_node *prv; /* 下一个叶子结点 */
	struct ta_bptree_index_node *nxtpage; /* next page for deallocation */
} ta_bptree_idx_node_t;

typedef struct ta_bptree {
	int max; /* 单个结点最大关键字个数 - 阶m=max+1 */
	int min; /* 单个结点最小关键字个数 */
	int sidx; /* 分裂索引 = (max+1)/2 */
	ta_bptree_idx_node_t *root; /* B树根结点地址 */
} ta_bptree_idx_t;

ta_bptree_idx_t * ta_bptree_idx;

// the convert utility for long long to byte array
void longlongToByteArray(long long value, unsigned char *buffer);

// the utility for converting byte array to long long
long long byteArrayToLonglong(unsigned char *buffer);

ta_bptree_idx_node_t* loadTaBptreeNode(ta_bptree_idx_t *btree,
		unsigned char *page, long long *id, ta_bptree_idx_node_t *parent);

ta_bptree_idx_node_t* readTaBpTreeNode(ta_bptree_idx_t *btree, long long start,
		size_t size, ta_bptree_idx_node_t *parent, FILE *ta_bptree_idx_db_fp);

// initially load time axis index database
// m should be 3 at least in B+tree, if m is 2, the tree is two-b tree
void loadTaIndex(ta_bptree_idx_t* ta_bptree_idx,FILE *ta_bptree_idx_db_fp);

void deallocTaBptreeIndex(ta_bptree_idx_t* ta_bptree_idx);

int splitTaBptreeNode(ta_bptree_idx_t *btree, ta_bptree_idx_node_t *node,
		FILE *ta_bptree_idx_db_fp, FILE *ta_id_fp);

int insertBptreeNode(ta_bptree_idx_t *btree, ta_bptree_idx_node_t *node,
		long long ts, int idx, long long tuIdxId);

long long insertTaBptreeNode(ta_bptree_idx_t *btree, long long ts, long long tuid,
		FILE *ta_bptree_idx_db_fp, FILE *ta_id_fp);

#endif /* TABPTREEIDX_H_ */
