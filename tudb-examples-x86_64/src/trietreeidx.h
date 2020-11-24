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
 * trietreeidx.h
 *
 * Created on: 2020年11月11日
 * Author: Dahai CAO
 */

#ifndef TRIETREEIDX_H_
#define TRIETREEIDX_H_

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//
//#include "error.h"
//#include "list.h"
//
//#define TRIE_FANOUT 10
//
///* - - - 键树节点结构 - - - */
//typedef struct trie_s {
//	int subchanged[TRIE_FANOUT]; /* 记录子节点发生变化的情况 */
//	int listchanged; /* 记录所属链表的变化情况 */
//	list_t *list;
//	struct trie_s *subtrie[TRIE_FANOUT];
//} trie_t;
//
//void InitHTrie(trie_t **trie);
//
//int InsertHTrie(trie_t **trie, const char str[], const int level, void *data,
//		size_t n, int changed);
//
//void* SearchHTrie(trie_t *trie, const char str[], const int level, void *data,
//		int (*cmp)(const void *data1, const void *data2));
//
//int TouchHTrie(trie_t **trie, const char str[], const int level, list_t ***list);
//
//list_t* GetListofHTrie(trie_t *trie, const char str[], const int level);
//
//void PrintHTrie(trie_t *trie, const int level, const int key,
//		void (*print)(const void *data));
///*
// void OperateTrie (trie_t *trie, void (* op_list) (void *data));
// */
//void RefreshHTrie(trie_t *trie, void (*op_data)(void *data));
//
//void FreeHTrie(trie_t *trie);
//
//int NeedRefresh(trie_t *trie);
//
///*
// * 最大可能匹配树查找
// */
//list_t* MatchHTrie(trie_t *trie, const char str[], const int level);
//
///*
// * 功能： TRIE树遍历操作函数
// *
// * 注意 节点操作可中断
// *
// * 返回 0 未执行完毕 1 执行完毕
// */
//int DealHTrie(trie_t *trie, int (*op_data)(void *data));

#endif /* TRIETREEIDX_H_ */
