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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * test.c
 *
 * Created on: 2020-01-07 18:33:57
 * Author: Dahai CAO 
 */

void hexconcat(char *buf, char *t) {
	if (strlen(buf) == 1) {
		strcat(t, "0000000");
	} else if (strlen(buf) == 2) {
		strcat(t, "000000");
	} else if (strlen(buf) == 3) {
		strcat(t, "00000");
	} else if (strlen(buf) == 4) {
		strcat(t, "0000");
	} else if (strlen(buf) == 5) {
		strcat(t, "000");
	} else if (strlen(buf) == 6) {
		strcat(t, "00");
	} else if (strlen(buf) == 7) {
		strcat(t, "0");
	}
	strcat(t, buf);
}

void createRequest(char *sd_buf) {
	if (strcmp(sd_buf, "bye") == 0 || strcmp(sd_buf, "quit") == 0
			|| strcmp(sd_buf, "exit") == 0) { // 0003: logout
		char buf[12 + sizeof(sd_buf)];
		memset(buf, 0, sizeof(buf));
		strcat(buf, "0003");
		char nn[2] = { 0 };
		itoa(1, nn, 16); // convert to hexdecimal number
		char h[8] = { 0 }; // 8 Bytes length
		hexconcat(nn, h);
		strcat(buf, h);
		strcat(buf, "X");
		strcpy(sd_buf, buf);
	} else if (strcmp(sd_buf, "show ") == 0) { //
	}
}

//#include "log.h"
//#include "confutil.h"

int main(int argv, char **argc) {

	char send_dat1[] = "bye";
	createRequest(send_dat1);
}
