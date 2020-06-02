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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "test.h"
/*
 * test.c
 *
 * Created on: 2020-01-07 18:33:57
 * Author: Dahai CAO 
 */

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

void createRequest(char *sd_buf) {
	if (strcmp(sd_buf, "bye") == 0 || strcmp(sd_buf, "quit") == 0
			|| strcmp(sd_buf, "exit") == 0) { // 0003: logout
		char buf[12 + sizeof(sd_buf)];
		memset(buf, 0, sizeof(buf));
		strcat(buf, "0003");
		char nn[2] = { 0 };
		itoa(1, nn, 16); // convert to hexdecimal number
		char h[8] = { 0 }; // 8 Bytes length
		hexconcat(nn, h);
		strcat(buf, h);
		strcat(buf, "X");
		strcpy(sd_buf, buf);
	} else if (strcmp(sd_buf, "show ") == 0) { //
	}
}

void initIdDB1(FILE *fp) {
	// 16 byte
	long long num = 16; //2004293008363;
	unsigned char buffer[8] = { 0 };
	LongToByteArray(num, buffer);

	//long long s = ByteArrayToLong(buffer);
	//printf("%s\n",s);*/
	// writing to id db
	fwrite(buffer, sizeof(unsigned char), 8, fp);
}

void initIdDB2(FILE *fp) {
	// 16 byte
	long long num = -1;	//2004293008363;
	unsigned char buffer[8] = { 0 };
	LongToByteArray(num, buffer);

	//long long s = ByteArrayToLong(buffer);
	//printf("%s\n",s);*/
	// writing to id db
	fwrite(buffer, sizeof(unsigned char), 8, fp);
	// writing to id db
	fwrite(buffer, sizeof(unsigned char), 8, fp);

}

void fileReadTest() {
	FILE *idfp = NULL;
	FILE *dbfp = NULL;
	char *timeaxisid = "D:/tudata/tustore.timeaxis.db.id";
	char *timeaxis = "D:/tudata/tustore.timeaxis.db";

	unsigned char buffer1[8] = { 0 };
	unsigned char buffer2[8] = { 0 };
	unsigned char buffer3[8] = { 0 };
	if ((access(timeaxisid, F_OK)) != -1) {
		// "existed"
		idfp = fopen(timeaxisid, "ab+");
		fread(buffer1, sizeof(unsigned char), 8, idfp);
		long long s0 = ByteArrayToLong(buffer1);
		printf("%ld\n", s0);
	}
	if ((access(timeaxis, F_OK)) != -1) {
		// "existed"
		dbfp = fopen(timeaxis, "ab+");
		long x = fread(buffer2, sizeof(unsigned char), 8, dbfp);
		printf("%ld\n", x);
		long long s1 = ByteArrayToLong(buffer2);
		printf("%ld\n", s1);
		// whence 可以是 SEEK_SET(0),SEEK_CUR(1),SEEK_END(2)
		// 这些值决定是从文件头、当前点和文件尾计算偏移量 offset。
		//fread(buffer2, sizeof(unsigned char), 8, dbfp);
		int y = fseek(dbfp, 8, SEEK_SET);
		printf("%d\n", y);
		long z = fread(buffer3, sizeof(unsigned char), 8, dbfp);
		printf("%ld\n", z);
		long long s2 = ByteArrayToLong(buffer3);
		printf("%ld\n", s2);
	}
	fclose(idfp);
	fclose(dbfp);
}

void fileWrieTest() {
	FILE *idfp = NULL;
	FILE *dbfp = NULL;
	char *timeaxisid = "D:/tudata/tustore.timeaxis.db.id";
	char *timeaxis = "D:/tudata/tustore.timeaxis.db";
	if ((access(timeaxisid, F_OK)) != -1) {
		// "existed"
		idfp = fopen(timeaxisid, "ab+");

	} else {	// "not existed"
		idfp = fopen(timeaxisid, "wb+");
		initIdDB1(idfp);
	}
	if ((access(timeaxis, F_OK)) != -1) {
		// "existed"
		dbfp = fopen(timeaxis, "ab+");
	} else { // "not existed"
		dbfp = fopen(timeaxis, "wb+");
		initIdDB2(dbfp);
	}
	fclose(idfp);
	fclose(dbfp);
}

void createTimeAxisRecord(FILE *idfp, FILE *dbfp) {
	unsigned long long curtm = (unsigned long) time(NULL);
	// get one id from id db, update new id to db
	// insert new record into time axis db.

}

//#include "log.h"
//#include "confutil.h"

int main(int argv, char **argc) {
	setvbuf(stdout, NULL, _IONBF, 0);
	//char send_dat1[] = "bye";
	//createRequest(send_dat1);
	//fileReadTest();
	//fileWrieTest();
	/* gets time of day */
	/*time_t now = time(NULL);
	 char buf[25] = {0};
	 struct tm *tblock;
	 //tblock = localtime(&timer);
	 //localtime() converts date/time to a structure
	 strftime(buf,24,"%Y%m%d",localtime(&now));
	 printf("%s\n",buf);

	 strftime(buf,24,"%Y-%m-%d %H:%M:%S",localtime(&now));
	 printf("%s\n",buf);

	 strftime(buf,24,"%y%m%d %H:%M:%S",localtime(&now));
	 printf("%s\n",buf);

	 strftime(buf,24,"%y%m%d",localtime(&now));
	 printf("%s\n",buf);

	 strftime(buf,24,"%H:%M:%S",localtime(&now));
	 printf("%s\n",buf);*/

	/*	long long num = 2004293008363;
	 unsigned char buffer[8] = {0};
	 LongToByteArray(num, buffer);

	 long long s = ByteArrayToLong(buffer);
	 printf("%lld\n",s);*/

	//printf("%lu\n", (unsigned long)time(NULL)); // For 64-bit systems
	//printf("%u\n", (unsigned)time(NULL)); // For 32-bit systems
}
