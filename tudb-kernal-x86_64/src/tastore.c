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

void EpToByteArray(evolved_point_t *ep, int len, unsigned char buffer[]) {
	memset(buffer, 0, len);
	buffer[0] = ep->inuse;
	for (int i = 1; i < LONG_LONG + 1; i++) {
		buffer[i] = ((ep->prvTsId >> (LONG_LONG * i)) & 0XFF);
	}
	for (int i = LONG_LONG + 1; i < 1 * LONG_LONG + 1; i++) {
		buffer[i] = ((ep->nxtTsId >> (LONG_LONG * i)) & 0XFF);
	}
	for (int i = 2 * LONG_LONG + 1; i < 3 * LONG_LONG + 1; i++) {
		buffer[i] = ((ep->time >> (LONG_LONG * i)) & 0XFF);
	}

}

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

// query one evolved point by specified time stamp (ts)
void searchforQueryEP(long long ts, unsigned char *currpos, ta_page_t *currpage,
		evolved_point_t *tmp, evolved_point_t *previous, evolved_point_t *next,
		FILE *tadbfp) {
	int recordbytes = 3 * LONG_LONG + 1; // byte counting in one record
	int pagererecords = 10LL; // record counting in one page
	int pagebytes = recordbytes * pagererecords; // byte counting in one page
	long long startbyte = 16LL; // start points in database
	// current record, it may be previous/next record
	long long prvId = parsePrvId(currpos);
	long long nxtId = parseNxtId(currpos);
	long long stamp = parseStamp(currpos);
	if (stamp == 0) {	// this is an empty DB,
		tmp->id = NULL_POINTER; //
		return;
	} else if (stamp != 0) {
		if (ts < stamp) { // search backwards
			while (currpage != NULL) {
				unsigned char inuse = parseInUse(currpos);
				long long prvId = parsePrvId(currpos);
				long long nxtId = parseNxtId(currpos);
				long long stamp = parseStamp(currpos);
				if (inuse == 1) {
					if (prvId == NULL_POINTER) {
						if (ts != stamp) { // not found
							tmp->id = NULL_POINTER;
							return;
						} else if (ts == stamp) { // ts is just the first
							tmp->id = timeaxispages->first;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;
							return;
						}
					} else {
						if (ts < stamp) {
							next->prvTsId = prvId; //
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
							tmp->id = next->prvTsId;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;
							return;
						}
					}
				} else {
					currpos += recordbytes;
					continue;
				}
			}
		} else if (ts > stamp) { // search forwards
			while (currpage != NULL) {
				// current record, it may be previous/next record
				unsigned char inuse = parseInUse(currpos);
				long long prvId = parsePrvId(currpos);
				long long nxtId = parseNxtId(currpos);
				long long stamp = parseStamp(currpos);
				if (inuse == 1) {
					if (nxtId == NULL_POINTER) { // this is last record
						if (ts != stamp) {
							tmp->id = NULL_POINTER; // not found
							return;
						} else if (ts == stamp) { // ts is just the last
							tmp->id = timeaxispages->last;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;
							return;
						}
					} else {
						if (ts > stamp) {
							previous->nxtTsId = nxtId; //
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
							tmp->id = previous->nxtTsId;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;
							return;
						}
					}
				} else {
					currpos += recordbytes;
					continue;
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

// query one or more evolved points between specified minimum time stamp (mints)
// maximum time stamp (maxts), mints should be less than maxts.
void searchforQueryBetween(long long mints, long long maxts, idbuf_t *buf,
		unsigned char *currpos, ta_page_t *currpage, evolved_point_t *tmp,
		evolved_point_t *previous, evolved_point_t *next, FILE *tadbfp) {
	if (mints > maxts) {
		return;
	} else if (mints == maxts) {
		searchforQueryEP(mints, timeaxispages->pages->content,
				timeaxispages->pages, tmp, previous, next, tadbfp);
		if (tmp->id != NULL_POINTER) {
			idbuf_t *bf = (idbuf_t*) malloc(sizeof(idbuf_t));
			bf->id = tmp->id;
			bf->nxt = NULL;
			buf = bf;
		}
		return;
	}
	int recordbytes = 3 * LONG_LONG + 1; // byte counting in one record
	int pagererecords = 10LL; // record counting in one page
	int pagebytes = recordbytes * pagererecords; // byte counting in one page
	long long startbyte = 16LL; // start points in database
	// current record, it may be previous/next record
	long long prvId = parsePrvId(currpos);
	long long nxtId = parseNxtId(currpos);
	long long stamp = parseStamp(currpos);
	if (stamp == 0) {	// this is an empty DB,
		tmp->id = NULL_POINTER; //
		return;
	} else if (stamp != 0) {
		if (maxts <= stamp) { // search backwards
			while (currpage != NULL) {
				unsigned char inuse = parseInUse(currpos);
				long long prvId = parsePrvId(currpos);
				long long nxtId = parseNxtId(currpos);
				long long stamp = parseStamp(currpos);
				if (inuse == 1) {
					if (prvId == NULL_POINTER) {
						if (maxts >= stamp) {
							idbuf_t *bf = (idbuf_t*) malloc(sizeof(idbuf_t));
							bf->id = next->prvTsId;
							bf->nxt = NULL;
							if (buf == NULL)
								buf = bf;
							else {
								bf->nxt = buf; // insert into head
								buf = bf;
							}
						}
						return;
					} else {
						if (mints <= stamp) {
							if (maxts >= stamp) {
								idbuf_t *bf = (idbuf_t*) malloc(
										sizeof(idbuf_t));
								bf->id = next->prvTsId;
								bf->nxt = NULL;
								if (buf == NULL)
									buf = bf;
								else {
									bf->nxt = buf;
									buf = bf;
								}
							}
							next->prvTsId = prvId; //
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
						} else if (mints > stamp) {
							return;
						}
					}
				} else {
					currpos += recordbytes;
					continue;
				}
			}
		} else if (mints >= stamp) { // search forwards
			while (currpage != NULL) {
				// current record, it may be previous/next record
				unsigned char inuse = parseInUse(currpos);
				long long prvId = parsePrvId(currpos);
				long long nxtId = parseNxtId(currpos);
				long long stamp = parseStamp(currpos);
				if (inuse == 1) {
					if (nxtId == NULL_POINTER) { // this is last record
						if (maxts >= stamp) {
							if (buf->id == NULL_POINTER && buf->nxt == NULL) {
								buf->id = previous->nxtTsId;
							} else {
								idbuf_t *p = buf;
								while (p->nxt != NULL) {
									p = p->nxt;
								}
								idbuf_t *bf = (idbuf_t*) malloc(
										sizeof(idbuf_t));
								bf->id = previous->nxtTsId;
								bf->nxt = NULL;
								p->nxt = bf;
							}
						}
						return;
					} else {
						if (maxts >= stamp) {
							if (mints <= stamp) {
								if (buf->id == NULL_POINTER && buf->nxt == NULL) {
									buf->id = previous->nxtTsId;
								} else {
									idbuf_t *p = buf;
									while (p->nxt != NULL) {
										p = p->nxt;
									}
									idbuf_t *bf = (idbuf_t*) malloc(
											sizeof(idbuf_t));
									bf->id = previous->nxtTsId;
									bf->nxt = NULL;
									p->nxt = bf;
								}
							}
							previous->nxtTsId = nxtId; //
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
						} else if (maxts < stamp) {
							return;
						}
					}
				} else {
					currpos += recordbytes;
					continue;
				}
			}
		} else if (mints <= stamp && maxts >= stamp) {
			unsigned char *cpos = currpos;
			ta_page_t *cpage = currpage;
			while (cpage != NULL) {
				unsigned char inuse = parseInUse(cpos);
				long long prvId = parsePrvId(cpos);
				long long nxtId = parseNxtId(cpos);
				long long stamp = parseStamp(cpos);
				if (inuse == 1) {
					if (prvId == NULL_POINTER) {
						idbuf_t *bf = (idbuf_t*) malloc(sizeof(idbuf_t));
						bf->id = next->prvTsId;
						bf->nxt = NULL;
						if (buf == NULL)
							buf = bf;
						else {
							bf->nxt = buf; // insert into head
							buf = bf;
						}
						break;
					} else {
						if (mints <= stamp) {
							idbuf_t *bf = (idbuf_t*) malloc(sizeof(idbuf_t));
							bf->id = next->prvTsId;
							bf->nxt = NULL;
							if (buf == NULL)
								buf = bf;
							else {
								bf->nxt = buf;
								buf = bf;
							}
							// lookup backwards
							next->prvTsId = prvId; //
							long long prv = cpage->start
									+ (prvId - cpage->startNo) * recordbytes;
							if (prv >= cpage->start && prv < cpage->end) { // in this page
								cpos = cpage->content
										+ (prvId - cpage->startNo)
												* recordbytes;
							} else { // not in this page
								cpage = searchPage(prvId, recordbytes,
										startbyte, pagererecords, pagebytes,
										tadbfp);
								cpos = cpage->content
										+ (prvId - cpage->startNo)
												* recordbytes;
							}
							continue;
						} else if (mints > stamp) {
							break;
						}
					}
				} else {
					cpos += recordbytes;
					continue;
				}
			}
			cpos = currpos;
			cpage = currpage;
			while (cpage != NULL) {
				// current record, it may be previous/next record
				unsigned char inuse = parseInUse(cpos);
				long long prvId = parsePrvId(cpos);
				long long nxtId = parseNxtId(cpos);
				long long stamp = parseStamp(cpos);
				if (inuse == 1) {
					if (nxtId == NULL_POINTER) { // this is last record
						if (buf->id == NULL_POINTER && buf->nxt == NULL) {
							buf->id = previous->nxtTsId;
						} else {
							idbuf_t *p = buf;
							while (p->nxt != NULL) {
								p = p->nxt;
							}
							idbuf_t *bf = (idbuf_t*) malloc(sizeof(idbuf_t));
							bf->id = previous->nxtTsId;
							bf->nxt = NULL;
							p->nxt = bf;
						}
					} else {
						if (maxts >= stamp) {
							if (buf->id == NULL_POINTER && buf->nxt == NULL) {
								buf->id = previous->nxtTsId;
							} else {
								idbuf_t *p = buf;
								while (p->nxt != NULL) {
									p = p->nxt;
								}
								idbuf_t *bf = (idbuf_t*) malloc(
										sizeof(idbuf_t));
								bf->id = previous->nxtTsId;
								bf->nxt = NULL;
								p->nxt = bf;
							}
							previous->nxtTsId = nxtId; //
							long long nxt = cpage->start
									+ (nxtId - cpage->startNo) * recordbytes;
							if (nxt >= cpage->start && nxt < cpage->end) { // in this page
								cpos = cpage->content
										+ (nxtId - cpage->startNo)
												* recordbytes;
							} else { // not in this page
								cpage = searchPage(nxtId, recordbytes,
										startbyte, pagererecords, pagebytes,
										tadbfp);
								cpos = cpage->content
										+ (nxtId - cpage->startNo)
												* recordbytes;
							}
							continue;
						} else if (maxts < stamp) {
							break;
						}
					}
				} else {
					cpos += recordbytes;
					continue;
				}
			}
			return;
		}
	}
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
				if (inuse == 1) {
					if (prvId == NULL_POINTER) { // the first record
						if (ts < stamp) { // ts will be the first
							long long id = getOneId();
							tmp->id = id; //
							tmp->prvTsId = NULL_POINTER;
							tmp->nxtTsId = timeaxispages->first;
							tmp->page = NULL;
							tmp->pos = NULL;

							next->id = timeaxispages->first;
							next->prvTsId = id; // the current will be the second
							next->pos = currpos;
							next->page = currpage;

							h->first = id;
							h->firstdirty = 1;
							return;
						} else if (ts > stamp) { // ts will be the second
							long long id = getOneId();
							tmp->id = id; //
							tmp->prvTsId = timeaxispages->first;
							tmp->nxtTsId = nxtId;
							tmp->page = NULL;
							tmp->pos = NULL;

							previous->id = timeaxispages->first;
							previous->nxtTsId = id;
							previous->pos = currpos;
							previous->page = currpage;

							next->id = nxtId;
							next->prvTsId = id;
							return;
						} else if (ts == stamp) { // ts is the first, no actions
							tmp->id = timeaxispages->first;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;
							return;
						}
					} else {
						if (ts < stamp) { // stamp will the next to ts
							next->prvTsId = prvId; //
							next->pos = currpos;
							next->page = currpage;
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
							long long id = getOneId();
							tmp->id = id; //
							tmp->prvTsId = next->prvTsId; //
							tmp->nxtTsId = nxtId; // it is next->id
							tmp->pos = NULL;
							tmp->page = NULL;

							previous->id = next->prvTsId;
							previous->nxtTsId = id;
							previous->pos = currpos;
							previous->page = currpage;

							next->id = nxtId;
							next->prvTsId = id; //
							return;
						} else if (ts == stamp) {
							tmp->id = next->prvTsId;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts; // stamp
							tmp->pos = currpos;
							tmp->page = currpage;
							return;
						}
					}
				} else {
					currpos += recordbytes;
					continue;
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
						if (ts > stamp) { // ts will the last
							long long id = getOneId();
							tmp->id = id; //
							tmp->prvTsId = timeaxispages->last;
							tmp->nxtTsId = NULL_POINTER;
							tmp->pos = NULL;
							tmp->page = NULL;

							previous->id = timeaxispages->last;
							previous->nxtTsId = id; //
							previous->pos = currpos;
							previous->page = currpage;

							h->last = id;
							h->lastdirty = 1;
							return;
						} else if (ts < stamp) { // ts will the second last.
							long long id = getOneId();
							tmp->id = id; //
							tmp->prvTsId = prvId;
							tmp->nxtTsId = timeaxispages->last;
							tmp->page = NULL;
							tmp->pos = NULL;

							previous->id = prvId;
							previous->nxtTsId = id; //

							next->id = nxtId;
							next->prvTsId = id;
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
							previous->nxtTsId = nxtId; //
							previous->pos = currpos;
							previous->page = currpage;

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
							long long id = getOneId();
							tmp->id = id; //
							tmp->prvTsId = prvId; //
							tmp->nxtTsId = previous->nxtTsId;
							tmp->pos = NULL;
							tmp->page = NULL;

							next->id = previous->nxtTsId;
							next->prvTsId = id;
							next->pos = currpos;
							next->page = currpage;

							previous->id = prvId;
							previous->nxtTsId = id; //

							return;
						} else if (ts == stamp) {
							tmp->id = previous->nxtTsId;
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
		} else if (ts == stamp) { // found
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
				if (inuse == 1) {
					if (prvId == NULL_POINTER) {
						if (ts != stamp) { // not found
							tmp->id = NULL_POINTER;
							return;
						} else if (ts == stamp) { // ts is just the first
							tmp->id = timeaxispages->first;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;

							next->id = nxtId;
							next->prvTsId = NULL_POINTER;

							h->first = nxtId;
							h->firstdirty = 1;
							return;
						}
					} else {
						if (ts < stamp) {
							next->prvTsId = prvId; //
							next->pos = currpos;
							next->page = currpage;
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

							tmp->id = next->prvTsId;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;

							ta_page_t *prvp = searchPage(prvId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *ppos = prvp->content
									+ (prvId - prvp->startNo) * recordbytes;
							long long curId = parseNxtId(ppos);

							previous->id = prvId;
							previous->nxtTsId = nxtId;
							previous->pos = ppos;
							previous->page = prvp;

							next->id = nxtId;
							next->prvTsId = prvId; //

							return;
						}
					}
				} else {
					currpos += recordbytes;
					continue;
				}
			}
		} else if (ts > stamp) { // search forwards
			while (currpage != NULL) {
				// current record, it may be previous/next record
				unsigned char inuse = parseInUse(currpos);
				long long prvId = parsePrvId(currpos);
				long long nxtId = parseNxtId(currpos);
				long long stamp = parseStamp(currpos);
				if (inuse == 1) {
					if (nxtId == NULL_POINTER) { // this is last record
						if (ts != stamp) {
							tmp->id = NULL_POINTER; // not found
							return;
						} else if (ts == stamp) { // ts is just the last
							tmp->id = timeaxispages->last;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;

							previous->id = prvId;
							previous->nxtTsId = NULL_POINTER; //

							h->last = prvId;
							h->lastdirty = 1;
							return;
						}
					} else {
						if (ts > stamp) {
							previous->nxtTsId = nxtId; //
							previous->pos = currpos;
							previous->page = currpage;

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
							tmp->id = previous->nxtTsId;
							tmp->prvTsId = prvId;
							tmp->nxtTsId = nxtId;
							tmp->time = ts;
							tmp->pos = currpos;
							tmp->page = currpage;

							previous->id = prvId;
							previous->nxtTsId = nxtId; //

							ta_page_t *nxtp = searchPage(nxtId, recordbytes,
									startbyte, pagererecords, pagebytes,
									tadbfp);
							unsigned char *npos = nxtp->content
									+ (nxtId - nxtp->startNo) * recordbytes;
							next->id = nxtId;
							next->prvTsId = prvId;
							next->pos = npos;
							next->page = nxtp;

							return;
						}
					}
				} else {
					currpos += recordbytes;
					continue;
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

			ta_page_t *prvp = searchPage(prvId, recordbytes, startbyte,
					pagererecords, pagebytes, tadbfp);
			unsigned char *ppos = prvp->content
					+ (prvId - prvp->startNo) * recordbytes;
			previous->id = prvId;
			previous->nxtTsId = nxtId;
			previous->pos = ppos;
			previous->page = prvp;

			ta_page_t *nxtp = searchPage(nxtId, recordbytes, startbyte,
					pagererecords, pagebytes, tadbfp);
			unsigned char *npos = nxtp->content
					+ (nxtId - nxtp->startNo) * recordbytes;
			next->id = nxtId;
			next->prvTsId = prvId;
			next->pos = npos;
			next->page = nxtp;

			return;
		}
	}
}

long long commitInsert(long long ts, evolved_point_t *p, evolved_point_t *t,
		evolved_point_t *n, tadb_head_t *h, int recordbytes,
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
		timeaxispages->last = h->last;
	}

	// write to DB file
	if (newp->dirty == 1) {
		unsigned char newta[3 * LONG_LONG + 1] = { 0 };
		void EpToByteArray( t, recordbytes, newta);
		fseek(tadbfp, t->id, SEEK_SET);
		fwrite(newta, sizeof(unsigned char), recordbytes, tadbfp);
		newp->dirty = 0;
	}

	if (p->page->dirty == 1) {
		if (p->prvTsId != -3) {
			unsigned char newprv[LONG_LONG] = { 0 };
			LongToByteArray(p->prvTsId, newprv);
			fseek(tadbfp, (p->id + 1LL), SEEK_SET);
			fwrite(newprv, sizeof(unsigned char), LONG_LONG, tadbfp);
		}
		if (p->nxtTsId != -3) {
			unsigned char newnxt[LONG_LONG] = { 0 };
			LongToByteArray(p->nxtTsId, newnxt);
			fseek(tadbfp, (p->id + 1LL + LONG_LONG), SEEK_SET);
			fwrite(newnxt, sizeof(unsigned char), LONG_LONG, tadbfp);
		}
		p->page->dirty = 0;
	}

	if (n->page->dirty == 1) {
		if (n->prvTsId != -3) {
			unsigned char newprv[LONG_LONG] = { 0 };
			LongToByteArray(n->prvTsId, newprv);
			fseek(tadbfp, (n->id + 1LL), SEEK_SET);
			fwrite(newprv, sizeof(unsigned char), LONG_LONG, tadbfp);
		}
		if (n->nxtTsId != -3) {
			unsigned char newnxt[LONG_LONG] = { 0 };
			LongToByteArray(n->nxtTsId, newnxt);
			fseek(tadbfp, (n->id + 1LL + LONG_LONG), SEEK_SET);
			fwrite(newnxt, sizeof(unsigned char), LONG_LONG, tadbfp);
		}
		n->page->dirty = 0;
	}

	if (h->firstdirty == 1) {
		unsigned char newf[LONG_LONG] = { 0 };
		LongToByteArray(h->first, newf);
		fseek(tadbfp, 0, SEEK_SET);
		fwrite(newf, sizeof(unsigned char), LONG_LONG, tadbfp);
		h->firstdirty = 0;
	}
	if (h->lastdirty == 1) {
		unsigned char newl[LONG_LONG] = { 0 };
		LongToByteArray(h->last, newl);
		fseek(tadbfp, LONG_LONG, SEEK_SET);
		fwrite(newl, sizeof(unsigned char), LONG_LONG, tadbfp);
		h->lastdirty = 0;
	}
	return t->id;
}

long long commitDelete(long long ts, evolved_point_t *p, evolved_point_t *t,
		evolved_point_t *n, tadb_head_t *h, int recordbytes,
		long long startbyte, int pagererecords, int pagebytes, FILE *tadbfp) {
	if (t->id != NULL_POINTER) {
		t->inuse = 0;
		putInUse(t);
		t->page->dirty = 1;
		t->page->hit++;
		recycleOneId(t->id);

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
			timeaxispages->last = h->last;
		}
//
		if (t->page->dirty == 1) {
			unsigned char inuse[1] = { t->inuse };
			fseek(tadbfp, t->id, SEEK_SET);
			fwrite(inuse, sizeof(unsigned char), 1, tadbfp);
			t->page->dirty = 0;
		}
		if (p->page->dirty == 1) {
			if (p->prvTsId != -3) {
				unsigned char newprv[LONG_LONG] = { 0 };
				LongToByteArray(p->prvTsId, newprv);
				fseek(tadbfp, (p->id + 1LL), SEEK_SET);
				fwrite(newprv, sizeof(unsigned char), LONG_LONG, tadbfp);
			}
			if (p->nxtTsId != -3) {
				unsigned char newnxt[LONG_LONG] = { 0 };
				LongToByteArray(p->nxtTsId, newnxt);
				fseek(tadbfp, (p->id + 1LL + LONG_LONG), SEEK_SET);
				fwrite(newnxt, sizeof(unsigned char), LONG_LONG, tadbfp);
			}
			p->page->dirty = 0;
		}

		if (n->page->dirty == 1) {
			if (n->prvTsId != -3) {
				unsigned char newprv[LONG_LONG] = { 0 };
				LongToByteArray(n->prvTsId, newprv);
				fseek(tadbfp, (n->id + 1LL), SEEK_SET);
				fwrite(newprv, sizeof(unsigned char), LONG_LONG, tadbfp);
			}
			if (n->nxtTsId != -3) {
				unsigned char newnxt[LONG_LONG] = { 0 };
				LongToByteArray(n->nxtTsId, newnxt);
				fseek(tadbfp, (n->id + 1LL + LONG_LONG), SEEK_SET);
				fwrite(newnxt, sizeof(unsigned char), LONG_LONG, tadbfp);
			}
			n->page->dirty = 0;
		}

		if (h->firstdirty == 1) {
			unsigned char newf[LONG_LONG] = { 0 };
			LongToByteArray(h->first, newf);
			fseek(tadbfp, 0, SEEK_SET);
			fwrite(newf, sizeof(unsigned char), LONG_LONG, tadbfp);
			h->firstdirty = 0;
		}
		if (h->lastdirty == 1) {
			unsigned char newl[LONG_LONG] = { 0 };
			LongToByteArray(h->last, newl);
			fseek(tadbfp, LONG_LONG, SEEK_SET);
			fwrite(newl, sizeof(unsigned char), LONG_LONG, tadbfp);
			h->lastdirty = 0;
		}
	}
	return t->id;
}

void showAllTimeAxis(FILE *tadbfp) {
	unsigned char buf[3LL * LONG_LONG + 1LL] = { 0 };
	fseek(tadbfp, 0LL, SEEK_END);
	long sz = ftell(tadbfp);
	printf("Db size:%ld bytes\n", sz);
	printf("------------\n");
	unsigned char first[LONG_LONG] = { 0 };
	unsigned char last[LONG_LONG] = { 0 };
	fseek(tadbfp, 0, SEEK_SET);
	fread(first, sizeof(unsigned char), LONG_LONG, tadbfp);
	long long f = ByteArrayToLong(first);
	fseek(tadbfp, LONG_LONG, SEEK_SET);
	printf("First Id:%lld\n", f);
	fread(last, sizeof(unsigned char), LONG_LONG, tadbfp);
	long long l = ByteArrayToLong(last);
	printf("Last Id:%lld\n", l);
	printf("------------\n");
	long long i = 16LL;
	fseek(tadbfp, i, SEEK_SET);
	int c;
	long long prvId = 0LL;
	long long nxtId = 0LL;
	long long ts = 0LL;
	while ((c = fgetc(tadbfp)) != EOF) {
		fseek(tadbfp, i * (3LL * LONG_LONG + 1LL), SEEK_SET);
		memset(buf, 0, 3LL * LONG_LONG + 1LL);
		fread(buf, sizeof(unsigned char), (3LL * LONG_LONG + 1LL), tadbfp);
		unsigned char inuse = buf[0];
		unsigned char prv[LONG_LONG] = { 0L };
		unsigned char nxt[LONG_LONG] = { 0L };
		unsigned char tms[LONG_LONG] = { 0L };
		for (int i = 0; i < LONG_LONG; i++) {
			prv[i] = buf[i + 1];
		}
		for (int i = 0; i < LONG_LONG; i++) {
			nxt[i] = buf[i + LONG_LONG + 1];
		}
		for (int i = 0; i < LONG_LONG; i++) {
			tms[i] = buf[i + 2 * LONG_LONG + 1];
		}
		prvId = ByteArrayToLong(prv);
		nxtId = ByteArrayToLong(nxt);
		ts = ByteArrayToLong(tms);
		if (ts != 0LL) {
			printf("%d %lld|%lld|%lld\n", inuse, prvId, nxtId, ts);
		} else {
			break;
		}
		i++;
	}
	printf("------------\n");

}

// query one set of IDs between minimum time stamp and maximum time stamp
void queryEvolvedPoints(long long mints, long long maxts, idbuf_t *buf,
		FILE *taidfp, FILE *tadbfp) {
	if (timeaxispages->pages != NULL) {
		evolved_point_t *temp = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		temp->inuse = 1;
		temp->prvTsId = -3;
		temp->nxtTsId = -3;
		temp->time = 0;
		temp->id = -3;
		evolved_point_t *previous = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		previous->inuse = 1;
		previous->prvTsId = -3;
		previous->nxtTsId = -3;
		previous->time = 0;
		previous->id = -3;
		evolved_point_t *next = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		next->inuse = 1;
		next->prvTsId = -3;
		next->nxtTsId = -3;
		next->time = 0;
		next->id = -3;
		searchforQueryBetween(mints, maxts, buf, timeaxispages->pages->content,
				timeaxispages->pages, temp, previous, next, tadbfp);
		free(temp);
		free(previous);
		free(next);
	}
}

// query one evolved point ID
long long queryEvolvedPoint(long long ts, FILE *taidfp, FILE *tadbfp) {
//	int recordbytes = 3LL * LONG_LONG + 1LL; // byte counting in one record
//	int pagererecords = 10LL; // record counting in one page
//	int pagebytes = recordbytes * pagererecords; // byte counting in one page
//	long long startbyte = 16LL; // start points in database
	long long id = NULL_POINTER;
	if (timeaxispages->pages != NULL) {
		evolved_point_t *temp = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		temp->inuse = 1;
		temp->prvTsId = -3;
		temp->nxtTsId = -3;
		temp->time = 0;
		temp->id = -3;
		evolved_point_t *previous = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		previous->inuse = 1;
		previous->prvTsId = -3;
		previous->nxtTsId = -3;
		previous->time = 0;
		previous->id = -3;
		evolved_point_t *next = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		next->inuse = 1;
		next->prvTsId = -3;
		next->nxtTsId = -3;
		next->time = 0;
		next->id = -3;
		searchforQueryEP(ts, timeaxispages->pages->content,
				timeaxispages->pages, temp, previous, next, tadbfp);
		id = temp->id;
		free(temp);
		free(previous);
		free(next);
		return id;
	}
	return NULL_POINTER;
}

// delete one evolved point
long long deleteEvolvedPoint(long long ts, FILE *taidfp, FILE *tadbfp) {
	int recordbytes = 3LL * LONG_LONG + 1LL; // byte counting in one record
	int pagererecords = 10LL; // record counting in one page
	int pagebytes = recordbytes * pagererecords; // byte counting in one page
	long long startbyte = 16LL; // start points in database
	long long id = NULL_POINTER;
	tadb_head_t *head = (tadb_head_t*) malloc(sizeof(tadb_head_t));
	head->first = -3;
	head->firstdirty = 0;
	head->last = -3;
	head->lastdirty = 0;
	if (timeaxispages->pages != NULL) {
		evolved_point_t *temp = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		temp->inuse = 1;
		temp->prvTsId = -3;
		temp->nxtTsId = -3;
		temp->time = 0;
		temp->id = -3;
		evolved_point_t *previous = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		previous->inuse = 1;
		previous->prvTsId = -3;
		previous->nxtTsId = -3;
		previous->time = 0;
		previous->id = -3;
		evolved_point_t *next = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		next->inuse = 1;
		next->prvTsId = -3;
		next->nxtTsId = -3;
		next->time = 0;
		next->id = -3;
		searchforDelete(ts, timeaxispages->pages->content, timeaxispages->pages,
				temp, previous, next, head, tadbfp);
		id = commitDelete(ts, previous, temp, next, head, recordbytes,
				startbyte, pagererecords, pagebytes, tadbfp);
		free(temp);
		free(previous);
		free(next);
		free(head);
	}
	return id;
}

long long insertEvolvedPoint(long long ts, FILE *taidfp, FILE *tadbfp) {
	int recordbytes = 3LL * LONG_LONG + 1LL; // byte counting in one record
	int pagererecords = 10LL; // record counting in one page
	int pagebytes = recordbytes * pagererecords; // byte counting in one page
	long long startbyte = 16LL; // start points in database
	long long id = NULL_POINTER;
	tadb_head_t *head = (tadb_head_t*) malloc(sizeof(tadb_head_t));
	head->first = -3;
	head->firstdirty = 0;
	head->last = -3;
	head->lastdirty = 0;
	if (timeaxispages->pages != NULL) {
		evolved_point_t *temp = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		temp->inuse = 1;
		temp->prvTsId = -3;
		temp->nxtTsId = -3;
		temp->time = 0;
		temp->id = -3;
		evolved_point_t *previous = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		previous->inuse = 1;
		previous->prvTsId = -3;
		previous->nxtTsId = -3;
		previous->time = 0;
		previous->id = -3;
		evolved_point_t *next = (evolved_point_t*) malloc(
				sizeof(evolved_point_t));
		next->inuse = 1;
		next->prvTsId = -3;
		next->nxtTsId = -3;
		next->time = 0;
		next->id = -3;
		searchforInsert(ts, timeaxispages->pages->content, timeaxispages->pages,
				temp, previous, next, head, tadbfp);
		id = commitInsert(ts, previous, temp, next, head, recordbytes,
				startbyte, pagererecords, pagebytes, tadbfp);
		free(temp);
		free(previous);
		free(next);
		free(head);
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

	//
	long long ts20 = 1593783931;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts20, taidfp, tadbfp);
	showAllPages();

	long long ts21 = 1593783977;	//(unsigned long) time(NULL);
	insertEvolvedPoint(ts21, taidfp, tadbfp);
	showAllPages();

	// delete evolved point
	long long ts22 = 1593783972;	//(unsigned long) time(NULL);
	deleteEvolvedPoint(ts22, taidfp, tadbfp);
	showAllPages();

	listAllTaIds();

	long long ts23 = 1593783961;
	long long id1 = queryEvolvedPoint(ts23, taidfp, tadbfp);
	printf("--%lld\n", id1);

	long long mints = 1593783969;
	long long maxts = 1593783974;
	idbuf_t *buf = (idbuf_t*) malloc(sizeof(idbuf_t));
	buf->id = NULL_POINTER;
	buf->nxt = NULL;
	queryEvolvedPoints(mints, maxts, buf, taidfp, tadbfp);

	fclose(taidfp);
	fclose(tadbfp);

	free(timeaxispages);
	free(cache);

}

