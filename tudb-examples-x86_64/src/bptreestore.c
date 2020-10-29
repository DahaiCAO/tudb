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
		fprintf(stderr, "[%s][%d] Parameter 'max' must greater than 2.\n",
		__FILE__, __LINE__);
		return -1;
	}

	bptree = (ta_bptree_t*) calloc(1, sizeof(ta_bptree_t));
	if (NULL == bptree) {
		fprintf(stderr, "[%s][%d] err msg:[%d] %s!\n", __FILE__, __LINE__,
		errno, strerror(errno));
		return -1;
	}
// m = 5
	bptree->max = m - 1; // 4
	bptree->min = m / 2; // 2
	if (0 != m % 2) { //3
		bptree->min++;
	}
	bptree->min--; //2
	bptree->sidx = m / 2; //2
	bptree->sidxLeaf = m / 4; //1
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
	node->child = (ta_bptree_node_t**) calloc(bptree->max + 2,
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

		/* Copy data 2 */
		if (node->leaf == 1) {
			memcpy(nodeRight->key, node->key + sidx,
					(total - sidx) * sizeof(int));
//			memcpy(nodeRight->child, node->child + sidx + 1,
//					(total - sidx) * sizeof(ta_bptree_node_t*));
//			for (int k = 0; k < bptree->max+1; k++) {
//				printf("child: %d\n", node->child[k]);
//			}
			nodeRight->num = (total - sidx);
			nodeRight->leaf = 1;
			node->num = sidx;

			node->next = nodeRight;
			nodeRight->prev = node;
		} else {
			memcpy(nodeRight->key, node->key + sidx + 1,
					(total - sidx - 1) * sizeof(int));
			memcpy(nodeRight->child, node->child + sidx + 1,
					(total - sidx) * sizeof(ta_bptree_node_t*));
			nodeRight->leaf = 0;
			nodeRight->num = (total - sidx - 1);
			node->num = sidx;

		}
		nodeRight->parent = node->parent;

		printf("total == %d\n", total);
		printf("sidx == %d\n", sidx);
		printf("total - sidx - 1 == %d\n", (total - sidx - 1));
		printf("nodeRight->num == %d\n", nodeRight->num);
		for (int k = 0; k < nodeRight->num; k++) {
			printf("right child keys: %d\n", nodeRight->key[k]);
		}
		printf("--\n");
		printf("node->num == %d\n", node->num);
		printf("total == %d\n", total);
		for (int k = 0; k < node->num; k++) {
			printf("left child keys: %d\n", node->key[k]);
		}
		printf("--\n");
		/* Insert into parent */
		parent = node->parent;
		if (NULL == parent) {
			/* Split root node */
			parent = bptree_create_node(bptree); // create new root
			if (NULL == parent) {
				fprintf(stderr, "[%s][%d] Create root failed!", __FILE__,
				__LINE__);
				return -1;
			}
			parent->leaf = 0;
			bptree->root = parent;
			node->parent = parent; // left child
			nodeRight->parent = parent; // right child
			parent->key[0] = node->key[sidx];
			parent->child[0] = node;			// child
			parent->child[1] = nodeRight; //
			parent->num++;
			bptree->minLeaf = node;
			//bptree->maxLeaf = nodeRight;
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
		if (node->leaf == 0) {
			memset(node->child + sidx + 1, 0,
					(total - sidx) * sizeof(ta_bptree_node_t*));
		}
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

int _bptree_insert(ta_bptree_t *bptree, ta_bptree_node_t *node, int key,
		int idx) {

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

	/* 2. 查找B+tree插入位置：在此当然也可以采用二分查找算法，
	 * 有兴趣的可以自己去优化，也可以用折半查找来做。*/
	while (NULL != node) {
		for (idx = 0; idx < node->num; idx++) {
			if (key < node->key[idx]) {
				break;
			}
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

int _bptree_merge(ta_bptree_t *bptree, ta_bptree_node_t *left,
		ta_bptree_node_t *right, int mid) {
	int m = 0;
	ta_bptree_node_t *parent = left->parent;

	// 把left和node的共同父节点的key拿到left节点中来。
	for (m = 0; m < right->num; m++) {
		if (parent->key[mid] == right->key[m]) {
			break;
		}
	}
	if (m != 0) {
		left->key[left->num++] = parent->key[mid];
	}
	for (int k = 0; k < left->num; k++) {
		printf("left child keys before: %d\n", left->key[k]);
	}
	// 把右节点node的keys拷贝到子节点中来
	memcpy(left->key + left->num, right->key, right->num * sizeof(int));
	for (int k = 0; k < left->num; k++) {
		printf("left child keys after: %d\n", left->key[k]);
	}

	if (left->leaf == 0) {
		// 把右节点node的children拷贝到子节点中来
		memcpy(left->child + left->num, right->child,
				(right->num + 1) * sizeof(ta_bptree_node_t*));
		// 修改右节点node的孩子父亲为左孩子的父亲
		for (m = 0; m <= right->num; m++) {
			if (NULL != right->child[m]) {
				right->child[m]->parent = left;
			}
		}
	}
	if (left->leaf == 1) {
		// 修改链表指针
		left->next = right->next;
		if (right->next)
			right->next->prev = left;
	}
	// 修改左孩子的num
	left->num += right->num;

	// 在父亲的keys中清除掉parent->key[mid]
	for (m = mid; m < parent->num - 1; m++) {
		parent->key[m] = parent->key[m + 1];
		parent->child[m + 1] = parent->child[m + 2];
	}
	for (int k = 0; k < parent->num; k++) {
		printf("parent keys after: %d\n", parent->key[k]);
	}
	// 删除掉最后一个key，因为这个key已经到了左孩子left去了。
	parent->key[m] = 0;
	parent->child[m + 1] = NULL;
	parent->num--;
	free(right);

	/* Check if need to merge further */
	if (parent->num < bptree->min) {
		return bptree_merge(bptree, parent);
	}

	return 0;
}

int bptree_merge(ta_bptree_t *bptree, ta_bptree_node_t *node) {
	int idx = 0, m = 0, mid = 0;
	ta_bptree_node_t *parent = node->parent, *right = NULL, *left = NULL;

	/* 1. node是根结点, 不必进行合并处理 */
	if (NULL == parent) {
		if (0 == node->num) {
			if (NULL != node->child[0]) {
				bptree->root = node->child[0];
				node->child[0]->parent = NULL;
				node->child[0]->leaf = 1;
			} else {
				bptree->root = NULL;
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
	else if (idx == parent->num) { // node是父亲的最后一个child
		mid = idx - 1;
		left = parent->child[mid]; // 获取为合并或者借用做准备

		/* 1) 合并结点 */
		if ((node->num + left->num + 1) <= bptree->max) {
			return _bptree_merge(bptree, left, node, mid);
		}

		/* 2) 借用结点:brother->key[num-1] */
		for (m = node->num; m > 0; m--) {
			node->key[m] = node->key[m - 1];
			if (node->leaf == 0)
				node->child[m + 1] = node->child[m];
		}
		for (int k = 0; k < node->num; k++) {
			printf("++node keys after: %d\n", node->key[k]);
		}
		if (node->leaf == 0)
			node->child[1] = node->child[0];

		//node->key[0] = parent->key[mid];
		node->key[0] = left->key[left->num - 1];
		node->num++;
		for (int k = 0; k < node->num; k++) {
			printf("++--node keys after: %d\n", node->key[k]);
		}

		if (node->leaf == 0) {
			node->child[0] = left->child[left->num];
			if (NULL != left->child[left->num]) {
				left->child[left->num]->parent = node;
			}
		}

		parent->key[mid] = left->key[left->num - 1];
		printf("++parent->key[mid] after: %d\n", parent->key[mid]);
		left->key[left->num - 1] = 0;
		if (left->leaf == 0)
			left->child[left->num] = NULL;
		left->num--;
		return 0;
	}

	/* 4. node: 非最后一个孩子结点(node < right)
	 * node as left child */
	// node不是父亲的最后一个child，它右边还有节点
	mid = idx;
	right = parent->child[mid + 1];

	/* 1) 合并结点 */
	if ((node->num + right->num + 1) <= bptree->max) {
		return _bptree_merge(bptree, node, right, mid);
	}

	for (int k = 0; k < node->num; k++) {
		printf("left child keys before: %d\n", node->key[k]);
	}

	/* 2) 借用结点: right->key[0] */
	node->key[node->num++] = parent->key[mid];
	if (node->leaf == 0) {
		node->child[node->num] = right->child[0];
		if (NULL != right->child[0]) {
			right->child[0]->parent = node;
		}
	}
	for (int k = 0; k < node->num; k++) {
		printf("left child keys after: %d\n", node->key[k]);
	}
	for (int k = 0; k < right->num; k++) {
		printf("right child keys before: %d\n", right->key[k]);
	}
	//parent->key[mid] = right->key[0];
	for (m = 0; m < right->num; m++) {
		right->key[m] = right->key[m + 1];
		if (right->leaf == 0) {
			right->child[m] = right->child[m + 1];
		}
	}
	parent->key[mid] = right->key[0];
	for (int k = 0; k < parent->num; k++) {
		printf("parent keys after: %d\n", parent->key[k]);
	}
	for (int k = 0; k < right->num; k++) {
		printf("right child keys before: %d\n", right->key[k]);
	}

	if (right->leaf == 0) {
		right->child[m] = NULL;
	}
	right->num--;
	return 0;
}

int _bptree_delete_leaf(ta_bptree_t *bptree, ta_bptree_node_t *node, int idx) {
	printf("to delete child key: %d\n", node->key[idx]);
	printf("%d\n", node->num);

	for (int k = 0; k < node->num; k++) {
		printf("delete child keys before: %d\n", node->key[k]);
	}

	memcpy(node->key + idx, node->key + idx + 1,
			(node->num - idx - 1) * sizeof(int));
	memset(node->key + node->num - 1, 0, sizeof(int));
	node->num--;

	for (int k = 0; k < node->num; k++) {
		printf("remained child keys: %d\n", node->key[k]);
	}
	printf("--\n");

	if (node->num < bptree->min) {
		return bptree_merge(bptree, node);
	}
	return 0;
}

//int _bptree_delete_nonleaf(ta_bptree_t *bptree, ta_bptree_node_t *node, int idx) {
//	ta_bptree_node_t *orig = node, *child = node->child[idx];
//
//	/* 使用node->child[idx]中的最大值替代被删除的关键字 */
//	while (NULL != child) {
//		node = child;
//		child = node->child[child->num];
//	}
//
//	orig->key[idx] = node->key[node->num - 1];
//
//	/* 最终其处理过程相当于是删除最底层结点的关键字 */
//	node->key[--node->num] = 0;
//	if (node->num < bptree->min) {
//		//return bptree_merge(bptree, node);
//	}
//
//	return 0;
//}

int bptree_delete(ta_bptree_t *bptree, int key) {
	int idx = 0;
	ta_bptree_node_t *node = bptree->root;

	while (NULL != node) {
		for (idx = 0; idx < node->num; idx++) {
			if (key == node->key[idx] && node->leaf == 1) {
				return _bptree_delete_leaf(bptree, node, idx);
			} else if (key < node->key[idx]) {
				break;
			}
		}

		node = node->child[idx];
	}
	return 0;
}

