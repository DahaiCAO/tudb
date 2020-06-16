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

#include "taidstore.h"

/*
 * idcache.c
 *
 * Created on: 2020年6月16日
 * Author: Dahai CAO
 */

id_cache_t *cache;

const long LONG_LONG = 8L; //

long long getOneId() {
	if (cache->rId[0] != 0) {
		for (int i = 99; i >= 0; i--) {
			if (cache->rId[i] != 0) {
				long long r = cache->rId[i];
				cache->rId[i] = 0;
				return r;
			}
		}
	} else if (cache->nId[0] != 0) {
		for (int i = 99; i >= 0; i--) {
			if (cache->nId[i] != 0) {
				long long r = cache->nId[i];
				cache->nId[i] = 0;
				return r;
			}
		}
	}
}

long long getId(FILE *idfp) {
	unsigned char ids[BUFFERSIZE] = { 0 };
	unsigned char zero[BUFFERSIZE] = { 0 };
	// get one id from id DB, update new id to db
	fseek(idfp, LONG_LONG, SEEK_SET);
	fread(ids, sizeof(unsigned char), LONG_LONG, idfp);
	long long s0 = ByteArrayToLong(ids);
	if (s0 == 0) { // no reused Id
		// fetch a new Id
		fseek(idfp, 0, SEEK_SET); // move file pointer to file head
		fread(ids, sizeof(unsigned char), LONG_LONG, idfp); // read the first bytes
		long long newid = ByteArrayToLong(ids); // convert array to long long
		// update the next free Id.
		LongToByteArray((newid + 1), ids); // convert
		fseek(idfp, 0L, SEEK_SET); // move file pointer to file end
		fwrite(ids, sizeof(unsigned char), LONG_LONG, idfp);
		return newid;
	} else if (s0 == 2) { // there is one reused Id
		fseek(idfp, LONG_LONG * 2, SEEK_SET); // move file pointer to the 16th byte
		fread(ids, sizeof(unsigned char), LONG_LONG, idfp); // read a byte array
		long long reusedId = ByteArrayToLong(ids);
		// remove and clear the reused Id
		fseek(idfp, LONG_LONG * 2, SEEK_SET);
		fwrite(zero, sizeof(unsigned char), LONG_LONG, idfp);
		// remove and clear the second long long
		fseek(idfp, LONG_LONG, SEEK_SET);
		fwrite(zero, sizeof(unsigned char), LONG_LONG, idfp);
		return reusedId;
	} else if (s0 > 2) { // there are more than one reused Ids
		// get one reused Id
		fseek(idfp, s0 * LONG_LONG, SEEK_SET);
		fread(ids, sizeof(unsigned char), LONG_LONG, idfp);
		long long reusedId = ByteArrayToLong(ids);
		// remove and clear the reused Id
		fseek(idfp, s0 * LONG_LONG, SEEK_SET);
		fwrite(zero, sizeof(unsigned char), LONG_LONG, idfp);
		// update the second long long
		fseek(idfp, LONG_LONG, SEEK_SET);
		LongToByteArray((s0 - 1), ids);
		fwrite(ids, sizeof(unsigned char), LONG_LONG, idfp);
		return reusedId;
	}
	return 0;
}

void loadIds(FILE *idfp) {
	// step 1: load new Ids
	unsigned char ids[BUFFERSIZE] = { 0 };
	// fetch a new Id
	fseek(idfp, 0, SEEK_SET); // move file pointer to file head
	fread(ids, sizeof(unsigned char), LONG_LONG, idfp); // read the first bytes
	long long newid = ByteArrayToLong(ids); // convert array to long long
	for (int i = 0; i < 10; i++) {
		cache->nId[i] = newid;
		newid += 1;
	}
	// update the next free Id.
	LongToByteArray((newid + 1), ids); // convert
	fseek(idfp, 0L, SEEK_SET); // move file pointer to file end
	fwrite(ids, sizeof(unsigned char), LONG_LONG, idfp);
	// step 2: load reused Ids
	fseek(idfp, LONG_LONG, SEEK_SET);
	fread(ids, sizeof(unsigned char), LONG_LONG, idfp);
	long long s0 = ByteArrayToLong(ids);
	if (s0 != 0) { // there reused Id
		// 16 means 2 * 8 Bytes for long long
		long long lenOfReusedId = (s0 + 1) * LONG_LONG - 16;
		if (lenOfReusedId > 800) {
			// only read 800 bytes
			long long p = lenOfReusedId - 8 * 100;
			fseek(idfp, p, SEEK_SET);
			unsigned char buf[8 * 100] = { 0 };
			fread(buf, sizeof(unsigned char), 8 * 100, idfp);
			for (int i = 0; i < 100; i++) {
				memset(ids, 0, LONG_LONG);
				for (int j = 0; j < 8; j++) {
					ids[j] = buf[i * 8 + j];
				}
				long long d = ByteArrayToLong(ids);
				cache->rId[i] = d;
			}
			fseek(idfp, p, SEEK_SET);		// clean all reused Ids
			memset(buf, 0, 8 * 100);
			fwrite(buf, sizeof(unsigned char), 8 * 100, idfp);
		} else {
			// all read
			long long p = lenOfReusedId;
			fseek(idfp, 2 * 8, SEEK_SET);
			unsigned char buf[100] = { 0 };//lenOfReusedId
			fread(buf, sizeof(unsigned char), lenOfReusedId, idfp);
			long long l = lenOfReusedId / 8;
			for (int i = 0; i < l; i++) {
				memset(ids, 0, LONG_LONG);
				for (int j = 0; j < 8; j++) {
					ids[j] = buf[i * 8 + j];
				}
				long long d = ByteArrayToLong(ids);
				cache->rId[i] = d;
			}
			fseek(idfp, 2 * 8, SEEK_SET);		// clean all reused Ids
			memset(buf, 0, lenOfReusedId);
			fwrite(buf, sizeof(unsigned char), lenOfReusedId, idfp);
		}

		fseek(idfp, s0 * LONG_LONG, SEEK_SET);
		fread(ids, sizeof(unsigned char), LONG_LONG * 50, idfp);
		unsigned char zero[BUFFERSIZE * 50] = { 0 };
		/*long long reusedId = ByteArrayToLong(ids);
		 // remove and clear the reused Id
		 fseek(idfp, s0 * LONG_LONG, SEEK_SET);
		 fwrite(zero, sizeof(unsigned char), LONG_LONG, idfp);
		 // update the second long long
		 fseek(idfp, LONG_LONG, SEEK_SET);
		 LongToByteArray((s0 - 1), ids);
		 fwrite(ids, sizeof(unsigned char), LONG_LONG, idfp);*/
	}

}

void recycleOneId() {

}

void recycleId(FILE *idfp, long long id) {
	unsigned char ids[BUFFERSIZE] = { 0L };
	unsigned char buf[BUFFERSIZE] = { 0L };
	fseek(idfp, LONG_LONG, SEEK_SET);
	fread(ids, sizeof(unsigned char), LONG_LONG, idfp);
	long long s0 = ByteArrayToLong(ids);
	if (s0 == 0L) {
		LongToByteArray(id, buf); // convert
		fseek(idfp, LONG_LONG * 2, SEEK_SET); // move file pointer from file head
		fwrite(buf, sizeof(unsigned char), LONG_LONG, idfp); // write

		LongToByteArray(2L, buf); // convert
		fseek(idfp, LONG_LONG, SEEK_SET); // move file pointer the 8th byte
		// update the second long long
		fwrite(buf, sizeof(unsigned char), LONG_LONG, idfp);
	} else {
		LongToByteArray(id, buf);
		fseek(idfp, (s0 + 1) * LONG_LONG, SEEK_SET);
		fwrite(buf, sizeof(unsigned char), LONG_LONG, idfp);

		LongToByteArray((s0 + 1), buf);
		fseek(idfp, LONG_LONG, SEEK_SET); // move file pointer the 8th byte
		// update the second long long
		fwrite(buf, sizeof(unsigned char), LONG_LONG, idfp);
	}
}


void readAllTaIds(FILE *idfp) {
	unsigned char buf[BUFFERSIZE] = { 0 };
	fseek(idfp, 0L, SEEK_END);
	long sz = ftell(idfp);
	printf("Db size:%ld bytes\n", sz);
	printf("------------\n");
	int i = 0;
	while (!feof(idfp) && sz > 0L) { // pageable to read
		fseek(idfp, i * LONG_LONG, SEEK_SET);
		fread(buf, sizeof(unsigned char), LONG_LONG, idfp);
		long long s0 = ByteArrayToLong(buf);
		// If you are on windows and using MingW, GCC uses the win32 runtime,
		// where printf needs %I64d for a 64 bit integer.
		// (and %I64u for an unsigned 64 bit integer)
		//printf("%I64d\n", s0);
		printf("%lld\n", s0);
		i++;
	}
	printf("------------\n");
	printf("Reused Id queue length:%d\n", (i - 2));
}


