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
	for (int i = 0; i < LONG_LONG; i++) {
		nxt[i] = *(p + i + LONG_LONG);
	}
	return ByteArrayToLong(nxt);
}

long long parseStamp(unsigned char *p) {
	unsigned char tms[LONG_LONG] = { 0L };
	for (int i = 0; i < LONG_LONG; i++) {
		tms[i] = *(p + i + 2 * LONG_LONG);
	}
	return ByteArrayToLong(tms);
}

void putPrvId(evolved_point_t *ep) {
	unsigned char prv[LONG_LONG] = { 0 };
	LongToByteArray(ep->prvTsId, prv);
	for (int i = 0; i < LONG_LONG; i++) {
		*(ep->pos + i) = prv[i];
	}
}

void putNxtId(evolved_point_t *ep) {
	unsigned char nxt[LONG_LONG] = { 0 };
	LongToByteArray(ep->nxtTsId, nxt);
	for (int i = 0; i < LONG_LONG; i++) {
		*(ep->pos + i + LONG_LONG) = nxt[i];
	}
}

void putStamp(evolved_point_t *ep) {
	unsigned char ts[LONG_LONG] = { 0 };
	LongToByteArray(ep->time, ts);
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
void lookup(long long ts, unsigned char *curr, ta_page_t *page,
		evolved_point_t *tmp, evolved_point_t *previous, evolved_point_t *next,
		FILE *tadbfp) {
	// current record, it may be previous/next record
	long long prvId = parsePrvId(curr);
	long long nxtId = parseNxtId(curr);
	long long stamp = parseStamp(curr);
	if (stamp == 0) {
		// this is an empty DB,
		// so this time stamp will be stored here
		tmp->pos = curr;
		tmp->nxtTsId = NULL_POINTER;
		tmp->prvTsId = NULL_POINTER;
		return;
	} else if (stamp != 0) {
		if (ts < stamp) {
			// tmp will be the next record of the current record
			if (prvId == NULL_POINTER) {
				long long id = getOneId();
				tmp->id = id; //
				tmp->prvTsId = NULL_POINTER;
				tmp->nxtTsId = timeaxispages->first;
				next->prvTsId = id; //
				// this next is current record
				next->pos = curr;
				// next->nxtTsId = nxtId; // keep same
				timeaxispages->first = id;
				timeaxispages->firstduty = 1;
			} else {
				// lookup backwards
				long long prv = page->start
						+ (prvId - page->startNo) * (3 * LONG_LONG);
				if (prv >= page->start) { // in this page
					unsigned char *p2 = page->content
							+ (prvId - page->startNo) * (3 * LONG_LONG);
					lookup(ts, p2, page, tmp, previous, next, tadbfp);
				} else if (prv < page->start) { // not in this page
					ta_page_t *prp = findPage(prvId);
					if (prp == NULL) {
						long long pn = (prvId * (3 * LONG_LONG)) / 240;
						long long st = pn * 240 + 16L;
						prp = readOnePage(st, pn * 10, tadbfp);
					}
					unsigned char *p3 = prp->content
							+ (prvId - prp->startNo) * (3 * LONG_LONG);
					lookup(ts, p3, prp, tmp, previous, next, tadbfp);
				}
			}
		} else if (ts > stamp) {
			// tmp record will be previous record of the current record
			if (nxtId == NULL_POINTER) {
				long long id = getOneId();
				tmp->id = id; //
				tmp->prvTsId = timeaxispages->last;
				tmp->nxtTsId = NULL_POINTER;
				// the previous is the current record
				previous->nxtTsId = id; //
				previous->pos = curr;
				//previous->prvTsId = nxtId;// keep
				timeaxispages->last = id;
				timeaxispages->lastduty = 1;
			} else {
				long long nxt = page->start
						+ (nxtId - page->startNo) * (3 * LONG_LONG);
				if (nxt <= page->end) { // in this page
					unsigned char *p2 = page->content
							+ (nxtId - page->startNo) * (3 * LONG_LONG);
					lookup(ts, p2, page, tmp, previous, next, tadbfp);
				} else if (nxt > page->end) { // not in this page
					ta_page_t *nxp = findPage(nxtId);
					if (nxp == NULL) {
						long long pn = (nxtId * (3 * LONG_LONG)) / 240;
						long long st = pn * 240 + 16L;
						nxp = readOnePage(st, pn * 10, tadbfp);
					}
					unsigned char *p3 = nxp->content
							+ (nxtId - nxp->startNo) * (3 * LONG_LONG);
					lookup(ts, p3, nxp, tmp, previous, next, tadbfp);
				}
			}
		} else if (ts == stamp) {
			// tmp is the current record
			ta_page_t *nxp = findPage(nxtId);
			if (nxp == NULL) {
				long long pn = (nxtId * (3 * LONG_LONG)) / 240;
				long long st = pn * 240 + 16L;
				nxp = readOnePage(st, pn * 10, tadbfp);
			}
			unsigned char *p4 = nxp->content
					+ (nxtId - nxp->startNo) * (3 * LONG_LONG);
			long long thisprvId = parsePrvId(p4);
			tmp->id = thisprvId;
			tmp->time = ts;
			tmp->prvTsId = prvId;
			tmp->nxtTsId = nxtId;
			tmp->pos = curr;
		}
	}
}

void search(long long ts, unsigned char *currpos, ta_page_t *currpage,
		evolved_point_t *tmp, evolved_point_t *previous, evolved_point_t *next,
		FILE *tadbfp) {
	// current record, it may be previous/next record
	long long prvId = parsePrvId(currpos);
	long long nxtId = parseNxtId(currpos);
	long long stamp = parseStamp(currpos);
	if (stamp == 0) {
		// this is an empty DB,
		// so this time stamp will be stored here
		tmp->pos = currpos;
		tmp->nxtTsId = NULL_POINTER;
		tmp->prvTsId = NULL_POINTER;
		return;
	} else if (stamp != 0) {
		if (ts < stamp) {
			while (currpage != NULL) {
				// current record, it may be previous/next record
				long long prvId = parsePrvId(currpos);
				long long nxtId = parseNxtId(currpos);
				long long stamp = parseStamp(currpos);
				// tmp will be the next record of the current record
				if (prvId == NULL_POINTER) {
					long long id = getOneId();
					tmp->id = id; //
					tmp->prvTsId = NULL_POINTER;
					tmp->nxtTsId = timeaxispages->first;
					next->prvTsId = id; //
					// this next is current record
					next->pos = currpos;
					// next->nxtTsId = nxtId; // keep same
					timeaxispages->first = id;
					timeaxispages->firstduty = 1;
					return;
				} else {
					if (ts < stamp) {
						// lookup backwards
						long long prv = currpage->start
								+ (prvId - currpage->startNo) * (3 * LONG_LONG);
						if (prv >= currpage->start) { // in this page
							currpos = currpage->content
									+ (prvId - currpage->startNo)
											* (3 * LONG_LONG);
						} else if (prv < currpage->start) { // not in this page
							ta_page_t *prp = findPage(prvId);
							if (prp == NULL) {
								long long pn = (prvId * (3 * LONG_LONG)) / 240;
								long long st = pn * 240 + 16L;
								currpage = readOnePage(st, pn * 10, tadbfp);
							} else {
								currpage = prp;
							}
							currpos = currpos = currpage->content
									+ (prvId - currpage->startNo)
											* (3 * LONG_LONG);
						}
						continue;
					} else if (ts > stamp) {
						ta_page_t *prvp = findPage(prvId);
						if (prvp == NULL) {
							long long pn = (prvId * (3 * LONG_LONG)) / 240;
							long long st = pn * 240 + 16L;
							prvp = readOnePage(st, pn * 10, tadbfp);
						}
						unsigned char *cpos = prvp->content
								+ (prvId - prvp->startNo) * (3 * LONG_LONG);
						//long long prvId1 = parsePrvId(cpos);
						long long curId = parseNxtId(cpos);
						//long long stamp1 = parseStamp(cpos);

						long long id = getOneId();
						tmp->id = id; //
						tmp->prvTsId = curId; //
						tmp->nxtTsId = nxtId;
						tmp->time = ts;

						previous->id = curId;
						previous->nxtTsId = id;
						previous->pos = currpos;

						ta_page_t *nxtp = findPage(nxtId);
						if (nxtp == NULL) {
							long long pn1 = (nxtId * (3 * LONG_LONG)) / 240;
							long long st1 = pn1 * 240 + 16L;
							nxtp = readOnePage(st1, pn1 * 10, tadbfp);
						}
						unsigned char *npos = nxtp->content
								+ (nxtId - nxtp->startNo) * (3 * LONG_LONG);
						//long long prvId2 = parsePrvId(npos);
						//long long nxtId2 = parseNxtId(npos);
						//long long stamp2 = parseStamp(npos);

						next->id = nxtId;
						next->prvTsId = id; //
						next->pos = npos;
						return;
					}
				}
			}
		}
	} else if (ts > stamp) {
		while (currpage != NULL) {
			// current record, it may be previous/next record
			long long prvId = parsePrvId(currpos);
			long long nxtId = parseNxtId(currpos);
			long long stamp = parseStamp(currpos);
			if (nxtId == NULL_POINTER) {
				long long id = getOneId();
				tmp->id = id; //
				tmp->prvTsId = timeaxispages->last;
				tmp->nxtTsId = NULL_POINTER;
				// the previous is the current record
				previous->nxtTsId = id; //
				previous->pos = currpos;
				//previous->prvTsId = nxtId;// keep
				timeaxispages->last = id;
				timeaxispages->lastduty = 1;
				return;
			} else {
				if (ts > stamp) {
					long long nxt = currpage->start
							+ (nxtId - currpage->startNo) * (3 * LONG_LONG);
					if (nxt <= currpage->end) { // in this page
						currpos = currpage->content
								+ (nxtId - currpage->startNo) * (3 * LONG_LONG);
					} else if (nxt > currpage->end) { // not in this page
						ta_page_t *nxp = findPage(nxtId);
						if (nxp == NULL) {
							long long pn = (nxtId * (3 * LONG_LONG)) / 240;
							long long st = pn * 240 + 16L;
							currpage = readOnePage(st, pn * 10, tadbfp);
						} else {
							currpage = nxp;
						}
						currpos = currpage->content
								+ (nxtId - currpage->startNo) * (3 * LONG_LONG);
					}
					continue;
				} else if (ts < stamp) {
					ta_page_t *nxtp = findPage(nxtId);
					if (nxtp == NULL) {
						long long pn = (nxtId * (3 * LONG_LONG)) / 240;
						long long st = pn * 240 + 16L;
						nxtp = readOnePage(st, pn * 10, tadbfp);
					}
					unsigned char *cpos = nxtp->content
							+ (nxtId - nxtp->startNo) * (3 * LONG_LONG);
					long long curId = parsePrvId(cpos);
					//long long nxtId1 = parseNxtId(cpos);
					//long long stamp1 = parseStamp(cpos);

					long long id = getOneId();
					tmp->id = id; //
					tmp->prvTsId = prvId; //
					tmp->nxtTsId = curId;
					tmp->time = ts;

					next->id = curId;
					next->prvTsId = id;
					next->pos = currpos;

					ta_page_t *prvp = findPage(prvId);
					if (prvp == NULL) {
						long long pn1 = (prvId * (3 * LONG_LONG)) / 240;
						long long st1 = pn1 * 240 + 16L;
						prvp = readOnePage(st1, pn1 * 10, tadbfp);
					}
					unsigned char *ppos = prvp->content
							+ (prvId - prvp->startNo) * (3 * LONG_LONG);
					//long long prvId2 = parsePrvId(npos);
					//long long nxtId2 = parseNxtId(ppos);
					//long long stamp2 = parseStamp(npos);
					previous->id = prvId;
					previous->nxtTsId = id; //
					previous->pos = ppos;

					return;
				}
			}
		}
	} else if (ts == stamp) {
		// tmp is the current record
		ta_page_t *nxp = findPage(nxtId);
		if (nxp == NULL) {
			long long pn = (nxtId * (3 * LONG_LONG)) / 240;
			long long st = pn * 240 + 16L;
			currpage = readOnePage(st, pn * 10, tadbfp);
		}
		currpos = currpage->content
				+ (nxtId - currpage->startNo) * (3 * LONG_LONG);
		long long curId = parsePrvId(currpos);
		tmp->id = curId;
		tmp->time = ts;
		tmp->prvTsId = prvId;
		tmp->nxtTsId = nxtId;
		tmp->pos = currpos;
		return;
	}
}

void commitEvolvedPoint(evolved_point_t *ep, FILE *tadbfp) {

}

long long updateEvolvedPoint(long long ts, FILE *taidfp, FILE *tadbfp) {
	ta_page_t *pp = timeaxispages->pages;
	long long id = -1;
	if (pp != NULL) {
		evolved_point_t *t = (evolved_point_t*) malloc(sizeof(evolved_point_t));
		t->prvTsId = -3;
		t->nxtTsId = -3;
		t->time = 0;
		t->id = -3;
		evolved_point_t *p = (evolved_point_t*) malloc(sizeof(evolved_point_t));
		p->prvTsId = -3;
		p->nxtTsId = -3;
		p->time = 0;
		p->id = -3;
		evolved_point_t *n = (evolved_point_t*) malloc(sizeof(evolved_point_t));
		n->prvTsId = -3;
		n->nxtTsId = -3;
		n->time = 0;
		n->id = -3;

		search(ts, pp->content, pp, t, p, n, tadbfp);
		ta_page_t *newp = findPage(t->id);
		if (newp == NULL) {
			long long pn = (t->id * (3 * LONG_LONG)) / 240;
			long long st = pn * 240 + 16L;
			newp = readOnePage(st, pn * 10, tadbfp);
		}
		t->pos = newp->content + (t->id - newp->startNo) * (3 * LONG_LONG);
		if (t->pos != NULL) {
			if (t->time != ts) { // found a position
				// insert the evolved point at the position
				t->time = ts;
				putPrvId(t);
				putNxtId(t);
				putStamp(t);
				newp->dirty = 1;
				// update previous evolved point
				if (p->prvTsId != -3)
					putPrvId(p);
				else if (p->nxtTsId != -3)
					putNxtId(p);
				// update next evolved point
				if (n->prvTsId != -3)
					putPrvId(n);
				else if (n->nxtTsId != -3)
					putNxtId(n);
			}
			id = t->id;
		}
		free(t);
		free(p);
		free(n);
	}
	return id;
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

