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

/*
 * Configuration utility is used to get the entries in configuration file. The
 * file is always read by program and not written, so we don't provide the write
 * functionality.
 *
 * Created on: 2020-01-08 16:33:14
 * Author: Dahai CAO 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "confutil.h"

/**
 * Trim is used to remove the white space from the string.
 */
void trim(char *strIn, char *strOut) {
	char *start, *end, *temp;
	temp = strIn;
	while (*temp == ' ') {
		++temp;
	}
	start = temp; // get head pointer
	// get the pointer of the last character from the original string (not '\0')
	temp = strIn + strlen(strIn) - 1;
	while (*temp == ' ') {
		--temp;
	}
	end = temp; // get tail pointer
	for (strIn = start; strIn <= end;) {
		*strOut++ = *strIn++;
	}
	*strOut = '\0';
	return;
}

void getValue(char *keyAndValue, const char *key, char *value) {
	char *p = keyAndValue;
	p = strstr(keyAndValue, key);
	if (p == NULL) { // does not exist this key
		return;
	}
	p += strlen(key);
	trim(p, value);
	p = strstr(value, "=");
	if (p == NULL) { // there is no '='
		return;
	}
	p += strlen("=");
	trim(p, value);

	p = strstr(value, "=");
	if (p != NULL) { // this key is redundant;
		return;
	}
	p = value;
	trim(p, value);
	return;
}

/*
 * read configurations from configuration file.
 */
void readconfigfile(const char *fname/*in*/, const char *key/*in*/,
		char **value/*out*/) {
	FILE *pf = NULL;
	char line[1024] = { 0 }, vtemp[1024] = { 0 };
	pf = fopen(fname, "r");
	while (!feof(pf)) {
		fgets(line, 1024, pf);
		getValue(line, key, vtemp);
		if (strlen(vtemp) != 0)
			break;
	}
	if (strlen(vtemp) != 0) {
		*value = (char*) malloc(sizeof(char) * strlen(vtemp) + 1);
		strcpy(*value, vtemp);
	} else
		*value = NULL;
	if (pf != NULL)
		fclose(pf);
	return;
}

char* getconfentry(const char *key) {
	char *value;
	readconfigfile(filename/*in*/, key/*in*/, &value/*out*/);
	return value;
}
