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
 * trietreeidx.c
 *
 * Created on: 2020年11月11日
 * Author: Dahai CAO
 */

//#include <stdio.h>
//#include <ctype.h>
//
//#include "trietreeidx.h"
//#include "alloc.h"
//
//static char keyarray[256];
//
///*-------------------------------
// * usage: 初始化键值数组
// * comment:将char映射成0-9的数值
// *-------------------------------*/
//void InitHTrie(trie_t **trie) {
//	int c;
//
//	for (c = 0; c < 256; c++) {
//		if (isdigit(c))
//			keyarray[c] = c - '0';
//		else
//			keyarray[c] = c % 10;
//	}
//
//	*trie = NULL;
//}
//
//static trie_t* NewNode() {
//	int i;
//	trie_t *node = (trie_t*) allocate(sizeof(trie_t));
//
//	if (node) {
//		node->list = NULL;
//		for (i = 0; i < TRIE_FANOUT; i++) {
//			node->subchanged[i] = 0;
//			node->listchanged = 0;
//			node->subtrie[i] = NULL;
//		}
//	}
//
//	if (node == NULL)
//		pr_error("errorcode:%d,msg:NewNode: allocate() return NULL/n");
//
//	return node;
//}
//
///*--------------------------------
// * usage: 向键树中插入一个新的数据
// * arguments: *trie -- 键树头指针
// *   str   -- 键值字符串
// *   level -- 键值长度
// *   data  -- 要插入的数据
// *   n     -- 要插入的数据的大小
// *   changed - 记录当前节点的子节点的内容是否发生了变化
// *     1 -- 有变化 0 -- 无变化
// * return: -1 -- 出错 0 -- 正常
// * comment：键树的叶节点是链表，出入数据时，先根据键值找到叶节点，再向
// *   叶节点所指的链表中插入数据。
// *---------------------------------*/
//int InsertHTrie(trie_t **trie, const char str[], const int level, void *data,
//		size_t n, int changed) {
//	int i;
//	int key;
//	trie_t *curnode;
//
//	if (*trie == NULL) {
//		*trie = NewNode();
//		if (*trie == NULL) {
//			return -1;
//		}
//	}
//
//	curnode = *trie;
//	for (i = 0; i < level; i++) {
//		key = (int) keyarray[(int) str[i]];
//		if (curnode->subtrie[key] == NULL) {
//			if ((curnode->subtrie[key] = NewNode()) == NULL)
//				return -1;
//		}
//		curnode->subchanged[key] = changed;
//
//		curnode = curnode->subtrie[key];
//	}
//
//	curnode->listchanged = changed;
//
//	return (InsertList(&(curnode->list), data, n));
//}
//
///*--------------------------------
// * usage: 在键树中查找数据
// * arguments: trie  -- 键树头指针
// *   str   -- 键值字符串
// *   level -- 键值长度
// *   data  -- 要查找的数据
// *   cmp   -- 比较函数指针
// * return: 找到 -- 返回指向该数据的指针 没找到 -- NULL
// * comment:查找规则由cmp函数指定
// *---------------------------------*/
//void* SearchHTrie(trie_t *trie, const char str[], const int level, void *data,
//		int (*cmp)(const void *data1, const void *data2)) {
//	int i;
//	int key;
//	trie_t *curnode;
//
//	if (trie == NULL)
//		return NULL;
//
//	curnode = trie;
//	for (i = 0; i < level; i++) {
//		key = (int) keyarray[(int) str[i]];
//		if (curnode->subtrie[key] == NULL)
//			return NULL;
//
//		curnode = curnode->subtrie[key];
//	}
//
//	return (SearchList(curnode->list, data, cmp));
//}
//
///*--------------------------------
// * usage: 在键树中查找键值指向的链头。并将经过的节点的changed字段置1
// *  表示该节点的子节点要发生变化。如节点不存在，则生成该节点
// * arguments: trie  -- 键树头指针
// *   str   -- 键值字符串
// *   level -- 键值长度
// *   list  -- 保存指向链头list指针的指针，由于要保存指针的指针，
// *     使用3层指针
// * return: 找到 -- 返回指向该链表头的指针 没找到 -- NULL
// * comment:
// *---------------------------------*/
//int TouchHTrie(trie_t **trie, const char str[], const int level, list_t ***list) {
//	int i;
//	int key;
//	trie_t *curnode;
//
//	if (*trie == NULL) {
//		*trie = NewNode();
//		if (*trie == NULL) {
//			pr_error("errorcode:%d,msg:TouchHTrie:NewNode () return NULL/n");
//
//			return -1;
//		}
//	}
//
//	curnode = *trie;
//	for (i = 0; i < level; i++) {
//		key = (int) keyarray[(int) str[i]];
//		if (curnode->subtrie[key] == NULL) {
//			if ((curnode->subtrie[key] = NewNode()) == NULL) {
//				pr_error("errorcode:%d,msg:NewNode () error/n");
//				return -1;
//			}
//		}
//		curnode->subchanged[key] = 1;
//
//		curnode = curnode->subtrie[key];
//	}
//
//	curnode->listchanged = 1;
//
//	*list = &(curnode->list);
//
//	return 0;
//}
//
///*-------------------------------------------
// *
// *-------------------------------------------*/
//list_t* GetListofHTrie(trie_t *trie, const char str[], const int level) {
//	int i;
//	int key;
//	trie_t *curnode;
//
//	if (trie == NULL)
//		return NULL;
//
//	curnode = trie;
//	for (i = 0; i < level; i++) {
//		key = (int) keyarray[(int) str[i]];
//		if (curnode->subtrie[key] == NULL)
//			return NULL;
//
//		curnode = curnode->subtrie[key];
//	}
//
//	return curnode->list;
//}
//
//list_t* MatchHTrie(trie_t *trie, const char str[], const int level) {
//	int i;
//	int key;
//	trie_t *curnode;
//
//	if (trie == NULL)
//		return NULL;
//
//	curnode = trie;
//
//	for (i = 0; i < level; i++) {
//		key = (int) keyarray[(int) str[i]];
//		if (curnode->subtrie[key] == NULL)
//			return curnode->list;
//
//		curnode = curnode->subtrie[key];
//	}
//
//	return curnode->list;
//}
//
///*-------------------------------
// * usage: 释放键树
// * arguments: trie -- the head of trie
// *-------------------------------*/
//void FreeHTrie(trie_t *trie) {
//	int i;
//
//	if (trie) {
//		for (i = 0; i < TRIE_FANOUT; i++)
//			FreeHTrie(trie->subtrie[i]);
//		FreeList(trie->list);
//		free(trie);
//	}
//}
//
///*----------------------------------
// * usage: print the data of the trie
// *----------------------------------*/
//void PrintHTrie(trie_t *trie, const int level, const int key,
//		void (*print)(const void *data)) {
//	int i;
//
//	if (trie) {
//		fprintf(stderr, "enter subtrie -- level:%d,key:%d/n", level, key);
//		for (i = 0; i < TRIE_FANOUT; i++) {
//			PrintHTrie(trie->subtrie[i], level + 1, i, print);
//		}
//		PrintList(trie->list, print);
//	}
//}
///*
// void OperateHTrie (trie_t *trie, void (* op_list) (void *data))
// {
//
// }
// */
//
///*------------------------------------------
// * usage: 刷新TRIE，对changed为1的节点的子节点的链表做op_list指定的操作
// * parameters: trie   -- trie head pointer
// *   op_list-- 对list的操作
// *------------------------------------------*/
//void RefreshHTrie(trie_t *trie, void (*op_data)(void *data)) {
//	int i;
//
//	if (trie) {
//		for (i = 0; i < TRIE_FANOUT; i++) {
//			if (trie->subchanged[i]) /* 子节点发生过变化 */
//			{
//				RefreshHTrie(trie->subtrie[i], op_data);
//				trie->subchanged[i] = 0;
//			}
//		}
//		if (trie->listchanged) {
//			OperateList(trie->list, op_data);
//			trie->listchanged = 0;
//		}
//	}
//}
//
//int NeedRefresh(trie_t *trie) {
//	int i;
//
//	if (trie) {
//		for (i = 0; i < TRIE_FANOUT; i++) {
//			if (trie->subchanged[i]) /* 子节点发生过变化 */
//			{
//				return 1;
//			}
//		}
//		if (trie->listchanged) {
//			return 1;
//		}
//	}
//
//	return 0;
//}
//
///*
// * 功能： TRIE树遍历操作函数
// *
// * 注意 节点操作可中断
// *
// * 返回 0 未执行完毕 1 执行完毕
// */
//int DealHTrie(trie_t *trie, int (*op_data)(void *data)) {
//	int i;
//
//	if (trie) {
//		for (i = 0; i < TRIE_FANOUT; i++) {
//			if (trie->subchanged[i]) /* 子节点发生过变化 */
//			{
//				/* 字节点操作中断， 返回 0 */
//				if (DealHTrie(trie->subtrie[i], op_data) == 0)
//					return 0;
//
//				trie->subchanged[i] = 0;
//			}
//		}
//		if (trie->listchanged) {
//			if (DealList(trie->list, op_data) == 0)
//				return 0;
//			trie->listchanged = 0;
//		}
//	}
//
//	return 1;
//}

