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

#include "tastore.h"

const long LONG_SIZE = 8L; //

void convertEpToBytes(evolved_point_t *ep, unsigned char buf[]) {
	memset(buf, 0, 25);
	buf[0] = ep->inUse;
	unsigned char p[BUFFERSIZE] = { 0 };
	unsigned char n[BUFFERSIZE] = { 0 };
	unsigned char ts[BUFFERSIZE] = { 0 };
	LongToByteArray(ep->prvTsId, p);
	LongToByteArray(ep->nxtTsId, n);
	LongToByteArray(ep->time, ts);
	for (int i = 1; i <= 8; i++) {
		buf[i] = p[i];
	}
	for (int i = 9; i <= 16; i++) {
		buf[i] = n[i];
	}
	for (int i = 17; i <= 25; i++) {
		buf[i] = ts[i];
	}
}

void convertBytesToEp(evolved_point_t *ep, unsigned char buf[]) {
	ep->inUse = buf[0];
	unsigned char p[BUFFERSIZE] = { 0 };
	unsigned char n[BUFFERSIZE] = { 0 };
	unsigned char ts[BUFFERSIZE] = { 0 };
	for (int i = 1; i <= 8; i++) {
		p[i] = buf[i];
	}
	for (int i = 9; i <= 16; i++) {
		n[i] = buf[i];
	}
	for (int i = 17; i <= 25; i++) {
		ts[i] = buf[i];
	}
	ep->prvTsId = ByteArrayToLong(p);
	ep->nxtTsId = ByteArrayToLong(n);
	ep->time = ByteArrayToLong(ts);
}

/*
 * text.c
 *
 * Created on: 2020年6月8日
 * Author: Dahai CAO
 */


// initialize DB
void initializeDB() {
	taidfp = NULL;
	char *timeaxisid = "D:/tudata/tustore.timeaxis.tdb.id";
	if ((access(timeaxisid, F_OK)) == -1) {
		taidfp = fopen(timeaxisid, "wb+");
		// initializes Id DB
		unsigned char zero[BUFFERSIZE] = { 0L };
		fseek(taidfp, 0L, SEEK_SET); // move file pointer to file head
		fwrite(zero, sizeof(unsigned char), LONG_SIZE, taidfp);
		//fseek(taidfp, LONG_SIZE, SEEK_SET);  // move file pointer to 8th byte
		fwrite(zero, sizeof(unsigned char), LONG_SIZE, taidfp);
	}
}

void initializeTaDB() {
	tadbfp = NULL;
	char *taDb = "D:/tudata/tustore.timeaxis.tdb";
	if ((access(taDb, F_OK)) == -1) {
		tadbfp = fopen(taDb, "wb+");
		// initializes Id DB
		unsigned char n1[BUFFERSIZE] = { 0L };
		LongToByteArray(-1L, n1); // convert
		fseek(tadbfp, 0L, SEEK_SET); // move file pointer to file head
		fwrite(n1, sizeof(unsigned char), LONG_SIZE, tadbfp);
		//fseek(tadbfp, LONG_SIZE, SEEK_SET);  // move file pointer to 8th byte
		fwrite(n1, sizeof(unsigned char), LONG_SIZE, tadbfp);
	}
}


void createTimeAixs(long long ts, FILE *taidfp, FILE *tadbfp) {
	// get a new Id
	unsigned char page[PAGESIZE] = { 0L };
	unsigned char tsBytes[EPSIZE] = { 0L };
	unsigned char p[BUFFERSIZE * 2] = { 0 };
	unsigned char first[BUFFERSIZE] = { 0L };
	unsigned char last[BUFFERSIZE] = { 0L };
	LongToByteArray(ts, tsBytes); // convert time stamp to byte array
	fseek(tadbfp, 0L, SEEK_SET); //
	fread(p, sizeof(unsigned char), LONG_SIZE * 2, tadbfp);
	for (int i = 0; i < BUFFERSIZE; i++) {
		first[i] = p[i];
	}
	for (int i = BUFFERSIZE; i < 2 * BUFFERSIZE; i++) {
		last[i] = p[i];
	}
	long long firstId = ByteArrayToLong(first);
	long long lastId = ByteArrayToLong(last);
	if (firstId == -1 && lastId == -1) {
		// empty DB, insert it directly.
		long long id = getId(taidfp); // get new Id
		evolved_point_t *ep = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		ep->inUse = 1;
		ep->nxtTsId = -1;
		ep->prvTsId = -1;
		ep->time = ts; //(unsigned long)time(NULL);
		convertEpToBytes(ep, tsBytes);
		fseek(tadbfp, 0L, SEEK_SET); // move file pointer to file head
		unsigned char n1[BUFFERSIZE] = { 0L };
		LongToByteArray(0, n1); // convert
		fwrite(0, sizeof(unsigned char), LONG_SIZE, tadbfp);
		fwrite(0, sizeof(unsigned char), LONG_SIZE, tadbfp);
		fwrite(tsBytes, sizeof(unsigned char), EPSIZE, tadbfp);
		free(ep);
	} else {
		// read one page
	}

}

int main(int argv, char **argc) {
	setvbuf(stdout, NULL, _IONBF, 0);
	// initialize server when install server.
//	initializeDB();
//	initializeTaDB();

//	cache = (id_cache_t*) malloc(sizeof(id_cache_t));
//	for (int i = 0; i < 100; i++) {
//		cache->nId[i] = 0;
//		cache->rId[i] = 0;
//	}
//	cache->nId[0] = 10;
//	cache->nId[1] = 11;
//	cache->nId[2] = 12;
//	cache->nId[3] = 13;
//	cache->nId[4] = 14;
//
//	cache->rId[0] = 2;
//	cache->rId[1] = 5;
//	cache->rId[2] = 7;
//	cache->rId[3] = 3;
//
//	long long id = getOneId();

	// get new Id from next free IDs
//	char *taid = "D:/tudata/tustore.timeaxis.db.id";
//	taidfp = fopen(taid, "rb+");
//	char *tadb = "D:/tudata/tustore.timeaxis.db";
//	tadbfp = fopen(tadb, "rb+");

	// test to get one id.
//	long long id = getId(taidfp);
//	printf("%lld\n", id);
//	id = getId(taidfp);
//	printf("%lld\n", id);
//	id = getId(taidfp);
//	printf("%lld\n", id);
//	id = getId(taidfp);
//	printf("%lld\n", id);
//	id = getId(taidfp);
//	printf("%lld\n", id);

	// test recycle one id
//	recycleId(taidfp, 6);
//	recycleId(taidfp, 7);
//	recycleId(taidfp, 2);
//	recycleId(taidfp, 3);
//	recycleId(taidfp, 4);

//	readAllTaIds(taidfp);

//	fclose(taidfp);
//	fclose(tadbfp);

//	free(cache);
//	printf("%lld\d", id);

}
