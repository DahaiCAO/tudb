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

#include "rqthelper.h"

/*
 * rqthelper.c
 *
 * Created on: 2020年1月20日
 * Author: great
 */

/**
 * Constructs a login command. The command is protocol on TCP.
 * We use plain code to construct it due to this client runs
 * with server.
 *
 * The request protocol is:
 * request name(4 Bytes integer)|length(8 Bytes integer)|request content\0
 *
 * The response protocol is:
 * response name(4 Bytes integer)|length(8 Bytes integer)|response content\0
 *
 */
void createLoginRequest(char *username, char *password, char *sd_buf) {
	strcpy(sd_buf, "0001"); // 0001: login command， 4 Bytes length

	int usrlen = strlen(username);
	char mm[10] = { 0 };
	itoa(usrlen, mm, 10);

	int pwdlen = strlen(password);
	char nn[10] = { 0 };
	itoa(pwdlen, nn, 10);

	int lensum = 12 + usrlen + pwdlen + 1; // " " is one byte

	itoa(lensum, nn, 16); // convert to hexdecimal number
	char h[8] = { 0 }; // 8 Bytes length
	hexconcat(nn, h);
	strcat(sd_buf, h);
	strcat(sd_buf, username);
	strcat(sd_buf, " ");
	strcat(sd_buf, password);
	//printf("%s\n", cmd);
}

/**
 * Adds '0' to a string denoting a hex decimal number, e.g.,
 *
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

/**
 * create a client request.
 * The request protocol is:
 * request name(4 Bytes integer)|length(8 Bytes integer)|request content\0
 */
void createRequest(char *sd_buf) {
	if (strcmp(sd_buf, "bye") == 0 || strcmp(sd_buf, "quit") == 0
			|| strcmp(sd_buf, "exit") == 0) { // 0003: logout
		char buf[sizeof(sd_buf)];
		strcat(buf, "0003");
		char nn[2] = {0};
		itoa(1, nn, 16); // convert to hexdecimal number
		char h[8] = { 0 }; // 8 Bytes length
		hexconcat(nn, h);
		strcat(buf, h);
		strcpy(buf, "X", 12);
	} else if (strcmp(sd_buf, "show ") == 0) { //
	}
}
