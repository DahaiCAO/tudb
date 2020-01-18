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
#include<setjmp.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "test.h"

/**
 * this function convert the hex decimal string to decimal integer.
 * The format of parameter is '0000000c' convert to 12,
 * '0000000e' -> 14. But, the convert may have some issues for the big number.
 * You need to test this algorithm.
 *
 * @author Dahai Cao created at 13:50 on 2020-01-18
 */
int htoi(char *s) {
	int n = 0;
	int i = 0;
	while (s[i] != '\0') {
		if (s[i] != '0') {
			if (s[i] >= '0' && s[i] <= '9') {
				n = n * 16 + (s[i] - '0');
			} else if (s[i] >= 'a' && s[i] <= 'f') {
				n = n * 16 + (s[i] - 'a') + 10;
			} else if (s[i] >= 'A' && s[i] <= 'F') {
				n = n * 16 + (s[i] - 'A') + 10;
			}
		}
		i++;
	}
	return n;
}

//void itoh(int integer) {
//	int c;
//	int i, j, digit;
//	for (i = 7, j = 0; i >= 0; i--, j++) {
//		digit = (integer >> (i * 4)) & 0xf;
//		if (digit < 10) {
//			c = digit + 0x30;
//		} else {
//			c = digit + 0x37;
//		}
//	}
//}

void hexconcat(char *buf, char *t) {
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

void contructLoginCmd(char *username, char *password, char *cmd) {
	strcpy(cmd, "0001");	// 0001: login command

	int usrlen = strlen(username);
	char mm[10] = { 0 };
	itoa(usrlen, mm, 10);

	int pwdlen = strlen(password);
	char nn[10] = { 0 };
	itoa(pwdlen, nn, 10);

	int lensum = usrlen + pwdlen + 4;

	itoa(lensum, nn, 16); // convert to hexdecimal number
	char h[8] = { 0 };
	hexconcat(nn, h);
	strcat(cmd, h);
	strcat(cmd, username);
	strcat(cmd, " ");
	strcat(cmd, password);
	printf("%s\n", cmd);

}

int main() {
//    int State = setjmp(mark);
//    if(State==0){
//        Div(4,0);
//    }else{
//        switch(State){
//            case 1:
//                printf("除0异常!\n");
//        }
//    }
//    return 0;

//	errno = 0;
//	if (NULL == fopen("d:\\tudata\\d.txt", "a+"))
//	{
//	printf("----Msg:%s\n", strerror(errno));
//	perror("----msg");
//	}
//	else
//	{
//	 printf("Msg:%s\n", strerror(errno));
//	 perror("msg");
//	}

	//setvbuf(stdout, NULL, _IOLBF, 0);
	//setvbuf(stdout, NULL, _IONBF, 0);

//    char age[5] = {0};
//    printf("Hello, please enter your age:\n");
//    fflush(stdout);
//    fflush(stdin);
//    rewind(stdin);
//    scanf("%1s", age);
//    printf("Your age is %s\n", age);
//    // elegant ?
//    fflush(stdout);
//    printf("%s\n", "The end.");
//    fflush(stdout);

//	char *c  = "lllll";
//    char mm[10];
//	int l = strlen(c);
//	char *ch = itoa(l, mm, 10);
//	printf("%s\n", ch);

	//char sendbuf[512] = { 0 }; //
	//contructLoginCmd("uuu", "ppp", sendbuf);

//	int iiii = 42;
//	char buf[8] = { 0 };
//	itoa(iiii, buf, 16);
//	//decimal_to_hexadecimal
//	printf("%s\n", buf);

//	int a = htoi("9FA8C");
//	printf("%d\n", a);
		int a = htoi("000fA1");
		printf("%d\n", a);

	//char str[255];
	//sprintf(str, "%x", 100); //将100转为16进制表示的字符串。

//			long iiii = 42949;
//			char buf[8] = { 0 };
//			ltoa(iiii, buf, 16);
//			//decimal_to_hexadecimal
//			printf("%s\n", buf);

	//char buf[512] = { 0 };
	//contructLoginCmd("root", "passwd", buf);

	return 0;

}
