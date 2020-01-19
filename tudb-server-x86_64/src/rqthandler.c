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
#undef UNICODE

#include <string.h>

#include "rqthandler.h"


/*
 * cmdparser.c
 *
 * Created on: 2020-01-03 17:50:17
 * Author: Dahai CAO 
 */

void hexconcat(char *buf,  char *t) {
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

void createReponse(char *msg, char *msghead, char *msgbody) {
	strcpy(msg, msghead); // 4 Bytes length

	int rsplen = 12 + strlen(msgbody);
	char len[8] = { 0 };
	itoa(rsplen, len, 16);

	char h[8] = { 0 }; // 8 Bytes length for store the hex number
	hexconcat(len, h);
	strcat(msg, h);
	strcat(msg, msgbody);
	//printf("%s\n", msg);
}

/**
 * About the communication protocol on client and server of TuDB:
 *
 * Odd numbers code: it is request;
 * 0001: Login request;
 * Even numbers code: it is response;
 * 0002: Login response:
 *
 * The request protocol is:
 * request name(4 Bytes integer)|length(4 Bytes integer)|request content\0
 *
 * The response protocol is:
 * response name(4 Bytes integer)|length(4 Bytes integer)|response content\0
 *
 */
char* handleMsg(char* msg, char *msghead, char *msgbody) {
	char *rsp = "";
	if (strcmp(msghead, "0001") == 0) { // 0001: login request
		char *root = "root";
		char *password = "passwd";
		// verify user name and password in TuDB system...
		char *usr = strtok(msgbody, " ");
		char *pwd = strtok(NULL, " ");
		// verify user name and password in memory
		if (strcmp(usr, root) == 0 && strcmp(pwd, password) == 0) {
			rsp = "1234567890_login_califnia_melbourne_sydney_queenland_china_airfare_arline_hot_fire";
			createReponse(msg, "0002", rsp); // 0002: login response
		} else {
			rsp = "1234567890_login_failed";
			createReponse(msg, "0002", rsp);
		}
		return msg;
	}
	return msg;
}




