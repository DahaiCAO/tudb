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

// the convert utility for long long to byte array
void longlongToByteArray(long long value, unsigned char *buffer) {
	memset(buffer, 0, LONG_LONG);
	for (int i = 0; i < LONG_LONG; i++) {
		*(buffer + i) = ((value >> (LONG_LONG * i)) & 0XFF);
	}
}

// the utility for converting byte array to long long
long long byteArrayToLonglong(unsigned char *buffer) {
	long long recoveredValue = 0;
	for (int i = 0; i < LONG_LONG; i++) {
		long long byteVal = (((long long) (buffer + i)) << (LONG_LONG * i));
		recoveredValue = recoveredValue | byteVal;
	}
	return recoveredValue;
}

ta_bptree_idx_node_t* loadTaBptreeNode(ta_bptree_idx_t *btree,
		unsigned char *page, long long *id, ta_bptree_idx_node_t *parent) {
	ta_bptree_idx_node_t *node = (ta_bptree_idx_node_t*) calloc(1,
			sizeof(ta_bptree_idx_node_t));
	node->id = id;
	node->content = page;
	node->dirty = 0;
	node->expiretime = TIME_AXIS_NODE_EXPIRE_TIME; // 10 minutes
	node->hit = 0;
	unsigned char *nbytes = (unsigned char*) calloc(LONG,
			sizeof(unsigned char));
	memcpy(nbytes, page, LONG);
	/* 实际关键字个数 */
	node->num = bytes2Integer(nbytes);
	/* 关键字：所占空间为(max+1) - 多出来的1个空间用于交换空间使用 */
	//p->key = (int*) calloc(btree->max + 1, sizeof(int));
	/* More than (max+1) is for move */
	/* 子结点：所占空间为（max+2）- 多出来的1个空间用于交换空间使用 */
	node->child = (ta_bptree_idx_node_t**) calloc(btree->max + 2,
			sizeof(ta_bptree_idx_node_t*));
	node->parent = parent; /* 父结点 */
	node->nxt = NULL; /* 下一个结点 */
	node->prv = NULL; /* 上一个结点 */
	node->nxtpage = NULL;

	// construct a linked list for page management.
	ta_bptree_idx_node_t *pp = ta_bptree_idx->root;
	if (pp != NULL) {
		while (pp->nxtpage != NULL) {
			pp = pp->nxtpage;
		}
		pp->nxtpage = node;
		pp = NULL;
	}
	return node;
}

ta_bptree_idx_node_t* readTaBpTreeNode(ta_bptree_idx_t *btree, long long id,
		size_t size, ta_bptree_idx_node_t *parent, FILE *ta_bptree_idx_db_fp) {
	// read one page data (one b+ tree node) to memory
	unsigned char *page = (unsigned char*) calloc(size, sizeof(unsigned char));
	fseek(ta_bptree_idx_db_fp, id, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(ta_bptree_idx_db_fp)) != EOF) {
		fseek(ta_bptree_idx_db_fp, id, SEEK_SET);
		fread(page, sizeof(unsigned char), size, ta_bptree_idx_db_fp); // read one page
	}
	// using this page data to construct one b+ tree node
	return loadTaBptreeNode(btree, page, id, parent);
}

// initially load time axis index database
// m should be 3 at least in B+tree, if m is 2, the tree is two-b tree
void loadTaIndex(ta_bptree_idx_t *ta_bptree_idx, FILE *ta_bptree_idx_db_fp) {
	unsigned char rootidBytes[LONG_LONG] = { 0 };
	fseek(ta_bptree_idx_db_fp, 0, SEEK_SET); //
	fread(rootidBytes, sizeof(unsigned char), LONG_LONG * 2,
			ta_bptree_idx_db_fp);
	long long rootId = byteArrayToLong(rootidBytes);
	ta_bptree_idx = (ta_bptree_idx_t*) calloc(1, sizeof(ta_bptree_idx_t));
	ta_bptree_idx->max = TA_BPLUS_TREE_M - 1;
	ta_bptree_idx->min = TA_BPLUS_TREE_M / 2;
	if (0 != TA_BPLUS_TREE_M % 2) { // Math.ceil(m/2)
		ta_bptree_idx->min++;
	}
	ta_bptree_idx->min = ta_bptree_idx->min - 1; // 4
	ta_bptree_idx->sidx = TA_BPLUS_TREE_M / 2;
	ta_bptree_idx->root = NULL; /* empty tree */
	ta_bptree_idx_node_t *root = readTaBpTreeNode(ta_bptree_idx, rootId,
			tm_axis_bptree_node_bytes, NULL, ta_bptree_idx_db_fp);
	ta_bptree_idx->root = root;
}

void deallocTaBptreeIndex(ta_bptree_idx_t *ta_bptree_idx) {

}

int splitTaBptreeNode(ta_bptree_idx_t *btree, ta_bptree_idx_node_t *node,
		FILE *ta_bptree_idx_db_fp, FILE *ta_id_fp) {
	int idx = 0, total = 0, sidx = btree->sidx;
	ta_bptree_idx_node_t *parent = NULL, *node2 = NULL;
	while (node->num > btree->max) {
		/* Split node */
		total = node->num;
		long long id = getOneId(ta_id_fp, caches->taIds,
				TIMEAXIS_ID_QUEUE_LENGTH);
		ta_bptree_idx_node_t *node2 = readTaBpTreeNode(btree, id,
				tm_axis_bptree_node_bytes, NULL, ta_bptree_idx_db_fp);
		memcpy(node2->content, node->content + (sidx + 1) * 2 * LONG_LONG,
				(total - sidx - 1) * 2 * LONG_LONG);
		memset(node->content + (sidx + 1) * 2 * LONG_LONG, 0, 3LL * LONG_LONG + 1LL);
		memcpy(node2->child, node->child + (sidx + 1) * 2 * LONG_LONG,
				(total - sidx) * sizeof(ta_bptree_idx_node_t*));
		node2->num = (total - sidx - 1);
		node2->parent = node->parent;
		node->num = sidx;
		parent = node->parent;
		if (NULL == parent) {
			/* Split root node */
//			parent = btree_create_node(btree);
//			if (NULL == parent) {
//				fprintf(stderr, "[%s][%d] Create root failed!", __FILE__,
//				__LINE__);
//				return -1;
//			}
			long long id1 = getOneId(ta_id_fp, caches->taIds,
					TIMEAXIS_ID_QUEUE_LENGTH);
			parent = readTaBpTreeNode(btree, id1,
					tm_axis_bptree_node_bytes, NULL, ta_bptree_idx_db_fp);

			btree->root = parent;
			parent->child[0] = node;
			node->parent = parent;
			node2->parent = parent;

			//parent->key[0] = node->key[sidx];
			parent->child[1] = node2;
			parent->num++;
		}
	}

}

//
//	while (node->num > btree->max) {
//		/* Split node */
//		total = node->num;
//		size_t size = LONG + (3 * TA_BPLUS_TREE_M - 2) * LONG_LONG
//				+ 2 * LONG_LONG;
//		ta_bptree_idx_node_t *node2 = readTaBpTreeNode(btree, 0, size, node,
//				ta_bptree_idx_db_fp, ta_id_fp);
////		node2 = btree_create_node(btree);
////		if (NULL == node2) {
////			fprintf(stderr, "[%s][%d] Create node failed!\n", __FILE__,
////			__LINE__);
////			return -1;
////		}
//		/* Copy data */
//		memcpy(node2->content, node->content + sidx + 1,
//				(total - sidx - 1) * sizeof(int));
//		memcpy(node2->child, node->child + sidx + 1,
//				(total - sidx) * sizeof(ta_bptree_idx_node_t*));
//
//		node2->num = (total - sidx - 1);
//		node2->parent = node->parent;
//
//		node->num = sidx;
//		/* Insert into parent */
//		parent = node->parent;
//		if (NULL == parent) {
//			/* Split root node */
//			parent = btree_create_node(btree);
//			if (NULL == parent) {
//				fprintf(stderr, "[%s][%d] Create root failed!", __FILE__,
//				__LINE__);
//				return -1;
//			}
//
//			btree->root = parent;
//			parent->child[0] = node;
//			node->parent = parent;
//			node2->parent = parent;
//
//			parent->key[0] = node->key[sidx];
//			parent->child[1] = node2;
//			parent->num++;
//		} else {
//			/* Insert into parent node */
//			for (idx = parent->num; idx > 0; idx--) {
//				if (node->key[sidx] < parent->key[idx - 1]) {
//					parent->key[idx] = parent->key[idx - 1];
//					parent->child[idx + 1] = parent->child[idx];
//					continue;
//				}
//				break;
//			}
//
//			parent->key[idx] = node->key[sidx];
//			parent->child[idx + 1] = node2;
//			node2->parent = parent;
//			parent->num++;
//		}
//
//		memset(node->key + sidx, 0, (total - sidx) * sizeof(int));
//		memset(node->child + sidx + 1, 0,
//				(total - sidx) * sizeof(ta_btree_node_t*));
//
//		/* Change node2's child->parent */
//		for (idx = 0; idx <= node2->num; idx++) {
//			if (NULL != node2->child[idx]) {
//				node2->child[idx]->parent = node2;
//			}
//		}
//		node = parent;
//	}
//
//	return 0;
//}

int insertBptreeNode(ta_bptree_idx_t *btree, ta_bptree_idx_node_t *node,
		long long ts, int idx, long long tuIdxId) {
	int i = 0;
	unsigned char tsBytes[LONG_LONG] = { 0 };
	longToByteArray(ts, tsBytes);	// convert time stamp to byte array
	unsigned char tuIdxIdBytes[LONG_LONG] = { 0 };
	longToByteArray(tuIdxId, tuIdxIdBytes);	// convert tu index id to byte array

	/* 1. 移动关键字:首先在最底层的某个非终端结点上插入一个关键字,
	 * 因此该结点无孩子结点，故不涉及孩子指针的移动操作 */
	if (node->num > 0) {
		size_t l = (node->num - idx - 1) * 2 * LONG_LONG;
		unsigned char *reBytes = (unsigned char*) calloc(l,
				sizeof(unsigned char));
		memcpy(reBytes, node->content + idx * 2 * LONG_LONG + LONG, l);
		memcpy(node->content + (idx + 1) * 2 * LONG_LONG + LONG, reBytes, l);
	}
	node->num++;
	unsigned char nBytes[LONG] = { 0 };
	integer2Bytes(node->num, nBytes);
	memcpy(node->content, nBytes, LONG);
	memcpy(tsBytes, node->content + idx * 2 * LONG_LONG + LONG, LONG_LONG);
	memcpy(tuIdxIdBytes, node->content + (idx * 2 + 1) * LONG_LONG + LONG,
	LONG_LONG);
	node->hit++;
	node->dirty = 1;

	/* 2. 分裂处理 */
	if (node->num > btree->max) {
		//return splitTaBptreeNode(btree, node);
	}

	return 0;
}

long long insertTaBptreeNode(ta_bptree_idx_t *btree, long long ts,
		long long tuid, FILE *ta_bptree_idx_db_fp, FILE *ta_id_fp) {
	int idx = 0;
	ta_bptree_idx_node_t *nd = btree->root;

	if (nd && nd->num == 0) {
		return insertBptreeNode(btree, nd, ts, idx, tuid);
	}

	// search time stamp
	while (nd) {
		for (idx = 0; idx < nd->num; idx++) {
			// parse time stamp
			long long ts1 = byteArrayToLonglong(
					nd->content + idx * 2 * LONG_LONG);
			if (ts <= ts1) {
				break;
			}
		}
		if (nd->child[idx]) {
			nd = nd->child[idx];
		} else {
			long long child1 = byteArrayToLonglong(
					nd->content + (idx * 2 + 1) * LONG_LONG);
			if (child1 != 0) {
//				ta_bptree_idx_node_t *p = readTaBpTreeNode(btree, child1,
//						tm_axis_bptree_node_bytes, nd, ta_bptree_idx_db_fp,
//						ta_id_fp);
//				nd->child[idx] = p;
//				nd = p;
//				p = NULL;
				continue;
			}
			break;

//			unsigned char *pos = nd->content + (btree->max * 2 * LONG_LONG)
//					+ idx * LONG_LONG;
//			unsigned char chdBytes[LONG_LONG] = { 0 };
//			memcpy(chdBytes, pos, LONG_LONG);
//			long long child = byteArrayToLong(chdBytes); // parse child bytes
//			if (0 != child) {
//				ta_bptree_idx_node_t *chld = readTaBpTreeNode(child,
//						(3 * TA_BPLUS_TREE_M - 1) * LONG_LONG,
//						ta_bptree_idx_db_fp, NULL);
//				nd->child[idx] = chld;
//				nd = chld;
//			} else {
//				break;
//			}
		}
	}

	/* 2. 查找插入位置：在此当然也可以采用二分查找算法，有兴趣的可以自己去优化 */
	// while (NULL != node) {
	//		for (idx = 0; idx < node->keyNum; idx++) {
	//			if (key == node->key[idx]) {
	//				fprintf(stderr, "[%s][%d] The node is exist!\n", __FILE__,
	//				__LINE__);
	//				return 0;
	//			} else if (key < node->key[idx]) {
	//				break;
	//			}
	//		}
	//
	//		if (NULL != node->child[idx]) {
	//			node = node->child[idx];
	//		} else {
	//			break;
	//		}
	// }
//	while (NULL != nd) {
//		while (idx < btree->max) {
//			long long ts1 = byteArrayToLonglong(
//					nd->content + (idx * 2 * LONG_LONG)); // parse time stamp
//			if (ts1 == 0) {
//				break;
//			}
//			if (ts == ts1) {
//				long long tuId = byteArrayToLonglong(
//						nd->content + (idx * 2 * LONG_LONG + LONG_LONG)); // parse tu id
//				if (tuId == 0) {
//					// get the next, if it is -2, it will be not a leaf node
//					unsigned char *p = nd->content
//							+ (3 * btree->max) * LONG_LONG;
//					unsigned char nxtBytes[LONG_LONG] = { 0 };
//					memcpy(nxtBytes, p, LONG_LONG);
//					long long nxt = byteArrayToLonglong(nd->content
//							+ (3 * btree->max) * LONG_LONG); // parse time stamp
//					if (nxt != -2) {
//						longToByteArray(tuIdBytes, tuidxid); // next block Id
//						// write a tuid
//						memcpy(
//								nd->content
//										+ (idx * 2 * LONG_LONG + LONG_LONG),
//								tuIdBytes, LONG_LONG);
//						return tuidxid;
//					}
//				} else {
//					return tuId; // return the head of TuId linked list in Tu Id index tdb
//				}
//			} else if (ts < ts1) {
//				break;
//			}
//			idx++;
//		}
//		if (NULL != nd->child[idx]) {
//			nd = nd->child[idx];
//		} else {
//			unsigned char *pos = nd->content + (btree->max * 2 * LONG_LONG)
//					+ idx * LONG_LONG;
//			unsigned char chdBytes[LONG_LONG] = { 0 };
//			memcpy(chdBytes, pos, LONG_LONG);
//			long long child = byteArrayToLong(chdBytes); // parse child bytes
//			if (0 != child) {
//				ta_bptree_idx_node_t *chld = readTaBpTreeNode(child,
//						(3 * TA_BPLUS_TREE_M - 1) * LONG_LONG,
//						ta_bptree_idx_db_fp, NULL);
//				nd->child[idx] = chld;
//				nd = chld;
//			} else {
//				break;
//			}
//		}
//	}
	/* 3. 执行插入操作 */
	return insertBptreeNode(btree, nd, ts, idx, tuid);
}

