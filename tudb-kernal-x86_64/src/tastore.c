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

//void convertEpToBytes(evolved_point_t *ep, unsigned char buf[]) {
//	memset(buf, 0, 25);
//	unsigned char prv[LONG_LONG] = { 0 };
//	unsigned char nxt[LONG_LONG] = { 0 };
//	unsigned char ts[LONG_LONG] = { 0 };
//	LongToByteArray(ep->prvTsId, prv);
//	LongToByteArray(ep->nxtTsId, nxt);
//	LongToByteArray(ep->time, ts);
//	for (int i = 0; i < 8; i++) {
//		buf[i] = prv[i];
//	}
//	for (int i = 8; i < 16; i++) {
//		buf[i] = nxt[i];
//	}
//	for (int i = 16; i < 24; i++) {
//		buf[i] = ts[i];
//	}
//}
//
//void convertBytesToEp(evolved_point_t *ep, unsigned char buf[]) {
//	unsigned char prv[LONG_LONG] = { 0 };
//	unsigned char nxt[LONG_LONG] = { 0 };
//	unsigned char ts[LONG_LONG] = { 0 };
//	for (int i = 0; i < 8; i++) {
//		prv[i] = buf[i];
//	}
//	for (int i = 8; i < 16; i++) {
//		nxt[i] = buf[i];
//	}
//	for (int i = 16; i < 24; i++) {
//		ts[i] = buf[i];
//	}
//	ep->prvTsId = ByteArrayToLong(prv);
//	ep->nxtTsId = ByteArrayToLong(nxt);
//	ep->time = ByteArrayToLong(ts);
//}
void getEvolvedPoint(evolved_point_t *ep, unsigned char *p) {
	unsigned char prv[LONG_LONG] = { 0L };
	unsigned char nxt[LONG_LONG] = { 0L };
	unsigned char tms[LONG_LONG] = { 0L };
	for (int i = 0; i < LONG_LONG; i++) {
		prv[i] = *(p + i);
	}
	for (int i = LONG_LONG; i < 2 * LONG_LONG; i++) {
		nxt[i] = *(p + i);
	}
	for (int i = 2 * LONG_LONG; i < 3 * LONG_LONG; i++) {
		tms[i] = *(p + i);
	}
	ep->prvTsId = ByteArrayToLong(prv);
	ep->nxtTsId = ByteArrayToLong(nxt);
	ep->time = ByteArrayToLong(tms);
}

void putEvolvedPoint(evolved_point_t *ep) {
	unsigned char prv[LONG_LONG] = { 0 };
	unsigned char nxt[LONG_LONG] = { 0 };
	unsigned char ts[LONG_LONG] = { 0 };
	LongToByteArray(ep->prvTsId, prv);
	LongToByteArray(ep->nxtTsId, nxt);
	LongToByteArray(ep->time, ts);
	for (int i = 0; i < LONG_LONG; i++) {
		*(ep->pos + i) = prv[i];
	}
	for (int i = 0; i < LONG_LONG; i++) {
		*(ep->pos + i + LONG_LONG) = nxt[i];
	}
	for (int i = 0; i < LONG_LONG; i++) {
		*(ep->pos + i + 2 * LONG_LONG) = ts[i];
	}
}

void readOnePage(long long start, FILE *tadbfp) {
	unsigned char page[TA_PAGE_SIZE] = { 0L };
	fseek(tadbfp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(tadbfp)) != EOF) {
		fseek(tadbfp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), TA_PAGE_SIZE, tadbfp); // read one page
	}

	ta_page_t *p = (ta_page_t*) malloc(sizeof(ta_page_t));
	p->dirty = 0;
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
		timeaxispages->pages = p;
	}
}

void lookupInPage(long long ts, unsigned char *position, long long start,
		long long end, evolved_point_t *tmp, FILE *tadbfp) {
	unsigned char *p = position;
	getEvolvedPoint(tmp, p);
	if (tmp->time == 0) {
		// this is empty DB, so ts will be stored here
		tmp->pos = position;
	} else if (tmp->time != 0) {
		if (ts > tmp->time) {
			// lookup forwards
			long long n = start + tmp->nxtTsId * (3 * LONG_LONG);
			if (n > end) {
				// look up other pages, if not exists, read one page again;
				ta_page_t *pp = timeaxispages->pages;
				if (pp != NULL) {
					int r = -1;
					while (pp != NULL) {
						if (n > pp->start && n < pp->end) {
							evolved_point_t *ep = (evolved_point_t*) malloc(
									sizeof(evolved_point_t));
							lookupInPage(ts, pp->content, pp->start, pp->end,
									ep, tadbfp);
							if (r == 1) {
								break; // found
							}
						} else {
							if (pp->nxtpage != NULL) {
								pp = pp->nxtpage;
							} else {
								readOnePage(pp->end, tadbfp);
							}
						}
					}
					if (r == -1) {
						//
					}
				}
			} else {

			}
		} else if (ts < tmp->time) {
			// lookup backwards
			long long p = tmp->prvTsId + 3 * LONG_LONG;
			// look up other pages, if not exists, read one page again;
			if (p < start) {

			}
		} else {

		}
	}
}

void commitEvolvedPoint(evolved_point_t *ep, FILE *tadbfp) {

}

void updateEvolvedPoint(long long ts, FILE *taidfp, FILE *tadbfp) {
	ta_page_t *pp = timeaxispages->pages;
	if (pp != NULL) {
		evolved_point_t *ep = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		while (pp != NULL) {
			if (pp->expiretime != -1) { // does not expired
				lookupInPage(ts, pp->content, pp->start, pp->end, ep, tadbfp);
			}
			if (ep->pos != NULL) { // found a position
				break;
			} else {
				pp = pp->nxtpage;
			}
		}
		if (ep->pos != NULL) { // found a position
			// insert the evolved point at the position
			evolved_point_t *nwep = (evolved_point_t*) malloc(
					sizeof(evolved_point_t));
			long long id = getOneId();
			nwep->id = id;
			nwep->nxtTsId = ep->nxtTsId;
			nwep->prvTsId = ep->prvTsId;
			nwep->time = ts;
			nwep->pos = ep->pos;

			putEvolvedPoint(nwep);
			pp->dirty = 1;
			free(nwep);
			//for (int i = 0; i < 24; i++) {
			//	printf("%d\n", *(r + i));
			//}
			free(ep);
		}
	}

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

	if (timeaxispages->pages == NULL) {
		readOnePage(16L, tadbfp);
	}

	//loadIds(taidfp);
	//readAllTaIds(taidfp);

	long long ts = (unsigned long) time(NULL);
	updateEvolvedPoint(ts, taidfp, tadbfp);
	long long ts1 = (unsigned long) time(NULL);
	updateEvolvedPoint(ts1, taidfp, tadbfp);

	// commitEvolvedPoint(ep, tadbfp);

	fclose(taidfp);
	fclose(tadbfp);

	free(timeaxispages);
	free(cache);
	//	printf("%lld\d", id);
	/*
	 unsigned char *p = (unsigned char*) malloc(sizeof(unsigned char) * 3);
	 unsigned char a[3] = { 1, 2, 3 };
	 for (int i = 0; i < 3; i++) {
	 *(p + i) = a[i];
	 }
	 //
	 for (int i = 0; i < 3; i++) {
	 printf("%d\n",*(p+i));
	 //printf("%d\n",a[i]);
	 }
	 */
}

