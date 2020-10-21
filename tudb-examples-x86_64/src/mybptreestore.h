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
 * mybptreestore.h
 *
 * Created on: 2020年10月17日
 * Author: Dahai CAO
 */

#ifndef MYBPTREESTORE_H_
#define MYBPTREESTORE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define M (4)


// this structure describes non-cluster B+ tree index node, one page is one node
typedef struct bplustree_index_page {
	long long expiretime; // expiration time to mark this page, -1 by default, that means blank page
	int dirty; // if dirty, then 1; otherwise, 0;
	int hit; // hit counting, 0 by default, hit once, plus 1;

	unsigned char *content; // buffer for page content. including keys Id and children ids
	int keynum; /* 关键字个数 */
	long long *key; /* 关键字：所占空间为(max+1) - 多出来的1个空间用于交换空间使用 */
	struct bplustree_index_page **child; /* 子结点：所占空间为（max+2）- 多出来的1个空间用于交换空间使用 */
	struct bplustree_index_page *parent; /* 父结点 */
	struct bplustree_index_page *next; /* 下一个叶子结点 */
} bptree_idx_page_t;

int BPLUSTREE_INDEX_PAGE_SIZE;

bptree_idx_page_t *lbl_bptree;

#endif /* MYBPTREESTORE_H_ */
