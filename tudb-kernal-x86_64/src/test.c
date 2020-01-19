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
 * test.c
 *
 * Created on: 2020年1月6日 上午7:18:15
 * Author: Dahai CAO 
 */

#include<stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "test.h"

// 在C语言中，数组作为参数，进行传递时，
// 传递的是指针 ，换句话说，字符数组作为参数，
// 进行传递时，传递的是字符数组的起始地址，
// 相当于一个字符指针，两者没有区别。比如下面的代码：
void f1(char *s) {
	printf("%s\n", s);
}
void f2(char sa[]) {
	printf("%s\n", sa);
}

void hexconcat(char *buf,  char *t) {
	if (strlen(buf) == 1) {
		strcat(t, "0000000");
	} else if (strlen(buf) == 2) {
		strcat(t, "000000");
	} else if (strlen(buf) == 3) {
		strcat(t, "00000");
	} else if (strlen(buf) == 4) {
		strcat(t, "0000");
	} else if (strlen(buf) == 5) {
		strcat(t, "000");
	} else if (strlen(buf) == 6) {
		strcat(t, "00");
	} else if (strlen(buf) == 7) {
		strcat(t, "0");
	}
	strcat(t, buf);
}


void createReponse(char *msg, char *msghead, char *msgbody) {
	strcpy(msg, msghead); // 4 Bytes length

	int rsplen = 12 + strlen(msgbody);
	char len[8] = { 0 };
	itoa(rsplen, len, 16);

	char h[8] = { 0 }; // 8 Bytes length for store the hex number
	hexconcat(len, h);
	strcat(msg, h);
	strcat(msg, msgbody);
	printf("%s\n", msg);
}

int main() {
	setvbuf(stdout, NULL, _IONBF, 0);
//	char rsa[] = "world hello";
//	f1(rsa); // 效果一样
//	f2(rsa); // 效果一样

	/*char c[8] = { 0 };
	 itoa(999, c, 10);// for windows platform
	 char msg1[] = "uuuuuuuuuuuuuuuu";
	 strcat(msg1, "jjj");
	 strcat(msg1, c);
	 printf("%s\n", msg1);*/
	// printf("EACCES:%s\n", strerror(EACCES));//打印没有权限错误信息
	// errno = EHOSTDOWN;//EHOSTDOWN 服务器关闭
	// perror("Error:");
//	char str[] = "我,是,中国,程序员";
//	char *ptr;
//	//char *p;
//	printf("开始前:  str=%s\n", str);
//	printf("开始分割:\n");
//	ptr = strtok(str, ",");
//	while (ptr != NULL) {
//		printf("ptr=%s\n", ptr);
//		ptr = strtok(NULL, ",");
//	}


	//int i;// 数组和指针的相互转化
	//char a[10];
	//char *b = "abcdefghi";
	//for (i = 0; i < 10; i++) {
	//	a[i] = b[i];
	//}
	//printf("%s", a);

	// 数组可以在栈上分配，也可以在堆上分配，但必须指定大小。
//	char str0[] = "root password"; //在栈上分配
	//char a1[100]; //在栈上分配
	// char* pa = new char[100];// 在堆上分配，返回首元素的地址
//	char *str = str0;	// 在堆上分配，返回首元素的地址
//	printf("ptr=%s\n", str);
//
//	char *ptr = strtok(str, " ");
//	printf("ptr=%s\n", ptr);
//	printf("str=%s\n", str);
//	char *ptr1 = strtok(NULL, " ");
//	printf("str=%s\n", ptr1);

//	char rsp[] = "1234567890_login_califnia_melbourne_sydney_queenland_china_airfare_arline_hot_fire";
//	char msg[1024];
//	createReponse(msg, "0003", rsp);

	   char src[40];
	   char dest[12];

	   memset(dest, '\0', sizeof(dest));
	   strcpy(src, "This is tutorialspoint.com");
	   strncpy(dest, src, 10);

	   printf("Final copied string : %s\n", dest);

	return 0;

}
