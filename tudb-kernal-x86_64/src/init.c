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

#include "init.h"
/*
 * init.c
 *
 * Created on: 2020年6月19日
 * Author: Dahai CAO
 */

// initialize DB
void initIdDB(char *path) {
	if ((access(path, F_OK)) == -1) {
		FILE *ta_id_fp = fopen(path, "wb+");
		// initializes Id DB
		unsigned char zero[LONG_LONG] = { 0L };
		fseek(ta_id_fp, 0L, SEEK_SET); // move file pointer to file head
		fwrite(zero, sizeof(unsigned char), LONG_LONG, ta_id_fp);
		//fseek(taidfp, LONG_LONG, SEEK_SET);  // move file pointer to 8th byte
		fwrite(zero, sizeof(unsigned char), LONG_LONG, ta_id_fp);
		fclose(ta_id_fp);
		ta_id_fp = NULL;
	}
}

void initIds(FILE *id_fp) {
	if (id_fp != NULL) {
		// initializes Id DB
		unsigned char nIds[LONG_LONG] = { 0L };
		unsigned char lastrIds[LONG_LONG] = { 0L };
		unsigned char rIds[LONG_LONG] = { 0L };
		long long nId = 2LL;
		long long lastrId = 2LL;
		long long rId = 1;
		LongToByteArray(nId, nIds); // convert
		LongToByteArray(lastrId, lastrIds); // convert
		LongToByteArray(rId, rIds); // convert
		fseek(id_fp, 0, SEEK_SET); // move file pointer to file end
		fwrite(nIds, sizeof(unsigned char), LONG_LONG, id_fp);
		fwrite(lastrIds, sizeof(unsigned char), LONG_LONG, id_fp);
		fwrite(rIds, sizeof(unsigned char), LONG_LONG, id_fp);
	}
}

// create and initialize new Tu time axis DB file
void initTimeAxisDB(char *path) {
	if ((access(path, F_OK)) == -1) {
		FILE *tadbfp = fopen(path, "wb+");
		// initializes Id DB
		unsigned char n2[LONG_LONG] = { 0L };
		LongToByteArray(NULL_POINTER, n2); // convert
		fseek(tadbfp, 0L, SEEK_SET); // move file pointer to file head
		fwrite(n2, sizeof(unsigned char), LONG_LONG, tadbfp);
		//fseek(tadbfp, LONG_LONG, SEEK_SET);  // move file pointer to 8th byte
		fwrite(n2, sizeof(unsigned char), LONG_LONG, tadbfp);
		fclose(tadbfp);
		tadbfp = NULL;
	}
}

// create and initialize new Tu DB file
void initDB(char *path) {
	if ((access(path, F_OK)) == -1) {
		// create a new Tu DB file
		FILE *tudbfp = fopen(path, "wb+");
		fclose(tudbfp);
		tudbfp = NULL;
	}
}

void initIdCaches(id_caches_t * caches) {
	caches->taIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->taIds->nId = NULL;
	caches->taIds->rId = NULL;
	caches->teIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->teIds->nId = NULL;
	caches->teIds->rId = NULL;
	caches->lblidxIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->lblidxIds->nId = NULL;
	caches->lblidxIds->rId = NULL;
	caches->lbltknIds =  (id_cache_t*) malloc(sizeof(id_cache_t));
	caches->lbltknIds->nId = NULL;
	caches->lbltknIds->rId = NULL;
}

void init() {


}
