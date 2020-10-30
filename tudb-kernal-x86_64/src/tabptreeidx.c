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

void print_ta_index(ta_idx_t *btree) {
	ta_idx_node_t *node = btree->root;
	int level = 1;
	print_bptree_children(node, level);
}

void print_ta_indexes(ta_idx_node_t *node, int level) {
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
		itoa(node->keys[i], str2, 10);
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

// this function loads a time axis index b+tree node(page), one node one page.
static ta_idx_node_t* readTaIndexPage(ta_idx_t *bptree, long long id,
		size_t size, ta_idx_node_t *parent, FILE *ta_bptree_idx_db_fp) {
	// read one page data (one b+ tree node) to memory
	unsigned char *page = (unsigned char*) calloc(size, sizeof(unsigned char));
	fseek(ta_bptree_idx_db_fp, id, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(ta_bptree_idx_db_fp)) != EOF) {
		fseek(ta_bptree_idx_db_fp, id, SEEK_SET);
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
	}
	return node;

}

/*
 * initialize time axis index memory pages.
 */
void initTaIndexMemPages(ta_idx_t *ta_idx, FILE *ta_db_fp, FILE *ta_idx_id_fp) {
	fseek(ta_db_fp, 0LL, SEEK_SET); // start to read from the first record
	unsigned char *threeids = (unsigned char*) calloc(3 * LONG_LONG,
			sizeof(unsigned char));
	fread(threeids, sizeof(unsigned char), 3 * LONG_LONG, ta_db_fp); // read three ids.

	ta_idx->rtId = bytesLonglong(threeids);
	if (ta_idx->rtId == NULL_POINTER) {
		ta_idx->rtId = getOneId(ta_idx_id_fp, caches->taIds,
				ID_INDEX_QUEUE_LENGTH);
	}
	ta_idx->maxLf = bytesLonglong(threeids + LONG_LONG);
	ta_idx->minLf = bytesLonglong(threeids + 2 * LONG_LONG);
	// read the first page from 3*LONG_LONG pointer.
	readTaIndexPage(ta_idx,
			ta_idx->rtId * ta_bptree_idx_node_bytes + start_pointer,
			ta_bptree_idx_node_bytes, NULL, ta_db_fp);
	free(threeids);
	threeids = NULL;
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
	bptree->sidx = m / 2;
	bptree->root = NULL; /* 空树 */
	bptree->minLeaf = NULL;
	bptree->maxLeaf = NULL;
	bptree->rtId = 0;
	bptree->maxLf = 0;
	bptree->minLf = 0;

	return bptree;
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
		node->chldrnIds = (long long*) calloc(bptree->max + 2,
				sizeof(long long)); //
		if (NULL == node->chldrnIds) {
			free(node->keys);
			free(node), node = NULL;
			fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__,
			errno, strerror(errno));
			return NULL;
		}

		/* More than (max+1) is for move */
		node->child = (ta_idx_node_t**) calloc(bptree->max + 2,
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
		if (node->leaf == 1) {
			memcpy(nodeRight->keys, node->keys + sidx,
					(total - sidx) * sizeof(int));
			memcpy(nodeRight->tuIdxIds, node->tuIdxIds + sidx,
					(total - sidx) * sizeof(ta_idx_node_t*));
			for (int k = 0; k < bptree->max; k++) {
				printf("child: %lld\n", node->tuIdxIds[k]);
			}
			nodeRight->num = (total - sidx);
			node->num = sidx;

			ta_idx_node_t *t = node->nxt;
			node->nxt = nodeRight;
			node->next = nodeRight->id;
			nodeRight->nxt = t;
			nodeRight->next = t->id;

			nodeRight->prv = node;
			nodeRight->prev = node->id;
			t->prv = nodeRight;
			t->prev = nodeRight->id;

			if (node->parent == NULL) { // this is root node splitting
				bptree->minLeaf = node;
				bptree->minLf = node->id;
				bptree->maxLeaf = nodeRight;
				bptree->maxLf = nodeRight->id;
			} else {
				if (node->nxt == NULL) { // if this leaf
					bptree->maxLeaf = nodeRight;
					bptree->maxLf = nodeRight->id;
				}
			}
		} else {
			memcpy(nodeRight->keys, node->keys + sidx + 1,
					(total - sidx - 1) * sizeof(int));
			memcpy(nodeRight->child, node->child + sidx + 1,
					(total - sidx) * sizeof(ta_idx_node_t*));
			memcpy(nodeRight->chldrnIds, node->chldrnIds + sidx + 1,
					(total - sidx) * sizeof(long long*));
			nodeRight->num = (total - sidx - 1);
			node->num = sidx;
		}
		node->dirty = 1;
		nodeRight->parent = node->parent;
		nodeRight->leaf = node->leaf;
		nodeRight->dirty = 1;
		nodeRight->hit = 1;
		nodeRight->expiretime = -1; // 计算过期的时间戳，未来完善

		printf("total == %d\n", total);
		printf("sidx == %d\n", sidx);
		printf("total - sidx - 1 == %d\n", (total - sidx - 1));
		printf("nodeRight->num == %d\n", nodeRight->num);
		for (int k = 0; k < nodeRight->num; k++) {
			printf("right child keys: %lld\n", nodeRight->keys[k]);
		}
		printf("--\n");
		printf("node->num == %d\n", node->num);
		printf("total == %d\n", total);
		for (int k = 0; k < node->num; k++) {
			printf("left child keys: %lld\n", node->keys[k]);
		}
		printf("--\n");
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
			parent->hit = 1;
			parent->expiretime = -1; // 根节点不计算过期的时间戳，内存始终保持有根节点
			bptree->root = parent; // parent is new root
			node->parent = parent; // left child node
			nodeRight->parent = parent; // right child node
			parent->keys[0] = node->keys[sidx];
			parent->child[0] = node; // left child node
			parent->child[1] = nodeRight; // right child node
			parent->chldrnIds[0] = node->id;
			parent->chldrnIds[1] = nodeRight->id;
			parent->num++;
			//bptree->minLeaf = node;
			//bptree->maxLeaf = nodeRight;
		} else {
			/* Insert into parent node */
			for (idx = parent->num; idx > 0; idx--) {
				if (node->keys[sidx] < parent->keys[idx - 1]) {
					parent->keys[idx] = parent->keys[idx - 1];
					parent->child[idx + 1] = parent->child[idx];
					parent->chldrnIds[idx + 1] = parent->chldrnIds[idx];
					continue;
				}
				break;
			}

			parent->keys[idx] = node->keys[sidx];
			parent->child[idx + 1] = nodeRight;
			parent->chldrnIds[idx + 1] = nodeRight->id;
			nodeRight->parent = parent;
			parent->num++;

			for (int k = 0; k < parent->num; k++) {
				printf("parent child node keys: %lld\n", parent->keys[k]);
			}
			for (int k = 0; k < parent->num + 1; k++) {
				printf("parent child node chldrnIds: %lld\n",
						parent->chldrnIds[k]);
			}

			printf("%s\n", "end parent");

		}

		memset(node->keys + sidx, 0, (total - sidx) * sizeof(int));
		if (node->leaf == 0) {
			memset(node->child + sidx + 1, 0,
					(total - sidx) * sizeof(ta_idx_node_t*));
			memset(node->chldrnIds + sidx + 1, 0,
					(total - sidx) * sizeof(ta_idx_node_t*));
		}
		for (int k = 0; k < node->num; k++) {
			printf("left child node keys: %d\n", node->keys[k]);
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

static int _bptree_insert(ta_idx_t *bptree, ta_idx_node_t *node, int key,
		int idx, FILE *ta_id_fp) {

	int i = 0;

	/* 1. 移动关键字:首先在最底层的某个非终端结点上插入一个关键字,
	 * 因此该结点无孩子结点，故不涉及孩子指针的移动操作 */
	for (i = node->num; i > idx; i--) {
		node->keys[i] = node->keys[i - 1];
	}

	node->keys[idx] = key; /* 插入 */
	node->num++;

	for (int k = 0; k < node->num; k++) {
		printf("::::%lld\n", node->keys[k]);
	}
	printf("%s\n", "end");

	/* 2. 分裂处理 */
	if (node->num > bptree->max) {
		return _bptree_split(bptree, node, ta_id_fp);
	}

	return 0;
}

ta_idx_node_t* taIndexInsertNode(ta_idx_t *bptree, long long ts, long long tuid,
		FILE *ta_id_fp, FILE *ta_db_fp) {
	int idx = 0;
	ta_idx_node_t *node = bptree->root;

	/* 2. 查找B+tree插入位置：在此当然也可以采用二分查找算法，
	 * 有兴趣的可以自己去优化，也可以用折半查找来做。*/
	while (NULL != node) {
		for (idx = 0; idx < node->num; idx++) {
			if (ts < node->keys[idx]) {
				break;
			}
		}
		if (node->leaf == 0) {
			if (node->child[idx] != NULL) {
				node = node->child[idx];
			} else {
				if (node->chldrnIds[idx] != 0) {
					node->child[idx] = readTaIndexPage(bptree,
							node->chldrnIds[idx] * ta_bptree_idx_node_bytes
									+ start_pointer, ta_bptree_idx_node_bytes,
							node, ta_db_fp);
					node = node->child[idx];
				}
			}
		} else {
			break;
		}
	}

	/* 3. 执行插入操作 */
	_bptree_insert(bptree, node, ts, idx, ta_id_fp);

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

static int _bptree_merge(ta_idx_t *bptree, ta_idx_node_t *left,
		ta_idx_node_t *right, int mid) {
	int m = 0;
	ta_idx_node_t *parent = left->parent;

	// 把left和node的共同父节点的key拿到left节点中来。
	for (m = 0; m < right->num; m++) {
		if (parent->keys[mid] == right->keys[m]) {
			break;
		}
	}
	if (m != 0) {
		left->keys[left->num] = parent->keys[mid];
		if (left->leaf == 1) {
			left->tuIdxIds[left->num] = 0;			// 新添加到叶子的节点没有tuid
		}
		left->num++;
	}
	for (int k = 0; k < left->num; k++) {
		printf("left child keys before: %d\n", left->keys[k]);
	}
	// 把右节点node的keys拷贝到子节点中来
	memcpy(left->keys + left->num, right->keys, right->num * sizeof(long long));
	for (int k = 0; k < left->num; k++) {
		printf("left child keys after: %d\n", left->keys[k]);
	}
	if (left->leaf == 1) {
		memcpy(left->tuIdxIds + left->num, right->tuIdxIds,
				right->num * sizeof(long long));
		for (int k = 0; k < left->num; k++) {
			printf("left child tuIdxIds after: %d\n", left->tuIdxIds[k]);
		}
	}

	if (left->leaf == 0) {
		// 把右节点node的children拷贝到子节点中来
		memcpy(left->child + left->num, right->child,
				(right->num + 1) * sizeof(ta_idx_node_t*));
		memcpy(left->chldrnIds + left->num, right->chldrnIds,
				(right->num + 1) * sizeof(long long));
		// 修改右节点node的孩子父亲为左孩子的父亲
		for (m = 0; m <= right->num; m++) {
			if (NULL != right->child[m]) {
				right->child[m]->parent = left;
			}
		}
	}
	if (left->leaf == 1) {			// 修改链表指针
		left->nxt = right->nxt;
		if (right->nxt) {
			left->next = right->nxt->id;
			right->nxt->prv = left;
			right->nxt->prev = left->id;
		}
	}
	// 修改左孩子的num
	left->num += right->num;

	// 在父亲的keys中清除掉parent->key[mid]
	for (m = mid; m < parent->num - 1; m++) {
		parent->keys[m] = parent->keys[m + 1];
		parent->child[m + 1] = parent->child[m + 2];
	}
	for (int k = 0; k < parent->num; k++) {
		printf("parent keys after: %d\n", parent->keys[k]);
	}
	// 删除掉最后一个key，因为这个key已经到了左孩子left去了。
	parent->keys[m] = 0;
	parent->child[m + 1] = NULL;
	parent->num--;
	deallocTaIndexNode(right);

	/* Check if need to merge further */
	if (parent->num < bptree->min) {
		return bptree_merge(bptree, parent);
	}

	return 0;
}

static int bptree_merge(ta_idx_t *bptree, ta_idx_node_t *node) {
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
	/* 3. node: 最后一个孩子结点(left < node)
	 * node as right child */
	else if (idx == parent->num) { // node是父亲的最后一个child
		mid = idx - 1;
		left = parent->child[mid]; // 获取孩子节点，为合并或者借用做准备

		/* 1) 合并结点 */
		if ((node->num + left->num + 1) <= bptree->max) {
			return _bptree_merge(bptree, left, node, mid);
		}

		/* 2) 借用结点:brother->key[num-1] */
		for (m = node->num; m > 0; m--) { // 为借用节点腾出个位置
			node->keys[m] = node->keys[m - 1];
			if (node->leaf == 0) {
				node->child[m + 1] = node->child[m];
				node->chldrnIds[m + 1] = node->chldrnIds[m];
			} else
				node->tuIdxIds[m] = node->tuIdxIds[m - 1];
		}

		for (int k = 0; k < node->num; k++) {
			printf("++node keys after: %d\n", node->keys[k]);
			if (node->leaf == 1)
				printf("++node tuIdxIds after: %d\n", node->tuIdxIds[k]);
			else
				printf("++node chldrnIds after: %d\n", node->chldrnIds[k]);
		}
		if (node->leaf == 0) {
			node->child[1] = node->child[0];
			node->chldrnIds[1] = node->chldrnIds[0];
		}

		//node->key[0] = parent->key[mid];
		node->keys[0] = left->keys[left->num - 1];
		if (node->leaf == 1)
			node->tuIdxIds[0] = left->tuIdxIds[left->num - 1];

		node->num++;

		for (int k = 0; k < node->num; k++) {
			printf("++--node keys after: %d\n", node->keys[k]);
			if (node->leaf == 1)
				printf("++node tuIdxIds after: %d\n", node->tuIdxIds[k]);
			else
				printf("++node chldrnIds after: %d\n", node->chldrnIds[k]);
		}

		if (node->leaf == 0) {
			node->child[0] = left->child[left->num];
			if (NULL != left->child[left->num]) {
				left->child[left->num]->parent = node;
			}
		}

		parent->keys[mid] = left->keys[left->num - 1];
		printf("++parent->key[mid] after: %d\n", parent->keys[mid]);
		left->keys[left->num - 1] = 0;
		if (left->leaf == 0) {
			left->child[left->num] = NULL;
			left->chldrnIds[left->num] = 0;
		} else
			left->tuIdxIds[left->num] = 0;
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
		printf("left child keys before: %d\n", node->keys[k]);
		if (node->leaf == 1)
			printf("++node tuIdxIds before: %d\n", node->tuIdxIds[k]);
		else
			printf("++node chldrnIds before: %d\n", node->chldrnIds[k]);
	}

	/* 2) 借用结点: right->key[0] */
	node->keys[node->num] = parent->keys[mid];
	if (node->leaf == 1) {
		node->tuIdxIds[node->num] = parent->tuIdxIds[mid];
	}
	node->num++;
	if (node->leaf == 0) {
		node->child[node->num] = right->child[0];
		node->chldrnIds[node->num] = right->chldrnIds[0];
		if (NULL != right->child[0]) {
			right->child[0]->parent = node;
		}
	}
	for (int k = 0; k < node->num; k++) {
		printf("left child keys after: %d\n", node->keys[k]);
	}
	for (int k = 0; k < right->num; k++) {
		printf("right child keys before: %d\n", right->keys[k]);
	}
	//parent->key[mid] = right->key[0];
	for (m = 0; m < right->num; m++) {
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
		printf("parent keys after: %d\n", parent->keys[k]);
	}
	for (int k = 0; k < right->num; k++) {
		printf("right child keys before: %d\n", right->keys[k]);
	}

	if (right->leaf == 0) {
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
		// delete tuid from tu index id in DB
	}
	if (node->tuIdxIds[idx] == 0) {
		memcpy(node->keys + idx, node->keys + idx + 1,
				(node->num - idx - 1) * sizeof(int));
		memset(node->keys + node->num - 1, 0, sizeof(int));
		memcpy(node->tuIdxIds + idx, node->tuIdxIds + idx + 1,
				(node->num - idx - 1) * sizeof(int));
		memset(node->tuIdxIds + node->num - 1, 0, sizeof(int));
		node->num--;
	}

	for (int k = 0; k < node->num; k++) {
		printf("remained child keys: %d\n", node->keys[k]);
	}
	printf("--\n");

	if (node->num < bptree->min) {
		return bptree_merge(bptree, node);
	}
	return 0;
}

// 先测插入，后测删除
int taIndexDeleteNode(ta_idx_t *bptree, long long ts, long long tuid,
		FILE *ta_db_fp) {
	int idx = 0;
	ta_idx_node_t *node = bptree->root;

	while (NULL != node) {
		for (idx = 0; idx < node->num; idx++) {
			if (ts == node->keys[idx] && node->leaf == 1) {
				return _bptree_delete_leaf(bptree, node, idx, tuid);
			} else if (ts < node->keys[idx]) {
				break;
			}
		}

		if (node->leaf == 0) {
			if (node->child[idx] != NULL) {
				node = node->child[idx];
			} else {
				if (node->chldrnIds[idx] != 0) {
					node->child[idx] = readTaIndexPage(bptree,
							node->chldrnIds[idx] * ta_bptree_idx_node_bytes
									+ start_pointer, ta_bptree_idx_node_bytes,
							node, ta_db_fp);
					node = node->child[idx];
				}
			}
		} else {
			break;
		}

	}
	return 0;
}

void deallocTaIndexNode(ta_idx_node_t *node) {

}

//// initially load time axis index database
//// m should be 3 at least in B+tree, if m is 2, the tree is two-b tree
//void loadTaIndex(ta_bptree_idx_t *ta_bptree_idx, FILE *ta_bptree_idx_db_fp) {
//	unsigned char rootidBytes[LONG_LONG] = { 0 };
//	fseek(ta_bptree_idx_db_fp, 0, SEEK_SET); //
//	fread(rootidBytes, sizeof(unsigned char), LONG_LONG * 2,
//			ta_bptree_idx_db_fp);
//	long long rootId = byteArrayToLong(rootidBytes);
//	ta_bptree_idx = (ta_bptree_idx_t*) calloc(1, sizeof(ta_bptree_idx_t));
//	ta_bptree_idx->max = TA_BPLUS_TREE_M - 1;
//	ta_bptree_idx->min = TA_BPLUS_TREE_M / 2;
//	if (0 != TA_BPLUS_TREE_M % 2) { // Math.ceil(m/2)
//		ta_bptree_idx->min++;
//	}
//	ta_bptree_idx->min = ta_bptree_idx->min - 1; // 4
//	ta_bptree_idx->sidx = TA_BPLUS_TREE_M / 2;
//	ta_bptree_idx->root = NULL; /* empty tree */
//	ta_bptree_idx_node_t *root = readTaBpTreeNode(ta_bptree_idx, rootId,
//			tm_axis_bptree_node_bytes, NULL, ta_bptree_idx_db_fp);
//	ta_bptree_idx->root = root;
//}
//
//void deallocTaBptreeIndex(ta_bptree_idx_t *ta_bptree_idx) {
//
//}
//
//int splitTaBptreeNode(ta_bptree_idx_t *btree, ta_bptree_idx_node_t *node,
//		FILE *ta_bptree_idx_db_fp, FILE *ta_id_fp) {
//	int idx = 0, total = 0, sidx = btree->sidx;
//	ta_bptree_idx_node_t *parent = NULL, *node2 = NULL;
//	while (node->num > btree->max) {
//		/* Split node */
//		total = node->num;
//		long long id = getOneId(ta_id_fp, caches->taIds,
//				TIMEAXIS_ID_QUEUE_LENGTH);
//		ta_bptree_idx_node_t *node2 = readTaBpTreeNode(btree, id,
//				tm_axis_bptree_node_bytes, NULL, ta_bptree_idx_db_fp);
//		memcpy(node2->content, node->content + (sidx + 1) * 2 * LONG_LONG,
//				(total - sidx - 1) * 2 * LONG_LONG);
//		memset(node->content + (sidx + 1) * 2 * LONG_LONG, 0, 3LL * LONG_LONG + 1LL);
//		memcpy(node2->child, node->child + (sidx + 1) * 2 * LONG_LONG,
//				(total - sidx) * sizeof(ta_bptree_idx_node_t*));
//		node2->num = (total - sidx - 1);
//		node2->parent = node->parent;
//		node->num = sidx;
//		parent = node->parent;
//		if (NULL == parent) {
//			/* Split root node */
////			parent = btree_create_node(btree);
////			if (NULL == parent) {
////				fprintf(stderr, "[%s][%d] Create root failed!", __FILE__,
////				__LINE__);
////				return -1;
////			}
//			long long id1 = getOneId(ta_id_fp, caches->taIds,
//					TIMEAXIS_ID_QUEUE_LENGTH);
//			parent = readTaBpTreeNode(btree, id1,
//					tm_axis_bptree_node_bytes, NULL, ta_bptree_idx_db_fp);
//
//			btree->root = parent;
//			parent->child[0] = node;
//			node->parent = parent;
//			node2->parent = parent;
//
//			//parent->key[0] = node->key[sidx];
//			parent->child[1] = node2;
//			parent->num++;
//		}
//	}
//
//}
//
////
////	while (node->num > btree->max) {
////		/* Split node */
////		total = node->num;
////		size_t size = LONG + (3 * TA_BPLUS_TREE_M - 2) * LONG_LONG
////				+ 2 * LONG_LONG;
////		ta_bptree_idx_node_t *node2 = readTaBpTreeNode(btree, 0, size, node,
////				ta_bptree_idx_db_fp, ta_id_fp);
//////		node2 = btree_create_node(btree);
//////		if (NULL == node2) {
//////			fprintf(stderr, "[%s][%d] Create node failed!\n", __FILE__,
//////			__LINE__);
//////			return -1;
//////		}
////		/* Copy data */
////		memcpy(node2->content, node->content + sidx + 1,
////				(total - sidx - 1) * sizeof(int));
////		memcpy(node2->child, node->child + sidx + 1,
////				(total - sidx) * sizeof(ta_bptree_idx_node_t*));
////
////		node2->num = (total - sidx - 1);
////		node2->parent = node->parent;
////
////		node->num = sidx;
////		/* Insert into parent */
////		parent = node->parent;
////		if (NULL == parent) {
////			/* Split root node */
////			parent = btree_create_node(btree);
////			if (NULL == parent) {
////				fprintf(stderr, "[%s][%d] Create root failed!", __FILE__,
////				__LINE__);
////				return -1;
////			}
////
////			btree->root = parent;
////			parent->child[0] = node;
////			node->parent = parent;
////			node2->parent = parent;
////
////			parent->key[0] = node->key[sidx];
////			parent->child[1] = node2;
////			parent->num++;
////		} else {
////			/* Insert into parent node */
////			for (idx = parent->num; idx > 0; idx--) {
////				if (node->key[sidx] < parent->key[idx - 1]) {
////					parent->key[idx] = parent->key[idx - 1];
////					parent->child[idx + 1] = parent->child[idx];
////					continue;
////				}
////				break;
////			}
////
////			parent->key[idx] = node->key[sidx];
////			parent->child[idx + 1] = node2;
////			node2->parent = parent;
////			parent->num++;
////		}
////
////		memset(node->key + sidx, 0, (total - sidx) * sizeof(int));
////		memset(node->child + sidx + 1, 0,
////				(total - sidx) * sizeof(ta_btree_node_t*));
////
////		/* Change node2's child->parent */
////		for (idx = 0; idx <= node2->num; idx++) {
////			if (NULL != node2->child[idx]) {
////				node2->child[idx]->parent = node2;
////			}
////		}
////		node = parent;
////	}
////
////	return 0;
////}
//
//int insertBptreeNode(ta_bptree_idx_t *btree, ta_bptree_idx_node_t *node,
//		long long ts, int idx, long long tuIdxId) {
//	int i = 0;
//	unsigned char tsBytes[LONG_LONG] = { 0 };
//	longToByteArray(ts, tsBytes);	// convert time stamp to byte array
//	unsigned char tuIdxIdBytes[LONG_LONG] = { 0 };
//	longToByteArray(tuIdxId, tuIdxIdBytes);	// convert tu index id to byte array
//
//	/* 1. 移动关键字:首先在最底层的某个非终端结点上插入一个关键字,
//	 * 因此该结点无孩子结点，故不涉及孩子指针的移动操作 */
//	if (node->num > 0) {
//		size_t l = (node->num - idx - 1) * 2 * LONG_LONG;
//		unsigned char *reBytes = (unsigned char*) calloc(l,
//				sizeof(unsigned char));
//		memcpy(reBytes, node->content + idx * 2 * LONG_LONG + LONG, l);
//		memcpy(node->content + (idx + 1) * 2 * LONG_LONG + LONG, reBytes, l);
//	}
//	node->num++;
//	unsigned char nBytes[LONG] = { 0 };
//	integer2Bytes(node->num, nBytes);
//	memcpy(node->content, nBytes, LONG);
//	memcpy(tsBytes, node->content + idx * 2 * LONG_LONG + LONG, LONG_LONG);
//	memcpy(tuIdxIdBytes, node->content + (idx * 2 + 1) * LONG_LONG + LONG,
//	LONG_LONG);
//	node->hit++;
//	node->dirty = 1;
//
//	/* 2. 分裂处理 */
//	if (node->num > btree->max) {
//		//return splitTaBptreeNode(btree, node);
//	}
//
//	return 0;
//}
//
//long long insertTaBptreeNode(ta_bptree_idx_t *btree, long long ts,
//		long long tuid, FILE *ta_bptree_idx_db_fp, FILE *ta_id_fp) {
//	int idx = 0;
//	ta_bptree_idx_node_t *nd = btree->root;
//
//	if (nd && nd->num == 0) {
//		return insertBptreeNode(btree, nd, ts, idx, tuid);
//	}
//
//	// search time stamp
//	while (nd) {
//		for (idx = 0; idx < nd->num; idx++) {
//			// parse time stamp
//			long long ts1 = byteArrayToLonglong(
//					nd->content + idx * 2 * LONG_LONG);
//			if (ts <= ts1) {
//				break;
//			}
//		}
//		if (nd->child[idx]) {
//			nd = nd->child[idx];
//		} else {
//			long long child1 = byteArrayToLonglong(
//					nd->content + (idx * 2 + 1) * LONG_LONG);
//			if (child1 != 0) {
////				ta_bptree_idx_node_t *p = readTaBpTreeNode(btree, child1,
////						tm_axis_bptree_node_bytes, nd, ta_bptree_idx_db_fp,
////						ta_id_fp);
////				nd->child[idx] = p;
////				nd = p;
////				p = NULL;
//				continue;
//			}
//			break;
//
////			unsigned char *pos = nd->content + (btree->max * 2 * LONG_LONG)
////					+ idx * LONG_LONG;
////			unsigned char chdBytes[LONG_LONG] = { 0 };
////			memcpy(chdBytes, pos, LONG_LONG);
////			long long child = byteArrayToLong(chdBytes); // parse child bytes
////			if (0 != child) {
////				ta_bptree_idx_node_t *chld = readTaBpTreeNode(child,
////						(3 * TA_BPLUS_TREE_M - 1) * LONG_LONG,
////						ta_bptree_idx_db_fp, NULL);
////				nd->child[idx] = chld;
////				nd = chld;
////			} else {
////				break;
////			}
//		}
//	}
//
//	/* 2. 查找插入位置：在此当然也可以采用二分查找算法，有兴趣的可以自己去优化 */
//	// while (NULL != node) {
//	//		for (idx = 0; idx < node->keyNum; idx++) {
//	//			if (key == node->key[idx]) {
//	//				fprintf(stderr, "[%s][%d] The node is exist!\n", __FILE__,
//	//				__LINE__);
//	//				return 0;
//	//			} else if (key < node->key[idx]) {
//	//				break;
//	//			}
//	//		}
//	//
//	//		if (NULL != node->child[idx]) {
//	//			node = node->child[idx];
//	//		} else {
//	//			break;
//	//		}
//	// }
////	while (NULL != nd) {
////		while (idx < btree->max) {
////			long long ts1 = byteArrayToLonglong(
////					nd->content + (idx * 2 * LONG_LONG)); // parse time stamp
////			if (ts1 == 0) {
////				break;
////			}
////			if (ts == ts1) {
////				long long tuId = byteArrayToLonglong(
////						nd->content + (idx * 2 * LONG_LONG + LONG_LONG)); // parse tu id
////				if (tuId == 0) {
////					// get the next, if it is -2, it will be not a leaf node
////					unsigned char *p = nd->content
////							+ (3 * btree->max) * LONG_LONG;
////					unsigned char nxtBytes[LONG_LONG] = { 0 };
////					memcpy(nxtBytes, p, LONG_LONG);
////					long long nxt = byteArrayToLonglong(nd->content
////							+ (3 * btree->max) * LONG_LONG); // parse time stamp
////					if (nxt != -2) {
////						longToByteArray(tuIdBytes, tuidxid); // next block Id
////						// write a tuid
////						memcpy(
////								nd->content
////										+ (idx * 2 * LONG_LONG + LONG_LONG),
////								tuIdBytes, LONG_LONG);
////						return tuidxid;
////					}
////				} else {
////					return tuId; // return the head of TuId linked list in Tu Id index tdb
////				}
////			} else if (ts < ts1) {
////				break;
////			}
////			idx++;
////		}
////		if (NULL != nd->child[idx]) {
////			nd = nd->child[idx];
////		} else {
////			unsigned char *pos = nd->content + (btree->max * 2 * LONG_LONG)
////					+ idx * LONG_LONG;
////			unsigned char chdBytes[LONG_LONG] = { 0 };
////			memcpy(chdBytes, pos, LONG_LONG);
////			long long child = byteArrayToLong(chdBytes); // parse child bytes
////			if (0 != child) {
////				ta_bptree_idx_node_t *chld = readTaBpTreeNode(child,
////						(3 * TA_BPLUS_TREE_M - 1) * LONG_LONG,
////						ta_bptree_idx_db_fp, NULL);
////				nd->child[idx] = chld;
////				nd = chld;
////			} else {
////				break;
////			}
////		}
////	}
//	/* 3. 执行插入操作 */
//	return insertBptreeNode(btree, nd, ts, idx, tuid);
//}

