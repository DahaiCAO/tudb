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
#include "lbltknstore.h"
#include <stdbool.h>
/*
 * lbltknstore.c
 *
 * Created on: 2020年8月13日
 * Author: Dahai CAO
 */

// read one label token page
// start the start pointer in memory
// start_no the start record id in this page
// lbl_tkn_db_fp label token DB file
lbl_tkn_page_t* readOneLabelTokenPage(long long start, long long start_no,
		FILE *lbl_tkn_db_fp) {
	lbl_tkn_page_bytes = lbl_tkn_record_bytes * lbl_tkn_page_records;
	unsigned char *page = (unsigned char*) malloc(
			sizeof(unsigned char) * lbl_tkn_page_bytes);
	memset(page, start_no, sizeof(unsigned char) * lbl_tkn_page_bytes);
	fseek(lbl_tkn_db_fp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(lbl_tkn_db_fp)) != EOF) {
		fseek(lbl_tkn_db_fp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), lbl_tkn_page_bytes, lbl_tkn_db_fp); // read one page
	}

	lbl_tkn_page_t *p = (lbl_tkn_page_t*) malloc(sizeof(lbl_tkn_page_t));
	p->dirty = 0;
	p->expiretime = 10; // 10 minutes
	p->startNo = start_no;
	p->hit = 0;
	p->nxtpage = NULL;
	p->prvpage = NULL;
	p->content = page;
	p->start = start;
	p->end = start + lbl_tkn_page_bytes;

	lbl_tkn_page_t *pp = lbl_tkn_pages;
	if (pp != NULL) {
		while (pp->nxtpage != NULL) {
			pp = pp->nxtpage;
		}
		pp->nxtpage = p;
		p->prvpage = pp;
	} else {
		lbl_tkn_pages = p;
	}
	return p;
}

void initLabelTokenDBMemPages(lbl_tkn_page_t *pages, FILE *lbl_tkn_db_fp) {
	if (pages == NULL) {
		readOneLabelTokenPage(0LL, 0LL, lbl_tkn_db_fp);
	}
}

int UnicodeToUtf8(char *pInput, char *pOutput) {
	int len = 0; //记录转换后的Utf8字符串的字节数
	while (*pInput) { //处理一个unicode字符
		char low = *pInput; //取出unicode字符的低8位
		pInput++;
		char high = *pInput; //取出unicode字符的高8位
		int w = high << 8;
		unsigned wchar = (high << 8) + low; //高8位和低8位组成一个unicode字符,加法运算级别高
		if (wchar <= 0x7F) { //英文字符
			pOutput[len] = (char) wchar;  //取wchar的低8位
			len++;
		} else if (wchar >= 0x80 && wchar <= 0x7FF) { //可以转换成双字节pOutput字符
			pOutput[len] = 0xc0 | ((wchar >> 6) & 0x1f); //取出unicode编码低6位后的5位，填充到110yyyyy 10zzzzzz 的yyyyy中
			len++;
			pOutput[len] = 0x80 | (wchar & 0x3f); //取出unicode编码的低6位，填充到110yyyyy 10zzzzzz 的zzzzzz中
			len++;
		} else if (wchar >= 0x800 && wchar < 0xFFFF) { //可以转换成3个字节的pOutput字符
			pOutput[len] = 0xe0 | ((wchar >> 12) & 0x0f); //高四位填入1110xxxx 10yyyyyy 10zzzzzz中的xxxx
			len++;
			pOutput[len] = 0x80 | ((wchar >> 6) & 0x3f); //中间6位填入1110xxxx 10yyyyyy 10zzzzzz中的yyyyyy
			len++;
			pOutput[len] = 0x80 | (wchar & 0x3f); //低6位填入1110xxxx 10yyyyyy 10zzzzzz中的zzzzzz
			len++;
		} else { //对于其他字节数的unicode字符不进行处理
			return -1;
		}
		pInput++; //处理下一个unicode字符
	}
	//utf8字符串后面，有个\0
	pOutput[len] = 0;
	return len;
}

bool check_ascii(const char *str, size_t length) {
	size_t i = 0;
	unsigned char ch = 0;
	while (i < length) {
		ch = *(str + i);
		//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
		if ((ch & 0x80) != 0)
			return false;
		i++;
	}
	return true;
}

//
bool check_utf8(const char *str, size_t length) {
	size_t i = 0;
	int nBytes = 0; //UTF8可用1 - 6个字节编码, ASCII用一个字节
	unsigned char ch = 0;
	//bool bAllAscii = true; //如果全部都是ASCII,说明不是UTF-8
	while (i < length) {
		ch = *(str + i);
		//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
		//if ((ch & 0x80) != 0)
		//	bAllAscii = false;
		if (nBytes == 0) { //计算字节数
			if ((ch & 0x80) != 0) {
				while ((ch & 0x80) != 0) {
					// i.e., 1110xxxx: <<1, 110xxxxx直到10xxxxxx
					//       11110xxx: <<1, 1110xxxx直到10xxxxxx
					ch <<= 1;
					nBytes++;
				}
				// nBytes是一个2到6之间的数
				if ((nBytes < 2) || (nBytes > 6)) {
					//第一个字节最少为110x xxxx，也就是至少移动2次（110xxxxx），
					// 最多移动6次（1111110x）
					return false;
				}
				nBytes--;					//减去自身占的一个字节
			}
		} else {					//多字节除了首字节外
			//多字节符的非首字节,应为 10xxxxxx
			if ((ch & 0xc0) != 0x80) {
				//剩下的字节都是10xx xxxx的形式
				return false;
			}
			//减到为零为止
			nBytes--;
		}
		i++;
	}
	if (nBytes != 0) {   //违返规则
		return false;
	}
	//if (bAllAscii) //如果全部都是ASCII, 也是UTF8，因此去掉该语句。
	//	return false;
	return (nBytes == 0);
}

// 只查询存粹的GBK
bool check_gbk(const char *str, size_t length) {
	size_t i = 0;
	int nBytes = 0;
	unsigned char ch = 0;
	unsigned char ch1 = 0;
	bool bAllAscii = true;
	while (i < length) {
		ch = *(str + i);
		ch1 = *(str + i + 1);
		//编码0~127,只有一个字节的编码，兼容ASCII码
		if (ch <= 0x7f) {
			i++;
			continue;
		} else {
			//大于127的使用双字节编码，落在gbk编码范围内的字符
			if (ch >= 0x81 && ch <= 0xfe && ch1 >= 0x40 && ch1 <= 0xfe
					&& ch1 != 0xf7) {
				i += 2;
				continue;
			} else {
				return false;
			}
		}
	}
	return true;
}

int is_str_utf8(const char *str) {
	unsigned int nBytes = 0; //UFT8可用1-6个字节编码,ASCII用一个字节
	unsigned char chr = *str;
	int bAllAscii = 1;
	for (unsigned int i = 0; str[i] != '\0'; ++i) {
		chr = *(str + i);
		//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
		if (nBytes == 0 && (chr & 0x80) != 0) {
			bAllAscii = 0;
		}
		if (nBytes == 0) {
			//如果不是ASCII码,应该是多字节符,计算字节数
			if (chr >= 0x80) {
				if (chr >= 0xFC && chr <= 0xFD) {
					nBytes = 6;
				} else if (chr >= 0xF8) {
					nBytes = 5;
				} else if (chr >= 0xF0) {
					nBytes = 4;
				} else if (chr >= 0xE0) {
					nBytes = 3;
				} else if (chr >= 0xC0) {
					nBytes = 2;
				} else {
					return 0;
				}
				nBytes--;
			}
		} else {
			//多字节符的非首字节,应为 10xxxxxx
			if ((chr & 0xC0) != 0x80) {
				return 0;
			}
			//减到为零为止
			nBytes--;
		}
	}
	//违返UTF8编码规则
	if (nBytes != 0) {
		return 0;
	}
	if (bAllAscii) { //如果全部都是ASCII, 也是UTF8
		return 1;
	}
	return 1;
}

long long insertLabelToken(long long ta_id, char *label, FILE *lbl_tkn_id_fp,
		FILE *lbl_tkn_fp) {
	lbl_tkn_record_bytes = 10 * LONG_LONG + 5;
	lbl_tkn_page_records = 10LL;
	lbl_tkn_page_bytes = lbl_tkn_record_bytes * lbl_tkn_page_records;
	long long startbyte = 0LL; // start points in database
	long long id = NULL_POINTER;
	if (lbl_tkn_pages != NULL) {
		lbl_tkn_t *tkn = (lbl_tkn_t*) malloc(sizeof(lbl_tkn_t));
		tkn->blkContent = label;
		tkn->id = -2;
		tkn->inUse = 0;
		tkn->len = strlen(label);
		tkn->page = NULL;
		tkn->taId = 0;
		tkn->nxtBlkId = -2;

	}
}

