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
#include "btreestore.h"
/*
 * tabtreestore.c
 *
 * Created on: 2020年8月2日
 * Author: Dahai CAO
 */

void print_btree(ta_btree_t *btree) {
	ta_btree_node_t *node = btree->root;
	int level = 1;
	print_btree_children(node, level);
}

void print_btree_children(ta_btree_node_t *node, int level) {
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
			print_btree_children(node->child[n], level);
	}
}

// m should be 3 at least in B-tree, if m is 2, the tree is two-b tree
int btree_create(ta_btree_t **_btree, int m) {

	ta_btree_t *btree = NULL;

	if (m < 3) {
		fprintf(stderr, "[%s][%d] Parameter 'max' must geater than 2.\n",
				__FILE__, __LINE__);
		return -1;
	}

	btree = (ta_btree_t*) calloc(1, sizeof(ta_btree_t));
	if (NULL == btree) {
		fprintf(stderr, "[%s][%d] errmsg:[%d] %s!\n", __FILE__, __LINE__, errno,
				strerror(errno));
		return -1;
	}

	btree->max = m - 1;
	// Math.ceiling(m/2)-1;
	btree->min = m / 2;
	if (0 != m % 2) {
		btree->min++;
	}
	btree->min--;
	btree->sidx = m / 2;
	btree->root = NULL; /* 空树 */

	*_btree = btree;

	return 0;
}

ta_btree_node_t* btree_create_node(ta_btree_t *btree) {
	ta_btree_node_t *node = NULL;

	node = (ta_btree_node_t*) calloc(1, sizeof(ta_btree_node_t));
	if (NULL == node) {
		fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__, errno,
				strerror(errno));
		return NULL;
	}

	node->num = 0;

	/* More than (max) is for move */
	node->key = (int*) calloc(btree->max + 1, sizeof(int));
	if (NULL == node->key) {
		free(node), node = NULL;
		fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__, errno,
				strerror(errno));
		return NULL;
	}

	/* More than (max+1) is for move */
	node->child = (ta_btree_node_t**) calloc(btree->max + 2,
			sizeof(ta_btree_node_t*));
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
int btree_split(ta_btree_t *btree, ta_btree_node_t *node) {

	int idx = 0, total = 0, sidx = btree->sidx;
	ta_btree_node_t *parent = NULL, *nodeRight = NULL;

	while (node->num > btree->max) {
		/* Split node */
		total = node->num;

		nodeRight = btree_create_node(btree); // create a right child
		if (NULL == nodeRight) {
			fprintf(stderr, "[%s][%d] Create node failed!\n", __FILE__,
					__LINE__);
			return -1;
		}

		/* Copy data */
		memcpy(nodeRight->key, node->key + sidx + 1,
				(total - sidx - 1) * sizeof(int));
		memcpy(nodeRight->child, node->child + sidx + 1,
				(total - sidx) * sizeof(ta_btree_node_t*));

		nodeRight->num = (total - sidx - 1);
		nodeRight->parent = node->parent;

		node->num = sidx;

		printf("total == %d\n", total);
		printf("sidx == %d\n", sidx);
		printf("total - sidx - 1 == %d\n", (total - sidx - 1));
		for (int k = 0; k < nodeRight->num; k++) {
			printf("right child keys: %d\n", nodeRight->key[k]);
		}
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
			nodeRight->parent = parent;

			parent->key[0] = node->key[sidx];
			parent->child[1] = nodeRight;
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

int _btree_insert(ta_btree_t *btree, ta_btree_node_t *node, int key, int idx) {

	int i = 0;

	/* 1. 移动关键字:首先在最底层的某个非终端结点上插入一个关键字,因此该结点无孩子结点，故不涉及孩子指针的移动操作 */
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

//
int _btree_merge(ta_btree_t *btree, ta_btree_node_t *left,
		ta_btree_node_t *right, int mid) {
	int m = 0;
	ta_btree_node_t *parent = left->parent;

	left->key[left->num++] = parent->key[mid];

	memcpy(left->key + left->num, right->key, right->num * sizeof(int));
	memcpy(left->child + left->num, right->child,
			(right->num + 1) * sizeof(ta_btree_node_t*));
	for (m = 0; m <= right->num; m++) {
		if (NULL != right->child[m]) {
			right->child[m]->parent = left;
		}
	}
	left->num += right->num;

	for (m = mid; m < parent->num - 1; m++) {
		parent->key[m] = parent->key[m + 1];
		parent->child[m + 1] = parent->child[m + 2];
	}

	parent->key[m] = 0;
	parent->child[m + 1] = NULL;
	parent->num--;
	free(right);

	/* Check */
	if (parent->num < btree->min) {
		return btree_merge(btree, parent);
	}

	return 0;
}

int btree_merge(ta_btree_t *btree, ta_btree_node_t *node) {
	int idx = 0, m = 0, mid = 0;
	ta_btree_node_t *parent = node->parent, *right = NULL, *left = NULL;

	/* 1. node是根结点, 不必进行合并处理 */
	if (NULL == parent) {
		if (0 == node->num) {
			if (NULL != node->child[0]) {
				btree->root = node->child[0];
				node->child[0]->parent = NULL;
			} else {
				btree->root = NULL;
			}
			free(node);
		}
		return 0;
	}

	/* 2. 查找node是其父结点的第几个孩子结点 */
	for (idx = 0; idx <= parent->num; idx++) {
		if (parent->child[idx] == node) {
			break;
		}
	}

	if (idx > parent->num) {
		fprintf(stderr,
				"[%s][%d] Didn't find node in parent's children array!\n",
				__FILE__, __LINE__);
		return -1;
	}
	/* 3. node: 最后一个孩子结点(left < node)
	 * node as right child */
	else if (idx == parent->num) {
		mid = idx - 1;
		left = parent->child[mid];

		/* 1) 合并结点 */
		if ((node->num + left->num + 1) <= btree->max) {
			return _btree_merge(btree, left, node, mid);
		}

		/* 2) 借用结点:brother->key[num-1] */
		for (m = node->num; m > 0; m--) {
			node->key[m] = node->key[m - 1];
			node->child[m + 1] = node->child[m];
		}
		node->child[1] = node->child[0];

		node->key[0] = parent->key[mid];
		node->num++;
		node->child[0] = left->child[left->num];
		if (NULL != left->child[left->num]) {
			left->child[left->num]->parent = node;
		}

		parent->key[mid] = left->key[left->num - 1];
		left->key[left->num - 1] = 0;
		left->child[left->num] = NULL;
		left->num--;
		return 0;
	}

	/* 4. node: 非最后一个孩子结点(node < right)
	 * node as left child */
	mid = idx;
	right = parent->child[mid + 1];

	/* 1) 合并结点 */
	if ((node->num + right->num + 1) <= btree->max) {
		return _btree_merge(btree, node, right, mid);
	}

	/* 2) 借用结点: right->key[0] */
	node->key[node->num++] = parent->key[mid];
	node->child[node->num] = right->child[0];
	if (NULL != right->child[0]) {
		right->child[0]->parent = node;
	}

	parent->key[mid] = right->key[0];
	for (m = 0; m < right->num; m++) {
		right->key[m] = right->key[m + 1];
		right->child[m] = right->child[m + 1];
	}
	right->child[m] = NULL;
	right->num--;
	return 0;
}

int _btree_delete(ta_btree_t *btree, ta_btree_node_t *node, int idx) {
	ta_btree_node_t *orig = node, *child = node->child[idx];

	/* 使用node->child[idx]中的最大值替代被删除的关键字 */
	while (NULL != child) {
		node = child;
		child = node->child[child->num];
	}

	orig->key[idx] = node->key[node->num - 1];

	/* 最终其处理过程相当于是删除最底层结点的关键字 */
	node->key[--node->num] = 0;
	if (node->num < btree->min) {
		return btree_merge(btree, node);
	}

	return 0;
}

int btree_delete(ta_btree_t *btree, int key) {
	int idx = 0;
	ta_btree_node_t *node = btree->root;

	while (NULL != node) {
		for (idx = 0; idx < node->num; idx++) {
			if (key == node->key[idx]) {
				return _btree_delete(btree, node, idx);
			} else if (key < node->key[idx]) {
				break;
			}
		}

		node = node->child[idx];
	}

	return 0;
}
