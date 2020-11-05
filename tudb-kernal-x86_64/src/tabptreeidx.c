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
#include "tabptreeidx.h"

/*
 * These functions are used to implement a b+ tree index for time axis.
 *
 * Created on: 2020年10月18日
 * Author: Dahai CAO
 */
static void print_ta_indexes(ta_idx_node_t *node, int level) {
	char *s = (char*) calloc(256, sizeof(char));
	//unsigned long long *p = (unsigned long long*) calloc(256, sizeof(unsigned long long));
	for (int n = 1; n < level - 1; n++) {
		strcat(s, "|           ");
	}
	if (level > 1) {
		strcat(s, "|----------");
	}
	strcat(s, "<id:");
	char str[20] = { 0 };
	itoa(node->id, str, 10);
	strcat(s, str);
	strcat(s, "(");
	memset(str, 20, sizeof(char));
	sprintf(str, "0x%lx", node);
	strcat(s, str);
	strcat(s, "),num:");
	memset(str, 20, sizeof(char));
	itoa(node->num, str, 10);
	strcat(s, str);
	strcat(s, ",leaf:");
	memset(str, 20, sizeof(char));
	itoa(node->leaf, str, 10);
	strcat(s, str);
	strcat(s, ",hit:");
	memset(str, 20, sizeof(char));
	itoa(node->hit, str, 10);
	strcat(s, str);
	strcat(s, ",exp:");
	memset(str, 20, sizeof(char));
	itoa(node->expiretime, str, 10);
	strcat(s, str);
	strcat(s, ",dirty:");
	memset(str, 20, sizeof(char));
	itoa(node->dirty, str, 10);
	strcat(s, str);
	strcat(s, "|keys:");
	for (int i = 0; i < node->num; i++) {
		char str2[25] = { 0 };
		itoa(node->keys[i], str2, 10);
		strcat(s, str2);
		strcat(s, " ");
	}
	if (node->leaf == 1) {
		strcat(s, "|tuids:");
		for (int i = 0; i < node->num; i++) {
			char str2[25] = { 0 };
			itoa(node->tuIdxIds[i], str2, 10);
			strcat(s, str2);
			strcat(s, " ");
		}
	} else {
		strcat(s, "|childIds:");
		for (int i = 0; i < node->num; i++) {
			char str2[25] = { 0 };
			itoa(node->chldrnIds[i], str2, 10);
			strcat(s, str2);
			strcat(s, "(");
			memset(str2, 20, sizeof(char));
			sprintf(str2, "0x%lx", node->child[i]);
			strcat(s, str2);
			strcat(s, ") ");
		}
	}
	strcat(s, "|prv id:");
	memset(str, 20, sizeof(char));
	itoa(node->prev, str, 10);
	strcat(s, str);
	strcat(s, "(");
	memset(str, 20, sizeof(char));
	sprintf(str, "0x%lx", node->prv);
	strcat(s, str);
	strcat(s, "),nxt id:");
	itoa(node->next, str, 10);
	strcat(s, str);
	strcat(s, "(");
	memset(str, 20, sizeof(char));
	sprintf(str, "0x%lx", node->nxt);
	strcat(s, str);
	strcat(s, "),parent id:");
	memset(str, 20, sizeof(char));
	sprintf(str, "%lld", node->parent == NULL ? 0 : node->parent->id);
	strcat(s, str);
	strcat(s, "(");
	memset(str, 20, sizeof(char));
	sprintf(str, "0x%lx", node->parent);
	strcat(s, str);
	strcat(s, "), content:");
	memset(str, 20, sizeof(char));
	sprintf(str, "0x%lx", node->content);
	strcat(s, str);
	strcat(s, ">");
	puts(s);

	level++;
	if (node->leaf == 0) {
		for (int n = 0; n <= node->num; n++) {
			if (node->child[n] != NULL)
				print_ta_indexes(node->child[n], level);
		}
	}
}

void print_ta_index(ta_idx_t *ta_idx) {

	char *r = (char*) calloc(256, sizeof(char));
	strcat(r, "(");
	char str0[20] = { 0 };
	strcat(r, "root id:");
	itoa(ta_idx->rtId, str0, 10);
	strcat(r, str0);
	strcat(r, "(");
	memset(str0, 20, sizeof(char));
	sprintf(str0, "0x%lx", ta_idx->root);
	strcat(r, str0);
	strcat(r, "); max leaf id:");
	memset(str0, 20, sizeof(char));
	itoa(ta_idx->maxLf, str0, 10);
	strcat(r, str0);
	strcat(r, "(");
	memset(str0, 20, sizeof(char));
	sprintf(str0, "0x%lx", ta_idx->maxLeaf);
	strcat(r, str0);
	strcat(r, "); min leaf id:");

	memset(str0, 20, sizeof(char));
	itoa(ta_idx->minLf, str0, 10);
	strcat(r, str0);
	strcat(r, "(");
	memset(str0, 20, sizeof(char));
	sprintf(str0, "0x%lx", ta_idx->minLeaf);
	strcat(r, str0);
	strcat(r, "); max:");

	memset(str0, 20, sizeof(char));
	itoa(ta_idx->max, str0, 10);
	strcat(r, str0);
	strcat(r, "; min:");
	memset(str0, 20, sizeof(char));
	itoa(ta_idx->min, str0, 10);
	strcat(r, str0);
	strcat(r, "; sidx:");
	memset(str0, 20, sizeof(char));
	itoa(ta_idx->sidx, str0, 10);
	strcat(r, str0);
	strcat(r, ")");

	puts(r);

	ta_idx_node_t *node = ta_idx->root;
	int level = 1;
	print_ta_indexes(node, level);

}

/* m should be 3 at least in B+tree, if m is 2, the tree is two-b tree
 *
 * This function uses another way to calculate it. the result is same.
 * we define m is key number. that means, children number is m+1.
 * that means this b+tree's order is m+1.
 * for leaf number, the key number is m, children number is m+1;
 * the max keys number is m, the min keys number is m/2
 * We suggest to use even number as order number.
 */
ta_idx_t* taIndexRootCreate(int m) {

	ta_idx_t *bptree = (ta_idx_t*) calloc(1, sizeof(ta_idx_t));
	if (NULL == bptree) {
		fprintf(stderr, "[%s][%d] err msg:[%d] %s!\n", __FILE__, __LINE__,
		errno, strerror(errno));
		return NULL;
	}

	bptree->max = m;
	bptree->min = m / 2;
	if (0 != m % 2) {
		bptree->min++;
	}
	bptree->min--;
	bptree->sidx = (m + 1) / 2;
	bptree->root = NULL; /* 空树 */
	bptree->minLeaf = NULL;
	bptree->maxLeaf = NULL;
	bptree->rtId = 0;
	bptree->maxLf = 0;
	bptree->minLf = 0;

	return bptree;
}

// this function loads a time axis index b+tree node(page), one node one page.
static ta_idx_node_t* readTaIndexPage(ta_idx_t *bptree, long long id,
		size_t offset, size_t size, ta_idx_node_t *parent,
		FILE *ta_bptree_idx_db_fp) {
	long long pos = id * size + offset;
	// read one page data (one b+ tree node) to memory
	unsigned char *page = (unsigned char*) calloc(size, sizeof(unsigned char));
	fseek(ta_bptree_idx_db_fp, pos, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(ta_bptree_idx_db_fp)) != EOF) {
		fseek(ta_bptree_idx_db_fp, pos, SEEK_SET);
		fread(page, sizeof(unsigned char), size, ta_bptree_idx_db_fp); // read one page
	}
	// using this page data to construct one b+ tree node
	ta_idx_node_t *node = (ta_idx_node_t*) calloc(1, sizeof(ta_idx_node_t));
	node->id = id;
	node->expiretime = -1;
	node->hit = 0;
	node->dirty = 0;
	/* 实际关键字个数 */
	node->num = byteArray2Integer(page);
	node->leaf = *(page + LONG);
	/* More than (max+1) is for move */
	/* 子结点：所占空间为（max+2）- 多出来的1个空间用于交换空间使用 */
	node->child = (ta_idx_node_t**) calloc(bptree->max + 2,
			sizeof(ta_idx_node_t*));
	node->parent = parent; /* 父结点 */
	node->nxt = NULL; /* 下一个结点 */
	node->prv = NULL; /* 上一个结点 */
	node->nxtpage = NULL;
	if (node->leaf == 1) { // leaf node parsing
		/* 关键字：所占空间为(max+1) - 多出来的1个空间用于交换空间使用 */
		node->keys = (long long*) calloc(bptree->max + 1, sizeof(long long));
		for (int i = 0; i < node->num; i++) {
			node->keys[i] = bytesLonglong(
					page + ta_bptree_idx_leng_leaf_bytes + i * LONG_LONG);
		}
		node->tuIdxIds = (long long*) calloc(bptree->max + 1,
				sizeof(long long));
		for (int i = 0; i < node->num; i++) {
			node->tuIdxIds[i] = bytesLonglong(
					page + ta_bptree_idx_leng_leaf_bytes
							+ ta_bptree_idx_keys_bytes + i * LONG_LONG);
		}
	} else { // non leaf node parsing
		node->keys = (long long*) calloc(bptree->max + 1, sizeof(long long));
		for (int i = 0; i < node->num; i++) {
			node->keys[i] = bytesLonglong(
					page + ta_bptree_idx_leng_leaf_bytes + i * LONG_LONG);
		}
		node->chldrnIds = (long long*) calloc(bptree->max + 2,
				sizeof(long long));
		for (int i = 0; i < node->num + 1; i++) {
			node->chldrnIds[i] = bytesLonglong(
					page + ta_bptree_idx_leng_leaf_bytes
							+ ta_bptree_idx_keys_bytes + i * LONG_LONG);
		}
	}
	node->next = bytesLonglong(
			page + ta_bptree_idx_leng_leaf_bytes + ta_bptree_idx_keys_bytes
					+ ta_bptree_idx_children_bytes); //
	node->prev = bytesLonglong(
			page + ta_bptree_idx_leng_leaf_bytes + ta_bptree_idx_keys_bytes
					+ ta_bptree_idx_children_bytes + LONG_LONG); //
	node->content = page;

	// construct a linked list for page management.
	ta_idx_node_t *p = ta_idx->root;
	if (p != NULL) {
		while (p->nxtpage != NULL) {
			p = p->nxtpage;
		}
		p->nxtpage = node;
		p = NULL;
	} else {
		ta_idx->root = node;
		ta_idx->rtId = node->id;
		ta_idx->maxLeaf = node;
		ta_idx->maxLf = node->id;
		ta_idx->minLeaf = node;
		ta_idx->minLf = node->id;
	}
	return node;

}

/*
 * initialize time axis index memory pages.
 */
void initTaIndexMemPages(ta_idx_t *ta_idx, FILE *ta_idx_db_fp,
		FILE *ta_idx_id_fp) {
	fseek(ta_idx_db_fp, 0LL, SEEK_SET); // start to read from the first record
	unsigned char *threeids = (unsigned char*) calloc(3 * LONG_LONG,
			sizeof(unsigned char));
	fread(threeids, sizeof(unsigned char), 3 * LONG_LONG, ta_idx_db_fp); // read three ids.

	ta_idx->rtId = bytesLonglong(threeids);
	if (ta_idx->rtId == 0) {
		ta_idx->rtId = getOneId(ta_idx_id_fp, caches->taIds,
				ID_INDEX_QUEUE_LENGTH);
	}
	ta_idx->maxLf = bytesLonglong(threeids + LONG_LONG);
	ta_idx->minLf = bytesLonglong(threeids + 2 * LONG_LONG);
	// read the first page from 3*LONG_LONG pointer.
	readTaIndexPage(ta_idx, ta_idx->rtId, start_pointer,
			ta_bptree_idx_node_bytes, NULL, ta_idx_db_fp);
	free(threeids);
	threeids = NULL;
}

ta_idx_node_t* taIndexCreateNode(ta_idx_t *bptree, unsigned char leaf,
		FILE *ta_id_fp) {
	ta_idx_node_t *node = (ta_idx_node_t*) calloc(1, sizeof(ta_idx_node_t));
	if (NULL == node) {
		fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__, errno,
				strerror(errno));
		return NULL;
	}
	node->id = getOneId(ta_id_fp, caches->taIds, ID_INDEX_QUEUE_LENGTH);
	node->dirty = 0; // if dirty, then 1; otherwise, 0;
	node->hit = 0; // hit counting, 0 by default, hit once, plus 1;
	// buffer in memory for b+tree index. including keys, tu index Id and children ids
	node->content = (unsigned char*) calloc(ta_bptree_idx_node_bytes,
			sizeof(unsigned char));
	node->expiretime = -1; // expiration time stamp, manager will check it to destroy for page swap
	node->leaf = leaf;
	node->next = 0; // next b+tree node Id
	node->prev = 0; // previous b+tree node Id
	node->num = 0; /* number of keys */
	node->parent = NULL; /* parent node */
	node->nxt = NULL; /* next leaf node pointer in memory */
	node->prv = NULL; /* previous leaf node pointer in memory */
	node->nxtpage = NULL; /* next page(node) for deallocation */

	/* More than (max) is for move */
	node->keys = (long long*) calloc(bptree->max + 1, sizeof(long long));
	if (NULL == node->keys) {
		free(node), node = NULL;
		fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__, errno,
				strerror(errno));
		return NULL;
	}

	if (leaf == 1) {
		node->tuIdxIds = (long long*) calloc(bptree->max + 1,
				sizeof(long long));
		if (NULL == node->tuIdxIds) {
			free(node->keys);
			free(node), node = NULL;
			fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__,
			errno, strerror(errno));
			return NULL;
		}

	} else {
		node->chldrnIds = (long long*) calloc(bptree->max + 1,
				sizeof(long long)); //
		if (NULL == node->chldrnIds) {
			free(node->keys);
			free(node), node = NULL;
			fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__,
			errno, strerror(errno));
			return NULL;
		}

		/* More than (max+1) is for move */
		node->child = (ta_idx_node_t**) calloc(bptree->max + 1,
				sizeof(ta_idx_node_t*));
		if (NULL == node->child) {
			free(node->keys);
			free(node->chldrnIds);
			free(node), node = NULL;
			fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__,
			errno, strerror(errno));
			return NULL;
		}
	}

	return node;
}

static int _bptree_split(ta_idx_t *bptree, ta_idx_node_t *node, FILE *ta_id_fp) {

	int idx = 0, total = 0, sidx = bptree->sidx;
	ta_idx_node_t *parent = NULL, *nodeRight = NULL;

	while (node->num > bptree->max) {
		/* Split node */
		total = node->num;

		nodeRight = taIndexCreateNode(bptree, node->leaf, ta_id_fp); // create a right child
		if (NULL == nodeRight) {
			fprintf(stderr,
					"[%s][%d] Create right node failed while splitting!\n",
					__FILE__,
					__LINE__);
			return -1;
		}

		/* Copy data 2 */
		memcpy(nodeRight->keys, node->keys + sidx,
				(total - sidx) * sizeof(long long));
		if (node->leaf == 1) {
			memcpy(nodeRight->tuIdxIds, node->tuIdxIds + sidx,
					(total - sidx) * sizeof(long long));

			ta_idx_node_t *t = node->nxt;
			node->nxt = nodeRight;
			node->next = nodeRight->id;
			nodeRight->nxt = t;
			if (t != NULL)
				nodeRight->next = t->id;

			nodeRight->prv = node;
			nodeRight->prev = node->id;
			if (t != NULL) {
				t->prv = nodeRight;
				t->prev = nodeRight->id;
			}
			t = NULL;
			if (node->parent == NULL) { // if root node splitting
				bptree->minLeaf = node;
				bptree->minLf = node->id;
				bptree->maxLeaf = nodeRight;
				bptree->maxLf = nodeRight->id;
			} else {
				if (node->id == bptree->maxLf) { // if leaf splitting
					bptree->maxLeaf = nodeRight;
					bptree->maxLf = nodeRight->id;
				}
			}
		} else {
			memcpy(nodeRight->child, node->child + sidx,
					(total - sidx) * sizeof(ta_idx_node_t*));
			memcpy(nodeRight->chldrnIds, node->chldrnIds + sidx,
					(total - sidx) * sizeof(long long));
		}
		nodeRight->num = (total - sidx);
		node->num = sidx;
		node->dirty = 1;
		node->hit++;
		nodeRight->parent = node->parent;
		nodeRight->leaf = node->leaf;
		nodeRight->dirty = 1;
		nodeRight->hit++;
		nodeRight->expiretime = -1; // 计算过期的时间戳，未来完善

		printf("total == %d\n", total);
		printf("sidx == %d\n", sidx);
		printf("nodeRight->num == %d\n", nodeRight->num);
		printf("node->num == %d\n", node->num);
		if (node->leaf == 1) {
			for (int k = 0; k < node->num; k++) {
				printf("l keys: %lld\n", node->keys[k]);
			}
			for (int k = 0; k < node->num; k++) {
				printf("l tuIdxIds: %lld\n", node->tuIdxIds[k]);
			}
			for (int k = 0; k < nodeRight->num; k++) {
				printf("r keys: %lld\n", nodeRight->keys[k]);
			}
			for (int k = 0; k < nodeRight->num; k++) {
				printf("r tuIdxIds: %lld\n", nodeRight->tuIdxIds[k]);
			}
		} else {
			for (int k = 0; k < node->num; k++) {
				printf("l keys: %lld\n", node->keys[k]);
			}
			for (int k = 0; k < node->num; k++) {
				printf("l chldrnIds: %lld\n", node->chldrnIds[k]);
			}
			for (int k = 0; k < nodeRight->num; k++) {
				printf("r keys: %lld\n", nodeRight->keys[k]);
			}
			for (int k = 0; k < nodeRight->num; k++) {
				printf("r chldrnIds: %lld\n", nodeRight->chldrnIds[k]);
			}

		}

		/* Insert into parent */
		parent = node->parent;
		if (NULL == parent) {
			/* Split root node */
			parent = taIndexCreateNode(bptree, 0, ta_id_fp); // create new root
			if (NULL == parent) {
				fprintf(stderr, "[%s][%d] Create root failed!", __FILE__,
				__LINE__);
				return -1;
			}
			parent->leaf = 0;
			parent->dirty = 1;
			parent->hit++;
			parent->expiretime = -1; // 根节点不计算过期的时间戳，内存始终保持有根节点
			bptree->root = parent; // parent is new root
			bptree->rtId = parent->id;
			node->parent = parent; // left child node
			nodeRight->parent = parent; // right child node
			parent->keys[0] = node->keys[0];
			parent->keys[1] = nodeRight->keys[0];
			parent->child[0] = node; // left child node
			parent->child[1] = nodeRight; // right child node
			parent->chldrnIds[0] = node->id;
			parent->chldrnIds[1] = nodeRight->id;
			parent->num += 2;
		} else {
			/* Insert into parent node */
			for (idx = parent->num; idx > 0; idx--) {
				if (nodeRight->keys[0] < parent->keys[idx - 1]) {
					parent->keys[idx] = parent->keys[idx - 1];
					parent->child[idx] = parent->child[idx - 1];
					parent->chldrnIds[idx] = parent->chldrnIds[idx - 1];
					continue;
				}
				break;
			}

			parent->keys[idx] = nodeRight->keys[0];
			parent->child[idx] = nodeRight;
			parent->chldrnIds[idx] = nodeRight->id;
			nodeRight->parent = parent;
			parent->num++;

			for (int k = 0; k < parent->num; k++) {
				printf("parent child node keys: %lld\n", parent->keys[k]);
			}
			for (int k = 0; k < parent->num; k++) {
				printf("parent child node chldrnIds: %lld\n",
						parent->chldrnIds[k]);
			}

			printf("%s\n", "end parent");

		}

		memset(node->keys + sidx, 0, (total - sidx) * sizeof(long long));
		if (node->leaf == 0) {
			memset(node->child + sidx, 0,
					(total - sidx) * sizeof(ta_idx_node_t*));
			memset(node->chldrnIds + sidx, 0,
					(total - sidx) * sizeof(long long));
		}
		if (nodeRight->leaf == 0) {
			/* Change right child node's child->parent */
			for (idx = 0; idx <= nodeRight->num; idx++) {
				if (NULL != nodeRight->child[idx]) {
					nodeRight->child[idx]->parent = nodeRight;
				}
			}
			for (int k = 0; k < nodeRight->num; k++) {
				printf("left child node keys: %lld\n", nodeRight->chldrnIds[k]);
			}
			printf("%s\n", "end left");
		}

		node = parent;
	}

	return 0;
}

static int _bptree_insert_leaf(ta_idx_t *bptree, ta_idx_node_t *node,
		long long key, long long tuid, int idx, FILE *ta_id_fp) {

	int i = 0;

	/* 1. 移动关键字:首先在最底层的某个非终端结点上插入一个关键字,
	 * 因此该结点无孩子结点，故不涉及孩子指针的移动操作 */
	for (i = node->num; i > idx; i--) {
		node->keys[i] = node->keys[i - 1];
	}
	for (i = node->num; i > idx; i--) {
		node->tuIdxIds[i] = node->tuIdxIds[i - 1];
	}

	node->keys[idx] = key; /* 插入 */
	node->tuIdxIds[idx] = tuid; /* 插入 */
	node->hit++;
	node->dirty = 1;
	node->num++;

	for (int k = 0; k < node->num; k++) {
		printf(":::: %lld\n", node->keys[k]);
	}
	printf("%s\n", "--");
	for (int k = 0; k < node->num; k++) {
		printf(">>>> %lld\n", node->tuIdxIds[k]);
	}

	/* 2. 分裂处理 */
	if (node->num > bptree->max) {
		return _bptree_split(bptree, node, ta_id_fp);
	}

	return 0;
}

static int _bptree_update_leaf(ta_idx_t *bptree, ta_idx_node_t *node,
		long long key, long long tuid, int idx, FILE *ta_id_fp) {

	node->tuIdxIds[idx] = tuid;

	for (int k = 0; k < node->num; k++) {
		printf(">>>>>>> %lld\n", node->keys[k]);
	}
	printf("%s\n", "--");
	for (int k = 0; k < node->num; k++) {
		printf(">>>>>>> %lld\n", node->tuIdxIds[k]);
	}
	node->hit++;
	node->dirty = 1;

	return 0;
}

ta_idx_node_t* taIndexInsertNode(ta_idx_t *bptree, long long ts, long long tuid,
		FILE *ta_id_fp, FILE *ta_db_fp) {
	int idx = 0, found = 0;
	ta_idx_node_t *node = bptree->root;
	/* 2. 查找B+tree插入位置：在此当然也可以采用二分查找算法，
	 * 有兴趣的可以自己去优化，也可以用折半查找来做。*/
	while (NULL != node) {
		for (idx = 0; idx < node->num; idx++) {
			if (ts < node->keys[idx]) {
				if (idx > 0 && node->leaf == 0)
					idx--;
				break;
			} else if (ts == node->keys[idx]) {
				found = 1;
				node->hit++;
				break;
			}

		}

		if (node->leaf == 0) {
			if (idx == node->num) {
				idx--;
				node->hit++;
			} else if (idx == 0) {
				if (ts < node->keys[idx]) {
					node->keys[idx] = ts;
					node->dirty = 1;
				}
				node->hit++;
			}
			if (node->child[idx] != NULL) {
				node = node->child[idx];
			} else {
				if (node->chldrnIds[idx] != 0) {
					node->child[idx] = readTaIndexPage(bptree,
							node->chldrnIds[idx], start_pointer,
							ta_bptree_idx_node_bytes, node, ta_db_fp);
					node = node->child[idx];
				}
			}
		} else {
			break;
		}
	}

	/* 3. 执行插入操作 */
	if (found == 0) {
		// insert
		_bptree_insert_leaf(bptree, node, ts, tuid, idx, ta_id_fp);
	} else {
		// update
		_bptree_update_leaf(bptree, node, ts, tuid, idx, ta_id_fp);
	}

	return node;
}

void commitIndexNode(ta_idx_t *bptree, FILE *ta_db_fp) {
	ta_idx_node_t *p = bptree->root;
	if (p != NULL) {
		while (p != NULL) {
			if (p->dirty == 1) {
				// update memory page
				unsigned char *bf = p->content;
				memset(bf, 0, ta_bptree_idx_node_bytes * sizeof(unsigned char));
				integerBytesArry(p->num, p->content);
				*(p->content + LONG) = p->leaf;
				if (p->leaf == 1) {
					for (int i = 0; i < p->num; i++) {
						longlongtoByteArray(p->keys[i],
								p->content + ta_bptree_idx_leng_leaf_bytes
										+ i * LONG_LONG);
					}
					for (int i = 0; i < p->num; i++) {
						longlongtoByteArray(p->keys[i],
								p->content + ta_bptree_idx_leng_leaf_bytes
										+ ta_bptree_idx_keys_bytes
										+ i * LONG_LONG);
					}
				} else {
					for (int i = 0; i < p->num; i++) {
						longlongtoByteArray(p->keys[i],
								p->content + ta_bptree_idx_leng_leaf_bytes
										+ i * LONG_LONG);
					}
					for (int i = 0; i < p->num + 1; i++) {
						longlongtoByteArray(p->keys[i],
								p->content + ta_bptree_idx_leng_leaf_bytes
										+ i * LONG_LONG);
					}
				}
				// write to db
				fseek(ta_db_fp,
						p->id * ta_bptree_idx_node_bytes + start_pointer,
						SEEK_SET);
				fwrite(p->content, sizeof(unsigned char),
						ta_bptree_idx_node_bytes, ta_db_fp);
				p->dirty = 0;
			}
			p = p->nxtpage;
		}
		p = NULL;
	}

	// update root node id/max node id/min node id
	fseek(ta_db_fp, 0, SEEK_SET);
	unsigned char *threeids = (unsigned char*) calloc(3 * LONG_LONG,
			sizeof(unsigned char));
	longlongtoByteArray(bptree->rtId, threeids);
	longlongtoByteArray(bptree->maxLf, threeids + LONG_LONG);
	longlongtoByteArray(bptree->minLf, threeids + 2 * LONG_LONG);
	fwrite(threeids, sizeof(unsigned char), 3 * LONG_LONG, ta_db_fp);
	free(threeids);
	threeids = NULL;
}

static void deallocTaIndexNode(ta_idx_node_t *node) {

}

// the merge is to right node merges to left node. (right -> left)
static int _bptree_merge(ta_idx_t *bptree, ta_idx_node_t *left,
		ta_idx_node_t *right, int mid) {
	int m = 0;
	ta_idx_node_t *parent = left->parent;

	for (int k = 0; k < left->num; k++) {
		printf("left child keys before: %lld\n", left->keys[k]);
	}
	// 把右节点node的keys拷贝到子节点中来
	memcpy(left->keys + left->num, right->keys, right->num * sizeof(long long));
	for (int k = 0; k < left->num; k++) {
		printf("left child keys after: %lld\n", left->keys[k]);
	}
	if (left->leaf == 1) {
		memcpy(left->tuIdxIds + left->num, right->tuIdxIds,
				right->num * sizeof(long long));
		for (int k = 0; k < left->num; k++) {
			printf("left child tuIdxIds after: %lld\n", left->tuIdxIds[k]);
		}
	} else if (left->leaf == 0) {
		// 把右节点node的children拷贝到子节点中来
		memcpy(left->child + left->num, right->child,
				right->num * sizeof(ta_idx_node_t*));
		memcpy(left->chldrnIds + left->num, right->chldrnIds,
				right->num * sizeof(long long));
		// 修改右节点node的孩子父亲为左孩子的父亲
		for (m = 0; m <= right->num; m++) {
			if (NULL != right->child[m]) {
				right->child[m]->parent = left;
			}
		}
	}

	// 修改左孩子的num
	left->num += right->num;

	// 在父亲的keys中清除掉parent->key[mid]
	for (m = mid; m < parent->num - 1; m++) {
		parent->keys[m] = parent->keys[m + 1];
		parent->child[m] = parent->child[m + 1];
		parent->chldrnIds[m] = parent->chldrnIds[m + 1];
	}
	for (int k = 0; k < parent->num; k++) {
		printf("parent keys after: %lld\n", parent->keys[k]);
	}
	// 删除掉最后一个key，因为这个key已经到了左孩子left去了。
	parent->keys[m] = 0;
	parent->child[m] = NULL;
	parent->chldrnIds[m] = 0;
	parent->num--;

	if (left->leaf == 1) {			// 修改链表指针
		left->nxt = right->nxt;
		if (right->nxt) {
			left->next = right->nxt->id;
			right->nxt->prv = left;
			right->nxt->prev = left->id;
		}
		if (right->parent == NULL) { // if root node splitting
			bptree->maxLeaf = left;
			bptree->maxLf = left->id;
		} else {
			if (right->id == bptree->maxLf) { // if leaf splitting
				bptree->maxLeaf = left;
				bptree->maxLf = left->id;
			}
		}
	}
	deallocTaIndexNode(right);

	/* Check if need to merge further */
	if (parent->num < bptree->min) {
		return ta_bptree_merge(bptree, parent);
	}

	return 0;
}

int ta_bptree_merge(ta_idx_t *bptree, ta_idx_node_t *node) {
	int idx = 0, m = 0, mid = 0;
	ta_idx_node_t *parent = node->parent, *right = NULL, *left = NULL;

	/* 1. node是根结点, 不必进行合并处理 */
	if (NULL == parent) {
		if (0 == node->num) {
			if (NULL != node->child[0]) {
				bptree->root = node->child[0];			// new root node
				bptree->rtId = node->child[0]->id;
				node->child[0]->parent = NULL;
				node->child[0]->leaf = 1;
				bptree->maxLeaf = node->child[0];
				bptree->maxLf = node->child[0]->id;
				bptree->minLeaf = node->child[0];
				bptree->minLf = node->child[0]->id;
			} else {
				bptree->root = NULL;
			}
			deallocTaIndexNode(node); // deallocate old root node
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
	/* 3. node: 最后一个孩子结点，即最右边节点(left < node)
	 * node as right child */
	else if (idx == parent->num) { // node是父亲的最后一个child
		mid = idx - 1;
		left = parent->child[mid]; // 获取左兄弟节点，为合并或者借用做准备

		/* 1) 合并结点 */
		if ((node->num + left->num + 1) <= bptree->max) {
			return _bptree_merge(bptree, left, node, mid);
		}

		/* 2) 借用结点:brother->key[num-1] */
		for (m = node->num; m > 0; m--) { // 为借用节点腾出个位置
			node->keys[m] = node->keys[m - 1];
			if (node->leaf == 0) {
				node->child[m] = node->child[m - 1];
				node->chldrnIds[m] = node->chldrnIds[m - 1];
			} else
				node->tuIdxIds[m] = node->tuIdxIds[m - 1];
		}
		if (node->leaf == 0) {
			node->keys[1] = node->keys[0];
			node->child[1] = node->child[0];
			node->chldrnIds[1] = node->chldrnIds[0];
		} else {
			node->keys[1] = node->keys[0];
			node->tuIdxIds[1] = node->tuIdxIds[0];
		}
		// 腾位置结束。
		for (int k = 0; k < node->num; k++) {
			printf("++node keys after: %lld\n", node->keys[k]);
			if (node->leaf == 1)
				printf("++node tuIdxIds after: %lld\n", node->tuIdxIds[k]);
			else
				printf("++node chldrnIds after: %lld\n", node->chldrnIds[k]);
		}

		node->keys[0] = left->keys[left->num - 1];
		if (node->leaf == 1) {
			node->tuIdxIds[0] = left->tuIdxIds[left->num - 1];
		} else if (node->leaf == 0) {
			node->child[0] = left->child[left->num - 1];
			if (NULL != left->child[left->num - 1]) {
				left->child[left->num - 1]->parent = node;
			}
		}
		node->num++;

		for (int k = 0; k < node->num; k++) {
			printf("++--node keys after: %lld\n", node->keys[k]);
			if (node->leaf == 1)
				printf("++node tuIdxIds after: %lld\n", node->tuIdxIds[k]);
			else
				printf("++node chldrnIds after: %lld\n", node->chldrnIds[k]);
		}

		parent->keys[mid] = left->keys[left->num - 1]; // 就换了个key
		printf("++parent->key[mid] after: %lld\n", parent->keys[mid]);
		left->keys[left->num - 1] = 0; // clear
		if (left->leaf == 0) {
			left->child[left->num - 1] = NULL;
			left->chldrnIds[left->num - 1] = 0;
		} else
			left->tuIdxIds[left->num - 1] = 0;
		left->num--;
		return 0;
	}

	/* 4. node: 非最后一个孩子结点(node < right)
	 * node as left child */
	// node不是父亲的最后（最右）一个child，它右边还有节点
	mid = idx;
	right = parent->child[mid + 1]; // 获取右兄弟节点

	/* 1) 合并结点 */
	if ((node->num + right->num + 1) <= bptree->max) {
		return _bptree_merge(bptree, node, right, mid);
	}

	for (int k = 0; k < node->num; k++) {
		printf("left child keys before: %lld\n", node->keys[k]);
		if (node->leaf == 1)
			printf("++node tuIdxIds before: %lld\n", node->tuIdxIds[k]);
		else
			printf("++node chldrnIds before: %lld\n", node->chldrnIds[k]);
	}

	/* 2) 借用结点: right->key[0] */
	node->keys[node->num] = parent->keys[mid];
	if (node->leaf == 1) {
		node->tuIdxIds[node->num] = right->tuIdxIds[0];
	} else if (node->leaf == 0) {
		node->child[node->num] = right->child[0];
		node->chldrnIds[node->num] = right->chldrnIds[0];
		if (NULL != right->child[0]) {
			right->child[0]->parent = node;
		}
	}
	node->num++;
	for (int k = 0; k < node->num; k++) {
		printf("left child keys after: %lld\n", node->keys[k]);
	}
	for (int k = 0; k < right->num; k++) {
		printf("right child keys after: %lld\n", right->keys[k]);
	}
	for (m = 0; m < right->num; m++) { // 移动右兄弟节点的
		right->keys[m] = right->keys[m + 1];
		if (right->leaf == 0) {
			right->child[m] = right->child[m + 1];
			right->chldrnIds[m] = right->chldrnIds[m + 1];
		} else {
			right->tuIdxIds[m] = right->tuIdxIds[m + 1];
		}
	}
	parent->keys[mid] = right->keys[0];
	for (int k = 0; k < parent->num; k++) {
		printf("parent keys after: %lld\n", parent->keys[k]);
	}
	for (int k = 0; k < right->num; k++) {
		printf("right child keys after: %lld\n", right->keys[k]);
	}
	right->keys[m] = 0;
	if (right->leaf == 1) {
		right->tuIdxIds[m] = 0;
	} else {
		right->child[m] = NULL;
		right->chldrnIds[m] = 0;
	}
	right->num--;
	return 0;
}

static int _bptree_delete_leaf(ta_idx_t *bptree, ta_idx_node_t *node, int idx,
		long long tuid) {
	printf("to delete child key: %lld\n", node->keys[idx]);
	printf("%d\n", node->num);

	for (int k = 0; k < node->num; k++) {
		printf("delete child keys before: %lld\n", node->keys[k]);
	}

	if (node->tuIdxIds[idx] != 0) { // node is a leaf
		// delete tuid from tu index id from index tuid DB
	}

	if (node->tuIdxIds[idx] == 0) { // delete a key and tu index id from leaf nodes
		memcpy(node->keys + idx, node->keys + idx + 1,
				(node->num - idx - 1) * sizeof(long long)); // delete the key
		memset(node->keys + node->num - 1, 0, sizeof(long long)); // clear
		memcpy(node->tuIdxIds + idx, node->tuIdxIds + idx + 1,
				(node->num - idx - 1) * sizeof(long long));
		memset(node->tuIdxIds + node->num - 1, 0, sizeof(long long));
		node->num--;
	}

	for (int k = 0; k < node->num; k++) {
		printf("remained child keys: %lld\n", node->keys[k]);
	}
	printf("--\n");

	if (node->num < bptree->min) {
		return ta_bptree_merge(bptree, node);
	}
	return 0;
}

// 先测插入，后测删除
int taIndexDeleteNode(ta_idx_t *bptree, long long ts, long long tuid,
		FILE *ta_db_fp) {

	int idx = 0, found = 0;
	ta_idx_node_t *node = bptree->root;
	/* 2. 查找B+tree插入位置：在此当然也可以采用二分查找算法，
	 * 有兴趣的可以自己去优化，也可以用折半查找来做。*/
	while (NULL != node) {
		for (idx = 0; idx < node->num; idx++) {
			if (ts < node->keys[idx]) {
				if (idx > 0 && node->leaf == 0)
					idx--;
				break;
			} else if (ts == node->keys[idx]) {
				found = 1;
				node->hit++;
				break;
			}

		}

		if (node->leaf == 0) {
			if (idx == node->num) {
				idx--;
				node->hit++;
			} else if (idx == 0) {
				if (ts < node->keys[idx]) {
					node->keys[idx] = ts;
					node->dirty = 1;
				}
				node->hit++;
			}
			if (node->child[idx] != NULL) {
				node = node->child[idx];
			} else {
				if (node->chldrnIds[idx] != 0) {
					node->child[idx] = readTaIndexPage(bptree,
							node->chldrnIds[idx], start_pointer,
							ta_bptree_idx_node_bytes, node, ta_db_fp);
					node = node->child[idx];
				}
			}
		} else {
			break;
		}
	}

	/* 3. 执行删除操作 */
	if (found == 1) {
		// update
		return _bptree_delete_leaf(bptree, node, idx, tuid);
	}

	return node;
}
