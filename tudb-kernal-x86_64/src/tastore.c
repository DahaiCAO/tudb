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
 * This class is used to manage time axis store.
 *
 * Created on: 2020年6月8日
 * Author: Dahai CAO
 */

unsigned char parseInUse(unsigned char *p) {
	unsigned char inuse = *p;
	return inuse;
}

long long parsePrvId(unsigned char *p) {
	unsigned char prv[LONG_LONG] = { 0L };
	for (int i = 0; i < LONG_LONG; i++) {
		prv[i] = *(p + i + 1);
	}
	return ByteArrayToLong(prv);
}

long long parseNxtId(unsigned char *p) {
	unsigned char nxt[LONG_LONG] = { 0L };
	for (int i = 0; i < LONG_LONG; i++) {
		nxt[i] = *(p + i + LONG_LONG + 1);
	}
	return ByteArrayToLong(nxt);
}

long long parseStamp(unsigned char *p) {
	unsigned char tms[LONG_LONG] = { 0L };
	for (int i = 0; i < LONG_LONG; i++) {
		tms[i] = *(p + i + 2 * LONG_LONG + 1);
	}
	return ByteArrayToLong(tms);
}

void putInUse(evolved_point_t *ep) {
	*ep->pos = ep->inuse;
}

void putPrvId(evolved_point_t *ep) {
	unsigned char prv[LONG_LONG] = { 0 };
	LongToByteArray(ep->prvTsId, prv);
	for (int i = 0; i < LONG_LONG; i++) {
		*(ep->pos + i + 1) = prv[i];
	}
}

void putNxtId(evolved_point_t *ep) {
	unsigned char nxt[LONG_LONG] = { 0 };
	LongToByteArray(ep->nxtTsId, nxt);
	for (int i = 0; i < LONG_LONG; i++) {
		*(ep->pos + i + LONG_LONG + 1) = nxt[i];
	}
}

void putStamp(evolved_point_t *ep) {
	unsigned char ts[LONG_LONG] = { 0 };
	LongToByteArray(ep->time, ts);
	for (int i = 0; i < LONG_LONG; i++) {
		*(ep->pos + i + 2 * LONG_LONG + 1) = ts[i];
	}
}

// read one page
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
	p->hit = 0;
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

// search one Id in all memory pages
ta_page_t* findPage(long long id) {
	ta_page_t *page = timeaxispages->pages;
	if (page != NULL) {
		while (page != NULL) {
			long long pos = 16LL + id * (3 * LONG_LONG + 1);
			if (pos >= page->start && pos < page->end) {
				return page;
			} else {
				page = page->nxtpage;
			}
		}
	}
	return NULL;
}

// show all records in the memory pages
void showAllPages() {
	ta_page_t *page = timeaxispages->pages;
	printf("----DB Head----\n");
	printf("First Id:%d\n", timeaxispages->first);
	printf("Last Id:%d\n", timeaxispages->last);
	if (page != NULL) {
		while (page != NULL) {
			printf("----Page head----\n");
			printf("Page hit:%d\n", page->hit);
			printf("Page duty:%d\n", page->dirty);
			printf("Page expire time:%lld minutes\n", page->expiretime);
			printf("Page start offset:%lld\n", page->start);
			printf("Page end offset:%lld\n", page->end);
			printf("Page start id:%lld\n", page->startNo);
			printf("----Page body----\n");
			int i = 0;
			unsigned char *curr;
			while (i < 10) { // 10 is page size.
				curr = page->content + i * (3 * LONG_LONG + 1);
				if (curr != NULL) {
					unsigned char inuse = parseInUse(curr);
					long long prvId = parsePrvId(curr);
					long long nxtId = parseNxtId(curr);
					long long stamp = parseStamp(curr);
					if (stamp != 0LL) {
						printf("%d %lld|%lld|%lld\n", inuse, prvId, nxtId,
								stamp);
					} else {
						break;
					}
					i++;
				} else {
					break;
				}
			}
			printf("----Page end---\n");
			page = page->nxtpage;
		}
	}
}

// search one page that contain the specified Id.
ta_page_t* searchPage(long long id, int recordbytes, long long startbyte,
		int pagererecords, int pagebytes, FILE *tadbfp) {
	ta_page_t *nxtp = findPage(id);
	if (nxtp == NULL) {
		long long pagenum = (id * recordbytes) / pagebytes;
		long long start = pagenum * pagebytes + startbyte;
		nxtp = readOnePage(start, pagenum * pagererecords, tadbfp);
	}
	return nxtp;
}

// search a time stamp in memory pages for inserting action
void searchforInsert(long long ts, unsigned char *currpos, ta_page_t *currpage,
		evolved_point_t *tmp, evolved_point_t *previous, evolved_point_t *next,
		tadb_head_t *h, FILE *tadbfp) {
	int recordbytes = 3 * LONG_LONG + 1; // byte counting in one record
	int pagererecords = 10LL; // record counting in one page
	int pagebytes = recordbytes * pagererecords; // byte counting in one page
	long long startbyte = 16LL; // start points in database
	// current record, it may be previous/next record
	long long prvId = parsePrvId(currpos);
	long long nxtId = parseNxtId(currpos);
	long long stamp = parseStamp(currpos);
	if (stamp == 0) {	// this is an empty DB,
		// so this time stamp will be stored here
		long long id = getOneId();
		tmp->id = id; //
		tmp->pos = currpos;
		tmp->nxtTsId = NULL_POINTER;
		tmp->prvTsId = NULL_POINTER;

		h->first = id;
		h->firstdirty = 1;
		h->last = id;
		h->lastdirty = 1;
		return;
	} else if (stamp != 0) {
		if (ts < stamp) { // search backwards
			while (currpage != NULL) {
				// current record, it may be previous/next record
				unsigned char inuse = parseInUse(currpos);
				long long prvId = parsePrvId(currpos);
				long long nxtId = parseNxtId(currpos);
				long long stamp = parseStamp(currpos);
				// tmp will be the next record of the current record
				if (prvId == NULL_POINTER) { // the first record
					if (inuse == 1) {
						if (ts < stamp) { // the ts will be the first record
							long long id = getOneId();
							tmp->id = id; //
							tmp->prvTsId = NULL_POINTER;
							tmp->nxtTsId = timeaxispages->first;

							next->prvTsId = id; // the ts will be the second
							next->pos = currpos;
							next->page = currpage;

							h->first = id;
							h->firstdirty = 1;
							return;
						} else if (ts > stamp) { // ts will be the second.
							long long id = getOneId();
							tmp->id = id; //
							tmp->prvTsId = timeaxispages->first;
							tmp->nxtTsId = nxtId;

							previous->id = timeaxispages->first;
							previous->nxtTsId = id;
							previous->pos = currpos;
							previous->page = currpage;

							ta_page_t *nxtp = searchPage(nxtId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *npos = nxtp->content
									+ (nxtId - nxtp->startNo) * recordbytes;
							next->id = nxtId;
							next->prvTsId = id;
							next->pos = npos;
							next->page = nxtp;
							return;
						} else if (ts == stamp) { //
							tmp->id = timeaxispages->first;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;
							return;
						}
					} else {
						currpos += recordbytes;
						continue;
					}
				} else {
					if (inuse == 1) {
						if (ts < stamp) {
							// lookup backwards
							long long prv = currpage->start
									+ (prvId - currpage->startNo) * recordbytes;
							if (prv >= currpage->start && prv < currpage->end) { // in this page
								currpos = currpage->content
										+ (prvId - currpage->startNo)
												* recordbytes;
							} else { // not in this page
								currpage = searchPage(prvId, recordbytes,
										startbyte, pagererecords, pagebytes,
										tadbfp);
								currpos = currpage->content
										+ (prvId - currpage->startNo)
												* recordbytes;
							}
							continue;
						} else if (ts > stamp) {
							ta_page_t *prvp = searchPage(prvId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *cpos = prvp->content
									+ (prvId - prvp->startNo) * recordbytes;
							long long curId = parseNxtId(cpos);

							long long id = getOneId();
							tmp->id = id; //
							tmp->prvTsId = curId; //
							tmp->nxtTsId = nxtId;

							previous->id = curId;
							previous->nxtTsId = id;
							previous->pos = currpos;
							previous->page = currpage;

							ta_page_t *nxtp = searchPage(nxtId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *npos = nxtp->content
									+ (nxtId - nxtp->startNo) * recordbytes;
							next->id = nxtId;
							next->prvTsId = id; //
							next->pos = npos;
							next->page = nxtp;
							return;
						} else if (ts == stamp) {
							ta_page_t *nxtp = searchPage(nxtId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *npos = nxtp->content
									+ (nxtId - nxtp->startNo) * recordbytes;
							tmp->id = parsePrvId(npos);
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;
							return;
						}
					} else {
						currpos += recordbytes;
						continue;
					}
				}
			}
		} else if (ts > stamp) { // search forwards
			while (currpage != NULL) {
				// current record, it may be previous/next record
				unsigned char inuse = parseInUse(currpos);
				long long prvId = parsePrvId(currpos);
				long long nxtId = parseNxtId(currpos);
				long long stamp = parseStamp(currpos);
				if (nxtId == NULL_POINTER) { // this is last record
					if (inuse == 1) {
						if (ts > stamp) { // the ts will last one
							long long id = getOneId();
							tmp->id = id; //
							tmp->prvTsId = timeaxispages->last;
							tmp->nxtTsId = NULL_POINTER;

							previous->id = timeaxispages->last;
							previous->nxtTsId = id; //
							previous->pos = currpos;
							previous->page = currpage;

							h->last = id;
							h->lastdirty = 1;
							return;
						} else if (ts < stamp) { // the ts will the second last.
							long long id = getOneId();
							tmp->id = id; //
							tmp->prvTsId = prvId;
							tmp->nxtTsId = timeaxispages->last;

							ta_page_t *prvp = searchPage(prvId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *ppos = prvp->content
									+ (prvId - prvp->startNo) * recordbytes;
							previous->id = prvId;
							previous->nxtTsId = id; //
							previous->pos = ppos;
							previous->page = prvp;

							next->id = timeaxispages->last;
							next->prvTsId = id; //
							next->pos = currpos;
							next->page = currpage;
							return;
						} else if (ts == stamp) {
							tmp->id = timeaxispages->last;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;
							return;
						}
					} else {
						currpos -= recordbytes;
						continue;
					}
				} else {
					if (inuse == 1) {
						if (ts > stamp) {
							long long nxt = currpage->start
									+ (nxtId - currpage->startNo) * recordbytes;
							if (nxt >= currpage->start && nxt < currpage->end) { // in this page
								currpos = currpage->content
										+ (nxtId - currpage->startNo)
												* recordbytes;
							} else { // not in this page
								currpage = searchPage(nxtId, recordbytes,
										startbyte, pagererecords, pagebytes,
										tadbfp);
								currpos = currpage->content
										+ (nxtId - currpage->startNo)
												* recordbytes;
							}
							continue;
						} else if (ts < stamp) {
							ta_page_t *nxtp = searchPage(nxtId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *cpos = nxtp->content
									+ (nxtId - nxtp->startNo) * recordbytes;
							long long curId = parsePrvId(cpos);
							long long id = getOneId();
							tmp->id = id; //
							tmp->prvTsId = prvId; //
							tmp->nxtTsId = curId;

							next->id = curId; // this is current record
							next->prvTsId = id;
							next->pos = currpos;
							next->page = currpage;

							ta_page_t *prvp = searchPage(prvId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *ppos = prvp->content
									+ (prvId - prvp->startNo) * recordbytes;
							previous->id = prvId;
							previous->nxtTsId = id; //
							previous->pos = ppos;
							previous->page = prvp;

							return;
						} else if (ts == stamp) {
							ta_page_t *nxtp = searchPage(nxtId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *npos = nxtp->content
									+ (nxtId - nxtp->startNo) * recordbytes;
							tmp->id = parsePrvId(npos);
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;
							return;
						}
					} else {
						currpos += recordbytes;
						continue;
					}
				}
			}
		} else if (ts == stamp) {
			// tmp is the current record
			ta_page_t *nxtpaage = searchPage(nxtId, recordbytes, startbyte,
					pagererecords, pagebytes, tadbfp);
			unsigned char *nxtpos = nxtpaage->content
					+ (nxtId - nxtpaage->startNo) * recordbytes;
			long long curId = parsePrvId(nxtpos);
			tmp->id = curId;
			tmp->time = ts;
			tmp->prvTsId = prvId;
			tmp->nxtTsId = nxtId;
			tmp->pos = currpos;
			tmp->page = currpage;
			return;
		}
	}
}

void searchforDelete(long long ts, unsigned char *currpos, ta_page_t *currpage,
		evolved_point_t *tmp, evolved_point_t *previous, evolved_point_t *next,
		tadb_head_t *h, FILE *tadbfp) {
	int recordbytes = 3 * LONG_LONG + 1; // byte counting in one record
	int pagererecords = 10LL; // record counting in one page
	int pagebytes = recordbytes * pagererecords; // byte counting in one page
	long long startbyte = 16LL; // start points in database
	// current record, it may be previous/next record
	long long prvId = parsePrvId(currpos);
	long long nxtId = parseNxtId(currpos);
	long long stamp = parseStamp(currpos);
	if (stamp == 0) {	// this is an empty DB,
		// so this time stamp will be stored here
		tmp->id = NULL_POINTER; //
		return;
	} else if (stamp != 0) {
		if (ts < stamp) { // search backwards
			while (currpage != NULL) {
				// current record, it may be previous/next record
				unsigned char inuse = parseInUse(currpos);
				long long prvId = parsePrvId(currpos);
				long long nxtId = parseNxtId(currpos);
				long long stamp = parseStamp(currpos);
				// tmp will be the next record of the current record
				if (prvId == NULL_POINTER) { // the first record
					if (inuse == 1) {
						if (ts != stamp) { // the ts will be the first record
							tmp->id = NULL_POINTER; // not found
							return;
						} else if (ts == stamp) { // ts is same as the first record
							tmp->id = timeaxispages->first;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;
							// get the second record
							ta_page_t *nxtp = searchPage(nxtId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *npos = nxtp->content
									+ (nxtId - nxtp->startNo) * recordbytes;
							next->id = nxtId;
							next->prvTsId = NULL_POINTER;
							next->pos = npos;
							next->page = nxtp;

							h->first = nxtId;
							h->firstdirty = 1;
							return;
						}
					} else {
						currpos += recordbytes;
						continue;
					}
				} else {
					if (inuse == 1) {
						if (ts < stamp) {
							// lookup backwards
							long long prv = currpage->start
									+ (prvId - currpage->startNo) * recordbytes;
							if (prv >= currpage->start && prv < currpage->end) { // in this page
								currpos = currpage->content
										+ (prvId - currpage->startNo)
												* recordbytes;
							} else { // not in this page
								currpage = searchPage(prvId, recordbytes,
										startbyte, pagererecords, pagebytes,
										tadbfp);
								currpos = currpage->content
										+ (prvId - currpage->startNo)
												* recordbytes;
							}
							continue;
						} else if (ts > stamp) {
							tmp->id = NULL_POINTER; // not found
							return;
						} else if (ts == stamp) {
							ta_page_t *prvp = searchPage(prvId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *ppos = prvp->content
									+ (prvId - prvp->startNo) * recordbytes;
							long long curId = parseNxtId(ppos);

							tmp->id = curId;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;

							previous->id = prvId;
							previous->nxtTsId = curId;
							previous->pos = ppos;
							previous->page = prvp;

							ta_page_t *nxtp = searchPage(nxtId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *npos = nxtp->content
									+ (nxtId - nxtp->startNo) * recordbytes;
							next->id = nxtId;
							next->prvTsId = curId; //
							next->pos = npos;
							next->page = nxtp;

							return;
						}
					} else {
						currpos += recordbytes;
						continue;
					}
				}
			}
		} else if (ts > stamp) { // search forwards
			while (currpage != NULL) {
				// current record, it may be previous/next record
				unsigned char inuse = parseInUse(currpos);
				long long prvId = parsePrvId(currpos);
				long long nxtId = parseNxtId(currpos);
				long long stamp = parseStamp(currpos);
				if (nxtId == NULL_POINTER) { // this is last record
					if (inuse == 1) {
						if (ts != stamp) { // the ts will last one
							tmp->id = NULL_POINTER; // not found
							return;
						} else if (ts == stamp) { // found
							tmp->id = timeaxispages->last;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;
							// get the last second record
							ta_page_t *prvp = searchPage(prvId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *ppos = prvp->content
									+ (prvId - prvp->startNo) * recordbytes;
							previous->id = prvId;
							previous->nxtTsId = NULL_POINTER; //
							previous->pos = ppos;
							previous->page = prvp;

							h->last = prvId;
							h->lastdirty = 1;
							return;
						}
					} else {
						currpos -= recordbytes;
						continue;
					}
				} else {
					if (inuse == 1) {
						if (ts > stamp) {
							long long nxt = currpage->start
									+ (nxtId - currpage->startNo) * recordbytes;
							if (nxt >= currpage->start && nxt < currpage->end) { // in this page
								currpos = currpage->content
										+ (nxtId - currpage->startNo)
												* recordbytes;
							} else { // not in this page
								currpage = searchPage(nxtId, recordbytes,
										startbyte, pagererecords, pagebytes,
										tadbfp);
								currpos = currpage->content
										+ (nxtId - currpage->startNo)
												* recordbytes;
							}
							continue;
						} else if (ts < stamp) {
							tmp->id = NULL_POINTER; // not found
							return;
						} else if (ts == stamp) { // found
							ta_page_t *prvp = searchPage(prvId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *ppos = prvp->content
									+ (prvId - prvp->startNo) * recordbytes;
							long long curId = parseNxtId(ppos);

							tmp->id = curId;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;

							previous->id = prvId;
							previous->nxtTsId = curId; //
							previous->pos = ppos;
							previous->page = prvp;

							ta_page_t *nxtp = searchPage(nxtId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *npos = nxtp->content
									+ (nxtId - nxtp->startNo) * recordbytes;
							next->id = nxtId; // this is current record
							next->prvTsId = curId;
							next->pos = npos;
							next->page = nxtp;

							return;
						}
					} else {
						currpos += recordbytes;
						continue;
					}
				}
			}
		} else if (ts == stamp) {
			// tmp is the current record
			ta_page_t *nxtpaage = searchPage(nxtId, recordbytes, startbyte,
					pagererecords, pagebytes, tadbfp);
			unsigned char *nxtpos = nxtpaage->content
					+ (nxtId - nxtpaage->startNo) * recordbytes;
			long long curId = parsePrvId(nxtpos);
			tmp->id = curId;
			tmp->time = ts;
			tmp->prvTsId = prvId;
			tmp->nxtTsId = nxtId;
			tmp->pos = currpos;
			tmp->page = currpage;

			ta_page_t *prvp = searchPage(prvId, recordbytes,
					startbyte, pagererecords, pagebytes,
					tadbfp);
			unsigned char *ppos = prvp->content
					+ (prvId - prvp->startNo) * recordbytes;

			previous->id = prvId;
			previous->nxtTsId = curId; //
			previous->pos = ppos;
			previous->page = prvp;

			ta_page_t *nxtp = searchPage(nxtId, recordbytes,
					startbyte, pagererecords, pagebytes,
					tadbfp);
			unsigned char *npos = nxtp->content
					+ (nxtId - nxtp->startNo) * recordbytes;
			next->id = nxtId; // this is current record
			next->prvTsId = curId;
			next->pos = npos;
			next->page = nxtp;

			return;
		}
	}
}


long long commitEvolvedPoint(long long ts, evolved_point_t *p,
		evolved_point_t *t, evolved_point_t *n, tadb_head_t *h, int recordbytes,
		long long startbyte, int pagererecords, int pagebytes, FILE *tadbfp) {
	ta_page_t *newp = searchPage(t->id, recordbytes, startbyte, pagererecords,
			pagebytes, tadbfp);
	t->pos = newp->content + (t->id - newp->startNo) * recordbytes;
	if (t->time != ts) { // found a position
		// insert the evolved point at the position
		t->time = ts;
		putInUse(t);
		putPrvId(t);
		putNxtId(t);
		putStamp(t);
		newp->dirty = 1;
		newp->hit++;
	} else {
		printf("%lld\n", t->id);
	}

	// update previous evolved point
	if (p->prvTsId != -3)
		putPrvId(p);
	if (p->nxtTsId != -3)
		putNxtId(p);
	if (p->prvTsId != -3 || p->nxtTsId != -3) {
		p->page->dirty = 1;
		p->page->hit++;
	}
	// update next evolved point
	if (n->prvTsId != -3)
		putPrvId(n);
	if (n->nxtTsId != -3)
		putNxtId(n);
	if (n->prvTsId != -3 || n->nxtTsId != -3) {
		n->page->dirty = 1;
		n->page->hit++;
	}
	if (h->firstdirty == 1) {
		timeaxispages->first = h->first;
	}
	if (h->lastdirty == 1) {
		//
		timeaxispages->last = h->last;
	}
	return t->id;
}

void deleteEvolvedPoint(long long ts, FILE *taidfp, FILE *tadbfp) {
	int recordbytes = 3LL * LONG_LONG + 1LL; // byte counting in one record
	int pagererecords = 10LL; // record counting in one page
	int pagebytes = recordbytes * pagererecords; // byte counting in one page
	long long startbyte = 16LL; // start points in database
	ta_page_t *pp = timeaxispages->pages;
	tadb_head_t *h = (tadb_head_t*) malloc(sizeof(tadb_head_t));
	h->first = -3;
	h->firstdirty = 0;
	h->last = -3;
	h->lastdirty = 0;
	if (pp != NULL) {
		// p, n, t are temporal variables:
		// previous, next, temporal
		evolved_point_t *t = (evolved_point_t*) malloc(sizeof(evolved_point_t));
		t->inuse = 1;
		t->prvTsId = -3;
		t->nxtTsId = -3;
		t->time = 0;
		t->id = -3;
		evolved_point_t *p = (evolved_point_t*) malloc(sizeof(evolved_point_t));
		p->inuse = 1;
		p->prvTsId = -3;
		p->nxtTsId = -3;
		p->time = 0;
		p->id = -3;
		evolved_point_t *n = (evolved_point_t*) malloc(sizeof(evolved_point_t));
		n->inuse = 1;
		n->prvTsId = -3;
		n->nxtTsId = -3;
		n->time = 0;
		n->id = -3;

		//search(ts, pp->content, pp, t, p, n, tadbfp);

	}
}

long long insertEvolvedPoint(long long ts, FILE *taidfp, FILE *tadbfp) {
	int recordbytes = 3LL * LONG_LONG + 1LL; // byte counting in one record
	int pagererecords = 10LL; // record counting in one page
	int pagebytes = recordbytes * pagererecords; // byte counting in one page
	long long startbyte = 16LL; // start points in database
	ta_page_t *pp = timeaxispages->pages;
	long long id = -1;
	tadb_head_t *h = (tadb_head_t*) malloc(sizeof(tadb_head_t));
	h->first = -3;
	h->firstdirty = 0;
	h->last = -3;
	h->lastdirty = 0;
	if (pp != NULL) {
		// p, n, t are temporal variables:
		// previous, next, temporal
		evolved_point_t *t = (evolved_point_t*) malloc(sizeof(evolved_point_t));
		t->inuse = 1;
		t->prvTsId = -3;
		t->nxtTsId = -3;
		t->time = 0;
		t->id = -3;
		evolved_point_t *p = (evolved_point_t*) malloc(sizeof(evolved_point_t));
		p->inuse = 1;
		p->prvTsId = -3;
		p->nxtTsId = -3;
		p->time = 0;
		p->id = -3;
		evolved_point_t *n = (evolved_point_t*) malloc(sizeof(evolved_point_t));
		n->inuse = 1;
		n->prvTsId = -3;
		n->nxtTsId = -3;
		n->time = 0;
		n->id = -3;

		searchforInsert(ts, pp->content, pp, t, p, n, h, tadbfp);

		id = commitEvolvedPoint(ts, p, t, n, h, recordbytes, startbyte,
				pagererecords, pagebytes, tadbfp);

		free(t);
		free(p);
		free(n);
	}
	return id;
}

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

	loadIds(taidfp);
	listAllTaIds();

	timeaxispages = (ta_buf_t*) malloc(sizeof(ta_buf_t));
	timeaxispages->pages = NULL;
	unsigned char p[LONG_LONG * 2] = { 0 };
	unsigned char first[LONG_LONG] = { 0 };
	unsigned char last[LONG_LONG] = { 0 };
	fseek(tadbfp, 0, SEEK_SET); //
	fread(p, sizeof(unsigned char), LONG_LONG * 2, tadbfp);
	for (int i = 0; i < LONG_LONG; i++) {
		first[i] = p[i];
	}
	for (int i = LONG_LONG; i < 2 * LONG_LONG; i++) {
		last[i] = p[i];
	}
	long long firstId = ByteArrayToLong(first);
	long long lastId = ByteArrayToLong(last);
	timeaxispages->first = firstId;
	timeaxispages->last = lastId;
	if (timeaxispages->pages == NULL) {
		readOnePage(16LL, 0, tadbfp);
	}

	// insert 13 time stamp sequentially
	//1593783935
	//1593783957
	long long ts = 1593783935;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts, taidfp, tadbfp);
	//showAllPages();
	long long ts1 = 1593783957;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts1, taidfp, tadbfp);
	//showAllPages();
	long long ts2 = 1593783958;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts2, taidfp, tadbfp);
	//showAllPages();
	long long ts3 = 1593783959;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts3, taidfp, tadbfp);
	//showAllPages();
	long long ts4 = 1593783960;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts4, taidfp, tadbfp);
	long long ts5 = 1593783963;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts5, taidfp, tadbfp);
	long long ts6 = 1593783964;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts6, taidfp, tadbfp);
	long long ts7 = 1593783965;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts7, taidfp, tadbfp);
	long long ts8 = 1593783969;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts8, taidfp, tadbfp);
	long long ts9 = 1593783970;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts9, taidfp, tadbfp);
	showAllPages();
	long long ts10 = 1593783972;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts10, taidfp, tadbfp);
	showAllPages();
	long long ts11 = 1593783974;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts11, taidfp, tadbfp);
	showAllPages();

	long long ts12 = 1593783975;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts12, taidfp, tadbfp);

	long long ts13 = 1593783977;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts13, taidfp, tadbfp);
	showAllPages();

	// insert 2 time stamps before the first time stamp
	long long ts14 = 1593783932;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts14, taidfp, tadbfp);
	showAllPages();
	long long ts15 = 1593783928;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts15, taidfp, tadbfp);
	showAllPages();

	// insert 2 time stamps in the middle
	long long ts16 = 1593783961;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts16, taidfp, tadbfp);
	showAllPages();
	long long ts17 = 1593783973;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts17, taidfp, tadbfp);
	showAllPages();

	long long ts18 = 1593783976;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts18, taidfp, tadbfp);
	showAllPages();

	long long ts19 = 1593783931;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts19, taidfp, tadbfp);
	showAllPages();

	long long ts20 = 1593783931;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts20, taidfp, tadbfp);
	showAllPages();

	long long ts21 = 1593783977;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts21, taidfp, tadbfp);
	showAllPages();

	listAllTaIds();
	// commitEvolvedPoint(ep, tadbfp);

	fclose(taidfp);
	fclose(tadbfp);

	free(timeaxispages);
	free(cache);

}

