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
#include "convert.h"

/*
 * this is time axis b+ tree index data structure.
 *
 * Created on: 2020年10月18日
 * Author: Dahai CAO
 */

/**
 * Time axis b+tree's order. B+tree's order of
 * leaf node is same as that of non-leaf node. But the information stores
 * in two nodes is different. Non-leaf nodes including root node, store keys (size_t, 8 bytes) and
 * children (subtree pointer) (size_t, 8 bytes), while leaf nodes store keys (size_t, 8 bytes)
 * and data pointer (non-cluster index) (size_t, 8 bytes). If m is order, that means
 * leaf node has m-1 data and m-1 keys, while non-leaf's nodes has m-1 keys and m children.
 *
 * Each node includes key number (integer), leaf mark(1 byte)
 * that is, if order is m, the bytes of a record  in index db is:
 * leaf bytes = length(int) + leaf(char) + ts(0~m-1 B) + data(0~m-1 B) + prev(8 B) + nxt(8 B)
 * non-leaf bytes = length(int) + leaf(char) + ts(0~m-1 B) + children(0~m-1 B) + prev(8 B) + nxt(8 B)
 *
 * NOTE: however, bytes of leaf node are same as bytes of non leaf node.
 * total bytes = 4 + 1 + (m-1)*8 + (m)*8 + 2*8 per node(page)
 *
 * B+tree 一般规律，分裂总是叶子分裂为叶子，非叶子分裂为非叶子
 * 合并是叶子合并完孩子叶子，非叶子合并完还是非叶子
 *
 * Dahai Cao designed at 9:01am on 2020-10-28
 */
// this is the order of leaf nodes in b+tree.
long TA_BPLUS_TREE_ORDER;
// time axis index page expiration time stamp, configurable in .conf file
size_t ta_bptree_idx_leng_leaf_bytes;
size_t ta_bptree_idx_node_bytes;
size_t ta_bptree_idx_keys_bytes;
size_t ta_bptree_idx_children_bytes;
size_t start_pointer;

// this structure describes non-cluster B+ tree index
// node for time axis index, one page is one node
typedef struct ta_bptree_index_node {
	long long id;
	unsigned char dirty; // if dirty, then 1; otherwise, 0;
	unsigned char hit; // hit counting, 0 by default, hit once, plus 1;
	unsigned char *content; // buffer in memory for b+tree index. including keys and tu index Ids or children ids
	long long expiretime; // expiration time stamp, manager will check it to destroy for page swap
	long long *keys; //
	long long *tuIdxIds; //
	long long *chldrnIds; //
	long long next; //
	long long prev; //
	int num; /* 关键字个数 */
	unsigned char leaf;
	struct ta_bptree_index_node **child; /* 子结点：所占空间为（max+2）- 多出来的1个空间用于交换空间使用 */
	struct ta_bptree_index_node *parent; /* 父结点 */
	struct ta_bptree_index_node *nxt; /* 下一个叶子结点 */
	struct ta_bptree_index_node *prv; /* 下一个叶子结点 */
	struct ta_bptree_index_node *nxtpage; /* next page for deallocation */
} ta_idx_node_t;

typedef struct ta_index {
	int max; /* 单个节点最大关键字个数 */
	int min; /* 单个节点最小关键字个数 */
	ta_idx_node_t *root; /* B树根结点地址 */
	ta_idx_node_t *minLeaf;
	ta_idx_node_t *maxLeaf;
	int sidx;/* 分裂索引 = (max+1)/2 */
	long long rtId;
	long long maxLf;
	long long minLf;
} ta_idx_t;

ta_idx_t *ta_idx;

void print_ta_index(ta_idx_t *btree);

void initTaIndexMemPages(ta_idx_t *ta_idx, FILE *ta_idx_db_fp, FILE *ta_idx_id_fp);

ta_idx_t* taIndexRootCreate(int m);

ta_idx_node_t* taIndexCreateNode(ta_idx_t *bptree, unsigned char leaf,
		FILE *ta_id_db_fp);

ta_idx_node_t* taIndexInsertNode(ta_idx_t *bptree, long long ts, long long tuid,
		FILE *ta_idx_id_fp, FILE *ta_idx_db_fp);

void commitIndexNode(ta_idx_t *btree, FILE *ta_db_fp);

int ta_bptree_merge(ta_idx_t *bptree, ta_idx_node_t *node);

int taIndexDeleteNode(ta_idx_t *bptree, long long ts, long long tuid,
		FILE *ta_db_fp);



#endif /* TABPTREEIDX_H_ */
