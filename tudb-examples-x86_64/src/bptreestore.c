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
#include "bptreestore.h"
/*
 * bptreestore.c
 *
 * Created on: 2020年10月20日
 * Author: Dahai CAO
 */

/*
 * print b+tree
 */
void print_bptree(ta_bptree_t *btree) {
	ta_bptree_node_t *node = btree->root;
	int level = 1;
	print_bptree_children(node, level);
}

void print_bptree_children(ta_bptree_node_t *node, int level) {
	char *s = (char*) calloc(256, sizeof(char));
	for (int n = 1; n < level - 1; n++) {
		strcat(s, "|           ");
	}
	if (level > 1) {
		strcat(s, "|----------");
	}
	strcat(s, "<");
	char str1[25];
	itoa(node->num, str1, 10);
	strcat(s, str1);
	strcat(s, " |");
	for (int i = 0; i < node->num; i++) {
		char str2[25];
		itoa(node->key[i], str2, 10);
		strcat(s, str2);
		strcat(s, " ");
	}
	strcat(s, ">");
	puts(s);
	level++;
	for (int n = 0; n <= node->num; n++) {
		if (node->child[n] != NULL)
			print_bptree_children(node->child[n], level);
	}
}


// m should be 3 at least in B+tree, if m is 2, the tree is two-b tree
int bptree_create(ta_bptree_t **_bptree, int m) {

	ta_bptree_t *bptree = NULL;

	if (m < 3) {
		fprintf(stderr, "[%s][%d] Parameter 'max' must geater than 2.\n",
				__FILE__, __LINE__);
		return -1;
	}

	bptree = (ta_btree_t*) calloc(1, sizeof(ta_btree_t));
	if (NULL == bptree) {
		fprintf(stderr, "[%s][%d] err msg:[%d] %s!\n", __FILE__, __LINE__, errno,
				strerror(errno));
		return -1;
	}

	bptree->max = m - 1;
	bptree->min = m / 2;
	if (0 != m % 2) {
		bptree->min++;
	}
	bptree->min--;
	bptree->sidx = m / 2;
	bptree->minLeaf = m / 4;
	bptree->root = NULL; /* 空树 */

	*_bptree = bptree;

	return 0;
}

ta_bptree_node_t* bptree_create_node(ta_bptree_t *bptree) {
	ta_bptree_node_t *node = NULL;

	node = (ta_bptree_node_t*) calloc(1, sizeof(ta_bptree_node_t));
	if (NULL == node) {
		fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__, errno,
				strerror(errno));
		return NULL;
	}

	node->num = 0;

	/* More than (max) is for move */
	node->key = (int*) calloc(bptree->max + 1, sizeof(int));
	if (NULL == node->key) {
		free(node), node = NULL;
		fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__, errno,
				strerror(errno));
		return NULL;
	}

	/* More than (max+1) is for move */
	node->child = (ta_bptree_node_t**) calloc(bptree->max + 1,
			sizeof(ta_bptree_node_t*));
	if (NULL == node->child) {
		free(node->key);
		free(node), node = NULL;
		fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__, errno,
				strerror(errno));
		return NULL;
	}

	return node;
}

// node is split as a left child node
int bptree_split(ta_bptree_t *bptree, ta_bptree_node_t *node) {

	int idx = 0, total = 0, sidx = bptree->sidx;
	ta_bptree_node_t *parent = NULL, *nodeRight = NULL;

	while (node->num > bptree->max) {
		/* Split node */
		total = node->num;

		nodeRight = bptree_create_node(bptree); // create a right child
		if (NULL == nodeRight) {
			fprintf(stderr, "[%s][%d] Create node failed!\n", __FILE__,
					__LINE__);
			return -1;
		}

		/* Copy data */
//		memcpy(nodeRight->key, node->key + sidx + 1,
//				(total - sidx - 1) * sizeof(int));
//		memcpy(nodeRight->child, node->child + sidx + 1,
//				(total - sidx) * sizeof(ta_btree_node_t*));
		memcpy(nodeRight->key, node->key + sidx,
				(total - sidx) * sizeof(int));
		memcpy(nodeRight->child, node->child + sidx,
				(total - sidx) * sizeof(ta_btree_node_t*));

		nodeRight->num = (total - sidx);
		nodeRight->parent = node->parent;
		nodeRight->leaf = 1;

		node->num = sidx;

		printf("total == %d\n", total);
		printf("sidx == %d\n", sidx);
		printf("total - sidx - 1 == %d\n", (total - sidx));
		for (int k = 0; k < nodeRight->num; k++) {
			printf("right child keys: %d\n", nodeRight->key[k]);
		}
		/* Insert into parent */
		parent = node->parent;
		if (NULL == parent) {
			/* Split root node */
			// create new root node
			parent = bptree_create_node(bptree); // this is root node
			if (NULL == parent) {
				fprintf(stderr, "[%s][%d] Create root failed!", __FILE__,
						__LINE__);
				return -1;
			}

			bptree->root = parent;
			parent->child[0] = node;
			node->parent = parent;
			nodeRight->parent = parent;

			parent->key[0] = node->key[sidx];
			parent->child[1] = nodeRight;
			parent->num++;
			parent->leaf = 0;

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
			parent->child[idx + 1] = nodeRight;
			nodeRight->parent = parent;
			parent->num++;

			for (int k = 0; k < parent->num; k++) {
				printf("parent child node keys: %d\n", parent->key[k]);
			}
			printf("%s\n", "end parent");

		}

		memset(node->key + sidx, 0, (total - sidx) * sizeof(int));
		memset(node->child + sidx + 1, 0,
				(total - sidx) * sizeof(ta_btree_node_t*));

		for (int k = 0; k < node->num; k++) {
			printf("left child node keys: %d\n", node->key[k]);
		}
		printf("%s\n", "end left");

		/* Change right child node's child->parent */
		for (idx = 0; idx <= nodeRight->num; idx++) {
			if (NULL != nodeRight->child[idx]) {
				nodeRight->child[idx]->parent = nodeRight;
			}
		}
		node = parent;
	}

	return 0;
}


int _bptree_insert(ta_bptree_t *bptree, ta_bptree_node_t *node, int key, int idx) {

	int i = 0;

	/* 1. 移动关键字:首先在最底层的某个非终端结点上插入一个关键字,
	 * 因此该结点无孩子结点，故不涉及孩子指针的移动操作 */
	for (i = node->num; i > idx; i--) {
		node->key[i] = node->key[i - 1];
	}

	node->key[idx] = key; /* 插入 */
	node->num++;

	for (int k = 0; k < node->num; k++) {
		printf("::::%d\n", node->key[k]);
	}
	printf("%s\n", "end");

	/* 2. 分裂处理 */
	if (node->num > bptree->max) {
		return bptree_split(bptree, node);
	}

	return 0;
}

int bptree_insert(ta_bptree_t *bptree, int key) {
	int idx = 0;
	ta_bptree_node_t *node = bptree->root;

	/* 1. 构建第一个结点（根节点） */
	if (NULL == node) {
		node = bptree_create_node(bptree);
		if (NULL == node) {
			fprintf(stderr, "[%s][%d] Create node failed!\n", __FILE__,
					__LINE__);
			return -1;
		}
		node->leaf = 1;
		node->num = 1;
		node->key[0] = key;
		node->parent = NULL;
		node->prev = NULL;
		node->next = NULL;

		bptree->root = node;
		return 0;
	}

	/* 2. 查找B+-tree插入位置：在此当然也可以采用二分查找算法，
	 * 有兴趣的可以自己去优化，也可以用折半查找来做。*/
	while (NULL != node) {
		for (idx = 0; idx < node->num; idx++) {
			if (key < node->key[idx]) {
				break;
			}
		}
		if (idx == node->num) {
			idx--;
		}
		if (node->child[idx]) {
			node = node->child[idx];
		} else {
			break;
		}
	}

	/* 3. 执行插入操作 */
	return _bptree_insert(bptree, node, key, idx);

}
