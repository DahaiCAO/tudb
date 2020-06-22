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
/*
 * text.c
 *
 * Created on: 2020年6月8日
 * Author: Dahai CAO
 */

void convertEpToBytes(evolved_point_t *ep, unsigned char buf[]) {
	memset(buf, 0, 25);
	unsigned char prv[LONG_LONG] = { 0 };
	unsigned char nxt[LONG_LONG] = { 0 };
	unsigned char ts[LONG_LONG] = { 0 };
	LongToByteArray(ep->prvTsId, prv);
	LongToByteArray(ep->nxtTsId, nxt);
	LongToByteArray(ep->time, ts);
	for (int i = 0; i < 8; i++) {
		buf[i] = prv[i];
	}
	for (int i = 8; i < 16; i++) {
		buf[i] = nxt[i];
	}
	for (int i = 16; i < 24; i++) {
		buf[i] = ts[i];
	}
}

void convertBytesToEp(evolved_point_t *ep, unsigned char buf[]) {
	unsigned char prv[LONG_LONG] = { 0 };
	unsigned char nxt[LONG_LONG] = { 0 };
	unsigned char ts[LONG_LONG] = { 0 };
	for (int i = 0; i < 8; i++) {
		prv[i] = buf[i];
	}
	for (int i = 8; i < 16; i++) {
		nxt[i] = buf[i];
	}
	for (int i = 16; i < 24; i++) {
		ts[i] = buf[i];
	}
	ep->prvTsId = ByteArrayToLong(prv);
	ep->nxtTsId = ByteArrayToLong(nxt);
	ep->time = ByteArrayToLong(ts);
}

ta_page_t* readOnePage(long long start, FILE *tadbfp) {
	unsigned char page[TA_PAGE_SIZE] = { 0L };
	fseek(tadbfp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(tadbfp)) != EOF) {
		fseek(tadbfp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), TA_PAGE_SIZE, tadbfp); // read one page
	}

	ta_page_t *p = (ta_page_t*) malloc(sizeof(ta_page_t));
	p->duty = 0;
	p->expiretime = 10; // 10 minutes
	p->hited = 0;
	p->nxtpage = NULL;
	p->prvpage = NULL;
	p->content = page;
	p->start = start;
	p->end = start + TA_PAGE_SIZE;

	ta_page_t *pp = timeaxispages->pages;
	if (pp != NULL) {
		while (pp->nxtpage != NULL) {
			pp = pp->nxtpage;
		}
		pp->nxtpage = p;
		p->prvpage = pp;
	} else {
		pp = p;
	}
	return pp;
}

void saveEvolvedPoint(long long ts, FILE *taidfp, FILE *tadbfp) {
//	unsigned char first[LONG_LONG] = { 0L };
//	unsigned char last[LONG_LONG] = { 0L };
//	unsigned char p[LONG_LONG * 2] = { 0 };
//	fseek(tadbfp, 0L, SEEK_SET); //
//	fread(p, sizeof(unsigned char), LONG_LONG * 2, tadbfp);
//	for (int i = 0; i < LONG_LONG; i++) {
//		first[i] = p[i];
//	}
//	for (int i = LONG_LONG; i < 2 * LONG_LONG; i++) {
//		last[i] = p[i];
//	}
//	long long firstId = ByteArrayToLong(first);
//	long long lastId = ByteArrayToLong(last);

//	for (int i = 0; i < 10; i++) {
//		unsigned char p[LONG_LONG] = { 0L };
//		unsigned char n[LONG_LONG] = { 0L };
//		unsigned char t[LONG_LONG] = { 0L };
//		int start = 24 * i;
//		for (int j = start; j < start + 8; j++) {
//			p[j] = page[j];
//		}
//		for (int j = start + 8; j < start + 16; j++) {
//			n[j] = page[j];
//		}
//		for (int j = start + 16; j < start + 24; j++) {
//			t[j] = page[j];
//		}
//
//	}
}

/*
 void createTimeAixs(long long ts, FILE *taidfp, FILE *tadbfp) {
 // get a new Id
 //unsigned char page[PAGESIZE] = { 0L };
 unsigned char tsBytes[EPSIZE] = { 0L };
 unsigned char p[BUFFERSIZE * 2] = { 0 };
 unsigned char first[BUFFERSIZE] = { 0L };
 unsigned char last[BUFFERSIZE] = { 0L };
 LongToByteArray(ts, tsBytes); // convert time stamp to byte array
 fseek(tadbfp, 0L, SEEK_SET); //
 fread(p, sizeof(unsigned char), LONG_LONG * 2, tadbfp);
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
 //long long id = getOneId(); // get new Id
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
 fwrite(0, sizeof(unsigned char), LONG_LONG, tadbfp);
 fwrite(0, sizeof(unsigned char), LONG_LONG, tadbfp);
 fwrite(tsBytes, sizeof(unsigned char), EPSIZE, tadbfp);
 free(ep);
 } else {
 // read one page
 }

 }*/

int main(int argv, char **argc) {
	setvbuf(stdout, NULL, _IONBF, 0);
	// get new Id from next free IDs
	char *taid = "D:/tudata/tustore.timeaxis.tdb.id";
	FILE *taidfp = fopen(taid, "rb+");
	char *tadb = "D:/tudata/tustore.timeaxis.tdb";
	FILE *tadbfp = fopen(tadb, "rb+");

	// initialize server when install server.
	// initTimeAxisIdDB(taid);
	// initTimeAxisDB(tadb);

	// initialize
	cache = (id_cache_t*) malloc(sizeof(id_cache_t));
	cache->nId = NULL;
	cache->rId = NULL;

	timeaxispages = (ta_buf_t*) malloc(sizeof(ta_buf_t));
	timeaxispages->pages = NULL;

	//loadIds(taidfp);
	//readAllTaIds(taidfp);

	ta_page_t *p = readOnePage(16L, tadbfp);

	long long ts = (unsigned long) time(NULL);
	saveEvolvedPoint(ts, taidfp, tadbfp);

	fclose(taidfp);
	fclose(tadbfp);

//	free(timeaxispages);
//	free(cache);
//	printf("%lld\d", id);

}

