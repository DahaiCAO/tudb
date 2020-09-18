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

/*
 * lbltknstore.c
 *
 * Created on: 2020年8月13日
 * Author: Dahai CAO
 */

/* read one label token page
 * start the start pointer in memory
 * start_no the start record id in this page
 * lbl_tkn_db_fp label token DB file
 */
static lbl_tkn_page_t* readOneLabelTokenPage(lbl_tkn_page_t *pages, long long start,
		long long start_no, FILE *lbl_tkn_db_fp) {
	unsigned char *page = (unsigned char*) malloc(
			sizeof(unsigned char) * lbl_tkn_page_bytes);
	memset(page, 0, sizeof(unsigned char) * lbl_tkn_page_bytes);
	fseek(lbl_tkn_db_fp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(lbl_tkn_db_fp)) != EOF) {
		fseek(lbl_tkn_db_fp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), lbl_tkn_page_bytes, lbl_tkn_db_fp); // read one page
	}

	lbl_tkn_page_t *p = (lbl_tkn_page_t*) malloc(sizeof(lbl_tkn_page_t));
	p->dirty = 0;
	p->expiretime = LABEL_TOKEN_PAGE_EXPIRE_TIME; // 10 minutes
	p->startNo = start_no;
	p->hit = 0;
	p->nxtpage = NULL;
	p->prvpage = NULL;
	p->content = page;
	p->start = start;
	p->end = start + lbl_tkn_page_bytes;

	lbl_tkn_page_t *pp = pages;
	if (pp != NULL) {
		while (pp->nxtpage != NULL) {
			pp = pp->nxtpage;
		}
		pp->nxtpage = p;
		p->prvpage = pp;
		pp = NULL;
	} else {
		lbl_tkn_pages = p;
	}
	return p;
}

void initLabelTokenDBMemPages(lbl_tkn_page_t *pages, FILE *lbl_tkn_db_fp) {
	// read the first page from 0 pointer.
	readOneLabelTokenPage(pages, 0LL, 0LL, lbl_tkn_db_fp);
}

//int UnicodeToUtf8(char *pInput, char *pOutput) {
//	int len = 0; //记录转换后的Utf8字符串的字节数
//	while (*pInput) { //处理一个unicode字符
//		char low = *pInput; //取出unicode字符的低8位
//		pInput++;
//		char high = *pInput; //取出unicode字符的高8位
//		int w = high << 8;
//		unsigned wchar = (high << 8) + low; //高8位和低8位组成一个unicode字符,加法运算级别高
//		if (wchar <= 0x7F) { //英文字符
//			pOutput[len] = (char) wchar;  //取wchar的低8位
//			len++;
//		} else if (wchar >= 0x80 && wchar <= 0x7FF) { //可以转换成双字节pOutput字符
//			pOutput[len] = 0xc0 | ((wchar >> 6) & 0x1f); //取出unicode编码低6位后的5位，填充到110yyyyy 10zzzzzz 的yyyyy中
//			len++;
//			pOutput[len] = 0x80 | (wchar & 0x3f); //取出unicode编码的低6位，填充到110yyyyy 10zzzzzz 的zzzzzz中
//			len++;
//		} else if (wchar >= 0x800 && wchar < 0xFFFF) { //可以转换成3个字节的pOutput字符
//			pOutput[len] = 0xe0 | ((wchar >> 12) & 0x0f); //高四位填入1110xxxx 10yyyyyy 10zzzzzz中的xxxx
//			len++;
//			pOutput[len] = 0x80 | ((wchar >> 6) & 0x3f); //中间6位填入1110xxxx 10yyyyyy 10zzzzzz中的yyyyyy
//			len++;
//			pOutput[len] = 0x80 | (wchar & 0x3f); //低6位填入1110xxxx 10yyyyyy 10zzzzzz中的zzzzzz
//			len++;
//		} else { //对于其他字节数的unicode字符不进行处理
//			return -1;
//		}
//		pInput++; //处理下一个unicode字符
//	}
//	//utf8字符串后面，有个\0
//	pOutput[len] = 0;
//	return len;
//}

/*************************************************************************************************
 * 将UTF8编码转换成Unicode（UCS-2LE）编码  低地址存低位字节
 * 参数：
 *    char* pInput   输入字符串
 *    char*pOutput   输出字符串
 * 返回值：转换后的Unicode字符串的字节数，如果出错则返回-1
 **************************************************************************************************/
//utf8转unicode
//int Utf8ToUnicode(char *pInput, char *pOutput) {
//	int outputSize = 0; //记录转换后的Unicode字符串的字节数
//	while (*pInput) {
//		if (*pInput > 0x00 && *pInput <= 0x7F) { //处理单字节UTF8字符（英文字母、数字）
//			*pOutput = *pInput;
//			pOutput++;
//			*pOutput = 0; //小端法表示，在高地址填补0
//		} else if (((*pInput) & 0xE0) == 0xC0) { //处理双字节UTF8字符
//			char high = *pInput;
//			pInput++;
//			char low = *pInput;
//			if ((low & 0xC0) != 0x80) { //检查是否为合法的UTF8字符表示
//				return -1; //如果不是则报错
//			}
//			*pOutput = (high << 6) + (low & 0x3F);
//			pOutput++;
//			*pOutput = (high >> 2) & 0x07;
//		} else if (((*pInput) & 0xF0) == 0xE0) { //处理三字节UTF8字符
//			char high = *pInput;
//			pInput++;
//			char middle = *pInput;
//			pInput++;
//			char low = *pInput;
//			if (((middle & 0xC0) != 0x80) || ((low & 0xC0) != 0x80)) {
//				return -1;
//			}
//			*pOutput = (middle << 6) + (low & 0x3F); //取出middle的低两位与low的低6位，组合成unicode字符的低8位
//			pOutput++;
//			*pOutput = (high << 4) + ((middle >> 2) & 0x0F); //取出high的低四位与middle的中间四位，组合成unicode字符的高8位
//		} else { //对于其他字节数的UTF8字符不进行处理
//			return -1;
//		}
//		pInput++; //处理下一个utf8字符
//		pOutput++;
//		outputSize += 2;
//	}
//	//unicode字符串后面，有两个\0
//	*pOutput = 0;
//	pOutput++;
//	*pOutput = 0;
//	return outputSize;
//}

//bool check_ascii(const char *str, size_t length) {
//	size_t i = 0;
//	unsigned char ch = 0;
//	while (i < length) {
//		ch = *(str + i);
//		//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
//		if ((ch & 0x80) != 0)
//			return false;
//		i++;
//	}
//	return true;
//}

// check UTF-8 and ASCII charset
bool check_utf8(unsigned char *str, size_t length) {
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

bool check_gb2312(unsigned char *str, size_t length) {
	return true;
}

bool check_gbk(unsigned char *str, size_t length) {
	size_t i = 0;
	unsigned char ch = 0;
	unsigned char ch1 = 0;
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

//int is_str_utf8(const char *str) {
//	unsigned int nBytes = 0; //UFT8可用1-6个字节编码,ASCII用一个字节
//	unsigned char chr = *str;
//	int bAllAscii = 1;
//	for (unsigned int i = 0; str[i] != '\0'; ++i) {
//		chr = *(str + i);
//		//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
//		if (nBytes == 0 && (chr & 0x80) != 0) {
//			bAllAscii = 0;
//		}
//		if (nBytes == 0) {
//			//如果不是ASCII码,应该是多字节符,计算字节数
//			if (chr >= 0x80) {
//				if (chr >= 0xFC && chr <= 0xFD) {
//					nBytes = 6;
//				} else if (chr >= 0xF8) {
//					nBytes = 5;
//				} else if (chr >= 0xF0) {
//					nBytes = 4;
//				} else if (chr >= 0xE0) {
//					nBytes = 3;
//				} else if (chr >= 0xC0) {
//					nBytes = 2;
//				} else {
//					return 0;
//				}
//				nBytes--;
//			}
//		} else {
//			//多字节符的非首字节,应为 10xxxxxx
//			if ((chr & 0xC0) != 0x80) {
//				return 0;
//			}
//			//减到为零为止
//			nBytes--;
//		}
//	}
//	//违返UTF8编码规则
//	if (nBytes != 0) {
//		return 0;
//	}
//	if (bAllAscii) { //如果全部都是ASCII, 也是UTF8
//		return 1;
//	}
//	return 1;
//}

int code_convert(char *from_charset, char *to_charset, char *inbuf,
		size_t inlen, char *outbuf, size_t outlen) {
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
		return -1;
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1) {
		pin = NULL;
		pout = NULL;
		return -1;
	}
	iconv_close(cd);
	*pout = '\0';
	pin = NULL;
	pout = NULL;
	return 0;
}

int u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
	//return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
	return code_convert("utf-8", "gbk", inbuf, inlen, outbuf, outlen);
}

int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
	//return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
	return code_convert("gbk", "utf-8", inbuf, inlen, outbuf, outlen);
}

void convert2Utf8(char *fromstr, char *tostr, size_t length) {
	if (!check_utf8(fromstr, strlen((const char*) fromstr))) {
		if (check_gbk(fromstr, strlen((const char*) fromstr))
				|| check_gb2312(fromstr, strlen((const char*) fromstr))) {
			g2u(fromstr, strlen(fromstr), tostr, sizeof(tostr));
		}
	} else {
		//tostr = &fromstr;
		memcpy(tostr, fromstr, length);
	}
}

/**
 * Local variables are automatically freed when the function ends,
 * you don't need to free them by yourself. You only free dynamically
 * allocated memory (e.g using malloc) as it's allocated on the heap:
 * char *arr = malloc(3 * sizeof(char));
 * strcpy(arr, "bo");
 * // ...
 * free(arr);
 * refer to: https://en.wikipedia.org/wiki/C_dynamic_memory_allocation
 */
void commitLabelToken(long long ta_id, lbl_tkn_t **list, FILE *lbl_tkn_db_fp,
		FILE *lbl_tkn_id_fp) {
	lbl_tkn_t **t = list;
	// assign a ID to every token block
	int j = 0;
	t = list;
	while (*(t + j) != NULL) {
		(*(t + j))->id = getOneId(lbl_tkn_id_fp, caches->lbltknIds,
				LABEL_ID_QUEUE_LENGTH);
		if (j > 0) {
			(*(t + j - 1))->nxtBlkId = (*(t + j))->id;
		}
		j++;
	}
	t = list;
	j = 0;
	// one by one to store label tokens
	while (*(t + j) != NULL) {
		// search a label token space for storing label token
		lbl_tkn_page_t *ps = lbl_tkn_pages;
		bool found = false;
		unsigned char *pos;
		while (!found) {
			while (ps != NULL) {
				if (ps->startNo <= (*(t + j))->id
						&& (*(t + j))->id
								< ps->startNo + LABEL_TOKEN_PAGE_RECORDS) {
					pos = ps->content
							+ ((*(t + j))->id - ps->startNo)
									* lbl_tkn_record_bytes;
					// convert label token block to byte array
					unsigned char ta_ids[LONG_LONG] = { 0 };
					LongToByteArray(ta_id, ta_ids);			// ta Id
					unsigned char length[LONG] = { 0 };
					Integer2Bytes((*(t + j))->len, length);
					unsigned char nblockId[LONG_LONG] = { 0 };
					LongToByteArray((*(t + j))->nxtBlkId, nblockId);// next block Id
					// store label token block
					memcpy(pos, ta_ids, LONG_LONG);
					unsigned char inuse[1] = { (*(t + j))->inUse };
					memcpy(pos + LONG_LONG, inuse, 1LL);
					memcpy(pos + LONG_LONG + 1, length, LONG);
					memcpy(pos + LONG_LONG + 1 + LONG, nblockId,
					LONG_LONG);
					memcpy(pos + LONG_LONG + 1 + LONG + LONG_LONG,
							(*(t + j))->blkContent, (*(t + j))->len);
					// update to DB
					fseek(lbl_tkn_db_fp, (*(t + j))->id * lbl_tkn_record_bytes,
					SEEK_SET); //
					fwrite(pos, sizeof(unsigned char), lbl_tkn_record_bytes,
							lbl_tkn_db_fp);
					found = true;
					pos = NULL;
					break;
				}
				ps = ps->nxtpage;
			}
			if (!found) {
				// read a new page
				long long pagenum = ((*(t + j))->id * lbl_tkn_record_bytes)
						/ lbl_tkn_page_bytes;
				readOneLabelTokenPage(lbl_tkn_pages,
						pagenum * lbl_tkn_page_bytes,
						pagenum * LABEL_TOKEN_PAGE_RECORDS, lbl_tkn_db_fp);
				continue;
			} else {
				ps = NULL;
				break;
			}
		}
		j++;
	}
	t = NULL;
}

lbl_tkn_t** divideLabelTokens(unsigned char *label) {
	lbl_tkn_t **list;
	unsigned char *tmpbuf = (unsigned char*) calloc(LABEL_BUFFER_LENGTH,
			sizeof(unsigned char));
	convert2Utf8((char*) label, (char*) tmpbuf, LABEL_BUFFER_LENGTH);
	size_t l = strlen((const char*) tmpbuf);
	if (l > LABEL_BLOCK_LENGTH) {
		size_t s = l / LABEL_BLOCK_LENGTH;
		size_t y = l % LABEL_BLOCK_LENGTH;
		lbl_tkn_t **p;
		if (y > 0) {
			list = (lbl_tkn_t**) calloc(s + 2, sizeof(lbl_tkn_t*));// one more for 0x0
		} else {
			list = (lbl_tkn_t**) calloc(s + 1, sizeof(lbl_tkn_t*));// one more for 0x0
		}
		p = list;
		for (int i = 0; i < s; i++) {
			unsigned char *buf = (unsigned char*) calloc(LABEL_BLOCK_LENGTH,
					sizeof(unsigned char));
			memcpy(buf, tmpbuf + i * LABEL_BLOCK_LENGTH, LABEL_BLOCK_LENGTH);
			lbl_tkn_t *tkn = (lbl_tkn_t*) malloc(sizeof(lbl_tkn_t));
			tkn->blkContent = buf;
			tkn->id = NULL_POINTER;
			tkn->inUse = 1;
			tkn->len = LABEL_BLOCK_LENGTH;
			tkn->page = NULL;
			tkn->taId = 0;
			tkn->nxtBlkId = NULL_POINTER;
			*p = tkn;
			p = p + 1;
			buf = NULL;
			tkn = NULL;
		}
		if (y > 0) {
			unsigned char *buf = (unsigned char*) calloc(LABEL_BLOCK_LENGTH,
					sizeof(unsigned char));
			memcpy(buf, tmpbuf + s * LABEL_BLOCK_LENGTH, y);
			lbl_tkn_t *tkn = (lbl_tkn_t*) malloc(sizeof(lbl_tkn_t));
			tkn->blkContent = buf;
			tkn->id = NULL_POINTER;
			tkn->inUse = 1;
			tkn->len = y;
			tkn->page = NULL;
			tkn->taId = 0;
			tkn->nxtBlkId = NULL_POINTER;
			*p = tkn;
			buf = NULL;
			tkn = NULL;
		}
		p = NULL;
	} else {
		list = (lbl_tkn_t**) calloc(1, sizeof(lbl_tkn_t));
		lbl_tkn_t *tkn = (lbl_tkn_t*) malloc(sizeof(lbl_tkn_t));
		tkn->blkContent = tmpbuf;
		tkn->id = NULL_POINTER;
		tkn->inUse = 0x1;
		tkn->len = strlen((const char*) tmpbuf);
		tkn->page = NULL;
		tkn->taId = 0;
		tkn->nxtBlkId = NULL_POINTER;
		*list = tkn;
		tkn = NULL;
	}
	tmpbuf = NULL;
	return list;
}

lbl_tkn_t ** searchLabelTokenList(long long id, FILE *lbl_tkn_db_fp) {
	lbl_tkn_t **list = NULL;
	lbl_tkn_page_t *ps = lbl_tkn_pages;
	int i = 1;
	unsigned char *pos;
	long long tId = id;
	while (tId != NULL_POINTER) {
		while (ps != NULL) {
			if (ps->startNo <= tId
					&& tId < ps->startNo + LABEL_TOKEN_PAGE_RECORDS) {
				pos = ps->content + (tId - ps->startNo) * lbl_tkn_record_bytes;
				lbl_tkn_t *tkn = (lbl_tkn_t*) malloc(sizeof(lbl_tkn_t));
				unsigned char *buf = (unsigned char*) calloc(
						lbl_tkn_record_bytes, sizeof(unsigned char));
				memcpy(buf, pos, lbl_tkn_record_bytes);
				tkn->page = ps;
				tkn->id = tId;
				unsigned char ta_ids[LONG_LONG] = { 0 };
				memcpy(ta_ids, buf, LONG_LONG);
				tkn->taId = ByteArrayToLong(ta_ids); // ta id

				tkn->inUse = *(buf + LONG_LONG); // in use

				unsigned char length[LONG] = { 0 };
				memcpy(length, buf + LONG_LONG + 1, LONG);
				tkn->len = Bytes2Integer(length); // block length

				unsigned char nblockId[LONG_LONG] = { 0 };
				memcpy(nblockId, (buf + LONG_LONG + 1 + LONG), LONG_LONG);
				tkn->nxtBlkId = ByteArrayToLong(nblockId); // next block id

				unsigned char *blockContent = (unsigned char*) calloc(
						LABEL_BLOCK_LENGTH, sizeof(unsigned char)); // block content
				memcpy(blockContent, buf + LONG_LONG + 1 + LONG + LONG_LONG,
						tkn->len);
				tkn->blkContent = blockContent;
				list = (lbl_tkn_t **)realloc(list, sizeof(lbl_tkn_t*) * i);
				*(list + i - 1) = tkn;
				free(buf);
				buf = NULL;
				i++;
				tId = tkn->nxtBlkId;
				tkn = NULL;
				blockContent = NULL;
				if (tId == NULL_POINTER) {
					break;
				} else {
					continue;
				}
			}
			ps = ps->nxtpage;
		}
		if (tId != NULL_POINTER) {
			// read a new page
			long long pagenum = (tId * lbl_tkn_record_bytes)
					/ lbl_tkn_page_bytes;
			readOneLabelTokenPage(lbl_tkn_pages, pagenum * lbl_tkn_page_bytes,
					pagenum * LABEL_TOKEN_PAGE_RECORDS, lbl_tkn_db_fp);
			continue;
		} else {
			break;
		}
	}
	list = (lbl_tkn_t **)realloc(list, sizeof(lbl_tkn_t*) * i);
	*(list + i - 1) = 0x0;// add one zero after the last element.
	ps = NULL;
	pos = NULL;
	return list;
}

unsigned char* findLabelToken(long long id, FILE *lbl_tkn_db_fp) {
	lbl_tkn_t **list = searchLabelTokenList(id, lbl_tkn_db_fp);
	// combine the label blocks to one label string.
	int j = 0;
	lbl_tkn_t **p = list;
	int l = 0;
	while (*(p + j)) { // calculate total label string length
		l = l + (*(p + j))->len;
		j++;
	}
	unsigned char *ct = (unsigned char*) calloc(l + 1, sizeof(unsigned char));
	int k = 0;
	j = 0;
	p = list;
	while (*(p + j)) { // put all block content into one buffer
		unsigned char *ch = (*(p + j))->blkContent;
		for (int i = 0; i < (*(p + j))->len; i++) {
			ct[k] = *(ch + i);
			k++;
		}
		j++;
	}
	p = NULL;
	deallocLabelTokenList(list);
	return ct;
}

void deleteLabelToken(long long id, FILE *lbl_tkn_db_fp) {
	lbl_tkn_t **list = searchLabelTokenList(id, lbl_tkn_db_fp);
	int j = 0;
	lbl_tkn_t **p = list;
	while (*(p + j)) {
		unsigned char *pos = ((*(p + j))->page)->content
				+ ((*(p + j))->id - ((*(p + j))->page)->startNo)
						* lbl_tkn_record_bytes;
		*(pos + LONG_LONG) = 0x0;
		recycleOneId((*(p + j))->id, caches->lbltknIds);
		// update to DB
		fseek(lbl_tkn_db_fp, (*(p + j))->id * lbl_tkn_record_bytes + LONG_LONG,
				SEEK_SET);
		unsigned char inuse[1] = { 0x0 };
		fwrite(inuse, sizeof(unsigned char), 1, lbl_tkn_db_fp);
		pos = NULL;
		j++;
	}
	deallocLabelTokenList(list);
	p = NULL;
	list = NULL;
}

// this update operation is used to update and override new label to old label
void commitUpdateLabelToken(lbl_tkn_t **list, lbl_tkn_t **newlist,
		FILE *lbl_tkn_id_fp, FILE *lbl_tkn_db_fp) {
	// c is old list length
	// j is new list length
	lbl_tkn_t **l = list;
	lbl_tkn_t **t = newlist;
	int c = 0;
	int j = 0;
	// get the new list length;
	while (*(t + j) != NULL) {
		j++;
	}
	while (*(l + c) != NULL) {
		c++;
	}
	//printf("%d\n", j);
	int k = 0;
	if (c < j) { // the new list is longer than the old list
		while (*(t + k) != NULL) {
			if (k < c) {
				(*(t + k))->id = (*(l + k))->id;
				(*(t + k))->taId = (*(l + k))->taId;
				(*(t + k))->nxtBlkId = (*(l + k))->nxtBlkId;
			} else {
				(*(t + k))->id = getOneId(lbl_tkn_id_fp, caches->lbltknIds,
						LABEL_ID_QUEUE_LENGTH);
				if (k >= c) {
					(*(t + k - 1))->nxtBlkId = (*(t + k))->id;
				}
			}
			k++;
		}
	} else if (c > j) { // the new list is shorter than the old list
		while (k < c) {
			if (*(t + k) != NULL) {
				(*(t + k))->id = (*(l + k))->id;
				(*(t + k))->taId = (*(l + k))->taId;
				(*(t + k))->nxtBlkId = (*(l + k))->nxtBlkId;
			} else {
				(*(l + k))->inUse = 0x0;
				if (k == j) {
					(*(t + k - 1))->nxtBlkId = NULL_POINTER;
				}
			}
			k++;
		}
	} else if (c == j) { // the new list is equal to old list
		while (*(t + k) != NULL) {
			if (*(t + k) != NULL) {
				(*(t + k))->id = (*(l + k))->id;
				(*(t + k))->taId = (*(l + k))->taId;
				(*(t + k))->nxtBlkId = (*(l + k))->nxtBlkId;
			}
			k++;
		}
	}
	// store new list to DB
	k = 0;
	// one by one to store label tokens
	while (*(t + k) != NULL) {
		// search a label token space for storing label token
		lbl_tkn_page_t *ps = lbl_tkn_pages;
		bool found = false;
		unsigned char *pos;
		while (!found) {
			while (ps != NULL) {
				if (ps->startNo <= (*(t + k))->id
						&& (*(t + k))->id
								< ps->startNo + LABEL_TOKEN_PAGE_RECORDS) {
					pos = ps->content
							+ ((*(t + k))->id - ps->startNo)
									* lbl_tkn_record_bytes;
					unsigned char ta_ids[LONG_LONG] = { 0 };
					LongToByteArray((*(t + k))->taId, ta_ids);			// ta Id
					memcpy(pos, ta_ids, LONG_LONG);

					unsigned char inuse[1] = { (*(t + k))->inUse };
					memcpy(pos + LONG_LONG, inuse, 1);		// in use flag

					unsigned char length[LONG] = { 0 };
					Integer2Bytes((*(t + k))->len, length);
					memcpy(pos + LONG_LONG + 1, length, LONG); // block length

					unsigned char nblockId[LONG_LONG] = { 0 };
					LongToByteArray((*(t + k))->nxtBlkId, nblockId); // next block Id
					memcpy(pos + LONG_LONG + 1 + LONG, nblockId,
					LONG_LONG);

					memcpy(pos + LONG_LONG + 1 + LONG + LONG_LONG,
							(*(t + k))->blkContent, (*(t + k))->len); // block content
					// update to DB
					fseek(lbl_tkn_db_fp, (*(t + k))->id * lbl_tkn_record_bytes,
					SEEK_SET); //
					fwrite(pos, sizeof(unsigned char), lbl_tkn_record_bytes,
							lbl_tkn_db_fp);
					pos = NULL;
					found = true;
					break;
				}
				ps = ps->nxtpage;
			}
			if (!found) {
				// read a new page
				long long pagenum = ((*(t + k))->id * lbl_tkn_record_bytes)
						/ lbl_tkn_page_bytes;
				readOneLabelTokenPage(lbl_tkn_pages,
						pagenum * lbl_tkn_page_bytes,
						pagenum * LABEL_TOKEN_PAGE_RECORDS, lbl_tkn_db_fp);
				continue;
			} else {
				ps = NULL;
				break;
			}
		}
		k++;
	}
	if (c > j) { // the new list is shorter than the old list
		while (k < c) { // each element in list can be found in memory pages.
			unsigned char *pos = ((*(l + k))->page)->content
					+ ((*(l + k))->id - ((*(l + k))->page)->startNo)
							* lbl_tkn_record_bytes;
			*(pos + LONG_LONG) = 0x0;
			recycleOneId((*(l + k))->id, caches->lbltknIds);
			// update to DB
			fseek(lbl_tkn_db_fp,
					(*(l + k))->id * lbl_tkn_record_bytes + LONG_LONG,
					SEEK_SET);
			unsigned char inuse[1] = { 0x0 };
			fwrite(inuse, sizeof(unsigned char), 1, lbl_tkn_db_fp);
			pos = NULL;
			k++;
		}
	}
	l = NULL;
	t = NULL;
}

void combineLabelTokens(lbl_tkn_t **list, int length) {
	int j = 0;
	lbl_tkn_t ** p = list;
	unsigned char *ct = (unsigned char*) calloc(length + 1,
			sizeof(unsigned char));
	int k = 0;
	while (*(p + j)) {
		unsigned char *ch = (*(p + j))->blkContent;
		for (int i = 0; i < (*(p + j))->len; i++) {
			ct[k] = *(ch + i);
			k++;
		}
		j++;
	}
	printf("%s\n", ct);
	free(ct);
	ct = NULL;
	p = NULL;
}

// deallocate label token list
void deallocLabelTokenList(lbl_tkn_t **list) {
	int j = 0;
	while (*(list + j)) {
		free((void *)(*(list + j))->blkContent);
		(*(list + j))->blkContent = NULL;
		(*(list + j))->page = NULL;
		free(*(list + j));
		j++;
	}
	free(list);
	list = NULL;
}

// deallocate label token memory pages
void deallocLabelTokenPages(lbl_tkn_page_t *pages) {
	lbl_tkn_page_t *p;
	while (pages) {
		p = pages;
		pages = pages->nxtpage;
		free(p->content);
		p->prvpage = NULL;
		p->nxtpage = NULL;
		free(p);
		p = NULL;
	}
	pages = NULL;
}
