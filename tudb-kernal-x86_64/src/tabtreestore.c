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
#include "tabtreestore.h"
/*
 * tabtreestore.c
 *
 * Created on: 2020年8月2日
 * Author: Dahai CAO
 */

long long TA_PAGE_SIZE = 10;

// read one page
ta_btree_node_t* readOneBTreePage(long long start, FILE *tadbfp) {
	unsigned char *page = (unsigned char*) malloc(
			sizeof(unsigned char) * TA_PAGE_SIZE);
	memset(page, 0, sizeof(unsigned char) * TA_PAGE_SIZE);
	fseek(tadbfp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(tadbfp)) != EOF) {
		fseek(tadbfp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), TA_PAGE_SIZE, tadbfp); // read one page
	}

	ta_btree_node_t *p = (ta_btree_node_t*) malloc(sizeof(ta_btree_node_t));
	p->dirty = 0;
	p->expiretime = 10; // 10 minutes
//	p->startNo = startNo;
	p->hit = 0;
	//p->key = page;
//	p->start = start;
//	p->end = start + TA_PAGE_SIZE;
//
//	ta_page_t *pp = timeaxispages->pages;
//	if (pp != NULL) {
//		while (pp->nxtpage != NULL) {
//			pp = pp->nxtpage;
//		}
//		pp->nxtpage = p;
//		p->prvpage = pp;
//	} else {
//		timeaxispages->pages = p;
//	}
	return p;
}

// m should be 3 at least in B-tree, if m is 2, the tree is two-b tree
int btree_create(ta_btree_t **_btree, int m, FILE *tadbfp) {
	ta_btree_t *btree = NULL;
	btree = (ta_btree_t*) calloc(1, sizeof(ta_btree_t));
	btree->max = m - 1;
	btree->min = m / 2;
	if (0 != m % 2) { // Math.ceil(m/2)
		btree->min++;
	}
	btree->min = btree->min - 1; // 4
	btree->sidx = m / 2;
	btree->root = NULL; /* empty tree */
	*_btree = btree;
	return 0;
}

ta_btree_node_t* btree_create_node(ta_btree_t *btree) {
	ta_btree_node_t *node = NULL;
	node = (ta_btree_node_t*) calloc(1, sizeof(ta_btree_node_t));
	node->num = 0;
	/* More than (max) is for move */
	node->key = (int*) calloc(btree->max + 1, sizeof(int));
	/* More than (max+1) is for move */
	node->child = (ta_btree_node_t**) calloc(btree->max + 2,
			sizeof(ta_btree_node_t*));
	return node;
}

int btree_split(ta_btree_t *btree, ta_btree_node_t *node) {
	int idx = 0, total = 0, sidx = btree->sidx;
	ta_btree_node_t *parent = NULL, *node2 = NULL;

	while (node->num > btree->max) {
		/* Split node */
		total = node->num;

		node2 = btree_create_node(btree);
		if (NULL == node2) {
			fprintf(stderr, "[%s][%d] Create node failed!\n", __FILE__,
			__LINE__);
			return -1;
		}

		/* Copy data */
		memcpy(node2->key, node->key + sidx + 1,
				(total - sidx - 1) * sizeof(int));
		memcpy(node2->child, node->child + sidx + 1,
				(total - sidx) * sizeof(ta_btree_node_t*));

		node2->num = (total - sidx - 1);
		node2->parent = node->parent;

		node->num = sidx;
		/* Insert into parent */
		parent = node->parent;
		if (NULL == parent) {
			/* Split root node */
			parent = btree_create_node(btree);
			if (NULL == parent) {
				fprintf(stderr, "[%s][%d] Create root failed!", __FILE__,
				__LINE__);
				return -1;
			}

			btree->root = parent;
			parent->child[0] = node;
			node->parent = parent;
			node2->parent = parent;

			parent->key[0] = node->key[sidx];
			parent->child[1] = node2;
			parent->num++;
		} else {
			/* Insert into parent node */
			for (idx = parent->num; idx > 0; idx--) {
				if (node->key[sidx] < parent->key[idx - 1]) {
					parent->key[idx] = parent->key[idx - 1];
					parent->child[idx + 1] = parent->child[idx];
					continue;
				}
				break;
			}

			parent->key[idx] = node->key[sidx];
			parent->child[idx + 1] = node2;
			node2->parent = parent;
			parent->num++;
		}

		memset(node->key + sidx, 0, (total - sidx) * sizeof(int));
		memset(node->child + sidx + 1, 0,
				(total - sidx) * sizeof(ta_btree_node_t*));

		/* Change node2's child->parent */
		for (idx = 0; idx <= node2->num; idx++) {
			if (NULL != node2->child[idx]) {
				node2->child[idx]->parent = node2;
			}
		}
		node = parent;
	}

	return 0;
}

int _btree_insert(ta_btree_t *btree, ta_btree_node_t *node, int key, int idx) {
	int i = 0;

	/* 1. 移动关键字:首先在最底层的某个非终端结点上插入一个关键字,因此该结点无孩子结点，故不涉及孩子指针的移动操作 */
	for (i = node->num; i > idx; i--) {
		node->key[i] = node->key[i - 1];
	}

	node->key[idx] = key; /* 插入 */
	node->num++;

	/* 2. 分裂处理 */
	if (node->num > btree->max) {
		return btree_split(btree, node);
	}

	return 0;
}

int btree_insert(ta_btree_t *btree, int key) {
	int idx = 0;
	ta_btree_node_t *node = btree->root;

	/* 1. 构建第一个结点 */
	if (NULL == node) {
		node = btree_create_node(btree);
		if (NULL == node) {
			fprintf(stderr, "[%s][%d] Create node failed!\n", __FILE__,
			__LINE__);
			return -1;
		}

		node->num = 1;
		node->key[0] = key;
		node->parent = NULL;

		btree->root = node;
		return 0;
	}

	/* 2. 查找插入位置：在此当然也可以采用二分查找算法，有兴趣的可以自己去优化 */
	while (NULL != node) {
		for (idx = 0; idx < node->num; idx++) {
			if (key == node->key[idx]) {
				fprintf(stderr, "[%s][%d] The node is exist!\n", __FILE__,
				__LINE__);
				return 0;
			} else if (key < node->key[idx]) {
				break;
			}
		}

		if (NULL != node->child[idx]) {
			node = node->child[idx];
		} else {
			break;
		}
	}

	/* 3. 执行插入操作 */
	return _btree_insert(btree, node, key, idx);
}

