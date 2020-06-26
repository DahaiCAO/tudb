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
void initTimeAxisIdDB(char *path) {
	if ((access(path, F_OK)) == -1) {
		FILE *taidfp = fopen(path, "wb+");
		// initializes Id DB
		unsigned char zero[LONG_LONG] = { 0L };
		fseek(taidfp, 0L, SEEK_SET); // move file pointer to file head
		fwrite(zero, sizeof(unsigned char), LONG_LONG, taidfp);
		//fseek(taidfp, LONG_LONG, SEEK_SET);  // move file pointer to 8th byte
		fwrite(zero, sizeof(unsigned char), LONG_LONG, taidfp);
		fclose(taidfp);
	}
}

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
	}
}

