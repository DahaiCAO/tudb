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

#include <unistd.h>

#include "test.h"

#include "btreestore.h"
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
	long long num = 0; //2004293008363;
	unsigned char buffer[8] = { 0 };
	LongToByteArray(num, buffer);

	//long long s = ByteArrayToLong(buffer);
	//printf("%s\n",s);*/
	// writing to id db
	fwrite(buffer, sizeof(unsigned char), 8, fp);
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
		printf("%lld\n", s0);
	}
	if ((access(timeaxis, F_OK)) != -1) {
		// "existed"
		dbfp = fopen(timeaxis, "ab+");
		long x = fread(buffer2, sizeof(unsigned char), 8, dbfp);
		printf("%ld\n", x);
		long long s1 = ByteArrayToLong(buffer2);
		printf("%lld\n", s1);
		// whence 可以是 SEEK_SET(0),SEEK_CUR(1),SEEK_END(2)
		// 这些值决定是从文件头、当前点和文件尾计算偏移量 offset。
		//fread(buffer2, sizeof(unsigned char), 8, dbfp);
		int y = fseek(dbfp, 8, SEEK_SET);
		printf("%d\n", y);
		long z = fread(buffer3, sizeof(unsigned char), 8, dbfp);
		printf("%ld\n", z);
		long long s2 = ByteArrayToLong(buffer3);
		printf("%lld\n", s2);
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
	createTimeAxisRecord(idfp, dbfp);
	fclose(idfp);
	fclose(dbfp);
}

long long getId(FILE *idfp) {
//	unsigned long long curtm = (unsigned long) time(NULL);
//	unsigned char buffer1[8] = { 0 };
	unsigned char ids[8] = { 0 };
	unsigned char zero[8] = { 0 };
	// onver
	//LongToByteArray(curtm, buffer1);
	// get one id from id DB, update new id to db
	fseek(idfp, 8, SEEK_SET);
	fread(ids, sizeof(unsigned char), 8, idfp);
	long long s0 = ByteArrayToLong(ids);
	if (s0 != 0) { // there is reused Id
		// get one reused Id
		fseek(idfp, s0, SEEK_SET);
		fread(ids, sizeof(unsigned char), 8, idfp);
		long long s0 = ByteArrayToLong(ids);
		// update the pointer
		fwrite(zero, sizeof(unsigned char), 8, idfp);
		fseek(idfp, 8, SEEK_SET);
		if (s0 > 1) {
			s0 = s0 - 1;
			LongToByteArray(s0, ids);
			fwrite(ids, sizeof(unsigned char), 8, idfp);
			return s0;
		} else {

		}
	} else { // no reused Id
		fseek(idfp, 0, SEEK_SET);
		fread(ids, sizeof(unsigned char), 8, idfp);
		long long newid = ByteArrayToLong(ids);
		return newid;
	}
	//long sz = ftell(idfp);
//	long long s1 = 0;
//	if (sz > 16) { // get the last recycled Id.
//		fseek(idfp, -8, SEEK_END); // move file pointer to file end
//		fread(ids, sizeof(unsigned char), 8, idfp); // get the last recycled Id
//		s1 = ByteArrayToLong(ids); // convert to long id
//		sz = sz - 8; // file length size
//		truncate(idfp, sz); // shrink file length(size)
//		//ftruncate(idfp, sz);
//	} else { // get the first Id
//		fread(ids, sizeof(unsigned char), 8, idfp);
//		s1 = ByteArrayToLong(ids);
//		long long s2 = s1 + 1;
//		unsigned char buf[8] = { 0 };
//		LongToByteArray(s2, buf);
//		fseek(idfp, 0, SEEK_SET);
//		fwrite(buf, sizeof(unsigned char), 8, idfp);
//	}
	// s1 is new Id for new record
	return 0;
}

long long recycleId(FILE *idfp, long long id) {

}

void createTimeAxisRecord(FILE *idfp, FILE *dbfp) {
	long long newid = getId(idfp);
	//fseek(dbfp, 0L, SEEK_END);
	//long sz1 = ftell(dbfp);
	// printf("nnnnn%ld\n", sz1);
	//

	// insert new record into time axis DB.

}

//#include "log.h"
//#include "confutil.h"

//int main(int argv, char **argc) {
//	setvbuf(stdout, NULL, _IONBF, 0);
//	//char send_dat1[] = "bye";
//	//createRequest(send_dat1);
//	//fileReadTest();
//	fileWrieTest();
//	/* gets time of day */
//	/*time_t now = time(NULL);
//	 char buf[25] = {0};
//	 struct tm *tblock;
//	 //tblock = localtime(&timer);
//	 //localtime() converts date/time to a structure
//	 strftime(buf,24,"%Y%m%d",localtime(&now));
//	 printf("%s\n",buf);
//
//	 strftime(buf,24,"%Y-%m-%d %H:%M:%S",localtime(&now));
//	 printf("%s\n",buf);
//
//	 strftime(buf,24,"%y%m%d %H:%M:%S",localtime(&now));
//	 printf("%s\n",buf);
//
//	 strftime(buf,24,"%y%m%d",localtime(&now));
//	 printf("%s\n",buf);
//
//	 strftime(buf,24,"%H:%M:%S",localtime(&now));
//	 printf("%s\n",buf);*/
//
//	/*	long long num = 2004293008363;
//	 unsigned char buffer[8] = {0};
//	 LongToByteArray(num, buffer);
//
//	 long long s = ByteArrayToLong(buffer);
//	 printf("%lld\n",s);*/
//
//	//printf("%lu\n", (unsigned long)time(NULL)); // For 64-bit systems
//	//printf("%u\n", (unsigned)time(NULL)); // For 32-bit systems
//}

// testing program for B tree
int main(int argv, char **argc) {
	setvbuf(stdout, NULL, _IONBF, 0);
	ta_btree_t **_btree = (ta_btree_t**) calloc(1, sizeof(ta_btree_t));
//	btree_create(_btree, 100);
//	btree_insert(*_btree, 29); //1
//	btree_insert(*_btree, 40); //2
//	btree_insert(*_btree, 22); //3
//	btree_insert(*_btree, 32); //3
//	btree_insert(*_btree, 59); //4
//	btree_insert(*_btree, 99); //5
//	btree_insert(*_btree, 72); //6
//	btree_insert(*_btree, 8); //7
//	btree_insert(*_btree, 37); //8
//	btree_insert(*_btree, 58); //9
//	btree_insert(*_btree, 78); //10
//	btree_insert(*_btree, 10); //11
//	btree_insert(*_btree, 20); //12
//	btree_insert(*_btree, 48); //13
//	btree_insert(*_btree, 43); //14
//	btree_insert(*_btree, 41); //15
//	btree_insert(*_btree, 28); //16
//	btree_insert(*_btree, 66); //17
//	btree_insert(*_btree, 97); //18
//	btree_insert(*_btree, 101); //19
//	btree_insert(*_btree, 18); //20

	//	// 11->12->6->5->13->7->3->4->2->1->9->8->10
	//		btree_create(_btree, 4);
	//		btree_insert(*_btree, 11); //1
	//		btree_insert(*_btree, 12); //2
	//		btree_insert(*_btree, 6); //3
	//		btree_insert(*_btree, 5); //3
	//		btree_insert(*_btree, 13); //4
	//		btree_insert(*_btree, 7); //5
	//		btree_insert(*_btree, 3); //6
	//		btree_insert(*_btree, 4); //7
	//		btree_insert(*_btree, 2); //8
	//		btree_insert(*_btree, 1); //9
	//		btree_insert(*_btree, 9); //10
	//		btree_insert(*_btree, 8); //11
	//		btree_insert(*_btree, 10); //12



	btree_create(_btree, 3);
	btree_insert(*_btree, 492); //1
	btree_insert(*_btree, 172); //2
	btree_insert(*_btree, 383); //3
	btree_insert(*_btree, 59); //4
	btree_insert(*_btree, 123); //5
	btree_insert(*_btree, 27); //6
	btree_insert(*_btree, 135); //7
	btree_insert(*_btree, 67); //8
	btree_insert(*_btree, 335); //9
	btree_insert(*_btree, 211); //10
	btree_insert(*_btree, 368); //11
	btree_insert(*_btree, 362); //12
	btree_insert(*_btree, 386); //13
	btree_insert(*_btree, 429); //14
	btree_insert(*_btree, 426); //15
	btree_insert(*_btree, 421); //16
	btree_insert(*_btree, 690); //17
	btree_insert(*_btree, 567); //18
	btree_insert(*_btree, 540); //19
	btree_insert(*_btree, 530); //20
	btree_insert(*_btree, 649); //21
	btree_insert(*_btree, 736); //22
	btree_insert(*_btree, 763); //23
	btree_insert(*_btree, 886); //24
	btree_insert(*_btree, 793); //25
	btree_insert(*_btree, 782); //26
	btree_insert(*_btree, 862); //27
	btree_insert(*_btree, 915); //28
	btree_insert(*_btree, 926); //29
	btree_insert(*_btree, 777); //30

	print_btree(_btree[0]);
}
