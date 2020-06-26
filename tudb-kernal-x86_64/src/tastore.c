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
/*void getEvolvedPoint(evolved_point_t *ep, unsigned char *p) {
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

void parseEvolvedPoint(long long prvId, long long nxtId, long long ts,
		unsigned char *p) {
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
	prvId = ByteArrayToLong(prv);
	nxtId = ByteArrayToLong(nxt);
	ts = ByteArrayToLong(tms);
}*/

long long parsePrvId(unsigned char *p) {
	unsigned char prv[LONG_LONG] = { 0L };
	for (int i = 0; i < LONG_LONG; i++) {
		prv[i] = *(p + i);
	}
	return ByteArrayToLong(prv);
}

long long parseNxtId(unsigned char *p) {
	unsigned char nxt[LONG_LONG] = { 0L };
	for (int i = LONG_LONG; i < 2 * LONG_LONG; i++) {
		nxt[i] = *(p + i);
	}
	return ByteArrayToLong(nxt);
}

long long parseStamp(unsigned char *p) {
	unsigned char tms[LONG_LONG] = { 0L };
	for (int i = 2 * LONG_LONG; i < 3 * LONG_LONG; i++) {
		tms[i] = *(p + i);
	}
	return ByteArrayToLong(tms);
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

ta_page_t* readOnePage(long long start, long long startNo, FILE *tadbfp) {
	unsigned char *page = (unsigned char*) malloc(
			sizeof(unsigned char) * TA_PAGE_SIZE);
	memset(page, 0, sizeof(unsigned char) * TA_PAGE_SIZE);
	fseek(tadbfp, start, SEEK_SET); // start to read from the first record
	int c;
	if ((c = fgetc(tadbfp)) != EOF) {
		fseek(tadbfp, start, SEEK_SET);
		fread(page, sizeof(unsigned char), TA_PAGE_SIZE, tadbfp); // read one page
	}

	ta_page_t *p = (ta_page_t*) malloc(sizeof(ta_page_t));
	p->dirty = 0;
	p->expiretime = 10; // 10 minutes
	p->startNo = startNo;
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
	return p;
}

ta_page_t* findPage(long long id) {
	ta_page_t *page = timeaxispages->pages;
	if (page != NULL) {
		while (page != NULL) {
			if (id >= page->start && id < page->end) {
				return page;
			} else {
				page = page->nxtpage;
			}
		}
	}
	return NULL;
}

/**
 * ts
 * tmp: search result
 */
void lookupInPage(long long ts, unsigned char *curr, unsigned char *pstart,
		long long startNo, long long start, long long end, evolved_point_t *tmp,
		FILE *tadbfp) {
	long long prvId = parsePrvId(curr);
	long long nxtId = parseNxtId(curr);
	long long stamp = parseStamp(curr);
	if (stamp == 0) {
		// this is empty DB, so ts will be stored here
		tmp->pos = curr;
		return;
	} else if (stamp != 0) {
		if (ts > stamp) {
			// found current record Id
			if (prvId != NULL_POINTER) {
				ta_page_t *prp = findPage(prvId);
				if (prp == NULL) {
					long long pn = (prvId * (3 * LONG_LONG)) / 240;
					long long st = pn * 240 + 16L;
					prp = readOnePage(st, pn * 10, tadbfp);
				}
				unsigned char *p1 = prp->content
						+ (prvId - prp->startNo) * (3 * LONG_LONG);
				long long nxtId1 = parseNxtId(p1);
				tmp->prvTsId = nxtId1;
			} else {
				tmp->prvTsId = NULL_POINTER;
			}
			// lookup forwards
			long long nxt = start + nxtId * (3 * LONG_LONG);
			if (nxt <= end) { // in this page
				unsigned char *p2 = pstart
						+ (nxtId - startNo) * (3 * LONG_LONG);
				lookupInPage(ts, p2, pstart, startNo, start, end, tmp, tadbfp);
			} else if (nxt > end) { // not in this page
				ta_page_t *nxp = findPage(nxtId);
				if (nxp == NULL) {
					long long pn = (nxtId * (3 * LONG_LONG)) / 240;
					long long st = pn * 240 + 16L;
					nxp = readOnePage(st, pn * 10, tadbfp);
				}
				unsigned char *p3 = nxp->content
						+ (nxtId - nxp->startNo) * (3 * LONG_LONG);
				lookupInPage(ts, p3, nxp->content, nxp->startNo, nxp->start,
						nxp->end, tmp, tadbfp);
			}
		}
	} else if (ts < stamp) {
		if (nxtId != NULL_POINTER) {
			ta_page_t *nxp = findPage(nxtId);
			if (nxp == NULL) {
				long long pn = (nxtId * (3 * LONG_LONG)) / 240;
				long long st = pn * 240 + 16L;
				nxp = readOnePage(st, pn * 10, tadbfp);
			}
			unsigned char *p1 = nxp->content
					+ (nxtId - nxp->startNo) * (3 * LONG_LONG);
			long long prvId1 = parsePrvId(p1);
			tmp->nxtTsId = prvId1;
		} else {
			tmp->nxtTsId = NULL_POINTER;
		}
		// lookup backwards
		long long prv = start + prvId * (3 * LONG_LONG);
		if (prv >= start) { // in this page
			unsigned char *p2 = pstart + (prvId - startNo) * (3 * LONG_LONG);
			lookupInPage(ts, p2, pstart, startNo, start, end, tmp, tadbfp);
		} else if (prv < start) {
			ta_page_t *prp = findPage(prvId);
			if (prp == NULL) {
				long long pn = (prvId * (3 * LONG_LONG)) / 240;
				long long st = pn * 240 + 16L;
				prp = readOnePage(st, pn * 10, tadbfp);
			}
			unsigned char *p3 = prp->content
					+ (prvId - prp->startNo) * (3 * LONG_LONG);
			lookupInPage(ts, p3, prp->content, prp->startNo, prp->start,
					prp->end, tmp, tadbfp);
		}
	}
	if (tmp->nxtTsId != -3 && tmp->nxtTsId != -3) {
		return;
	}
}

void commitEvolvedPoint(evolved_point_t *ep, FILE *tadbfp) {

}

void updateEvolvedPoint(long long ts, FILE *taidfp, FILE *tadbfp) {
	ta_page_t *pp = timeaxispages->pages;
	if (pp != NULL) {
		evolved_point_t *t = (evolved_point_t*) malloc(sizeof(evolved_point_t));
		t->prvTsId = -3;
		t->nxtTsId = -3;
		t->time = ts;
		while (pp != NULL) {
			if (pp->expiretime != -1) { // does not expired
				lookupInPage(ts, pp->content, pp->content, pp->startNo,
						pp->start, pp->end, t, tadbfp);
			}
			if (t->pos != NULL) { // found a position
				break;
			} else {
				pp = pp->nxtpage;
			}
		}
		if (t->pos != NULL && t->time != ts) { // found a position
		// insert the evolved point at the position
			evolved_point_t *nwep = (evolved_point_t*) malloc(
					sizeof(evolved_point_t));
			long long id = getOneId();
			nwep->id = id;
			nwep->nxtTsId = t->nxtTsId;
			nwep->prvTsId = t->prvTsId;
			nwep->time = ts;
			nwep->pos = t->pos;

			putEvolvedPoint(nwep);
			pp->dirty = 1;
			free(nwep);
			//for (int i = 0; i < 24; i++) {
			//	printf("%d\n", *(r + i));
			//}
		}
		free(t);
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
		readOnePage(16L, 0, tadbfp);
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

