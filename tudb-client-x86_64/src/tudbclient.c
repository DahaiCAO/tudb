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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "log.h"
#include "tudbclientsock.h"

char *user = "user:";
char *pass = "password:";
char *user_opt = "-u";
char *password_opt = "-p";
char username[64] = { '\0' }; // maximum length of user name is 64 Bytes.
char password[64] = { '\0' }; // maximum length of password is 64 Bytes.
char *promptStr = "tudb>";

/**
 * Output the welcome message.
 */
void printWelcomeMesssage() {
	printf("%s\n",
			"/+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/");
	printf("%s\n",
			"         _____________                ___________     ______________               ");
	printf("%s\n",
			"        |             |              |           \\   |              \\              ");
	printf("%s\n",
			"        |___       ___|              |     ___    \\  |      ___      |                ");
	printf("%s\n",
			"            |     |  ____      ____  |    |   |    | |     |___|     |                  ");
	printf("%s\n",
			"            |     | |    |    |    | |    |   |    | |              /                  ");
	printf("%s\n",
			"            |     | |    |    |    | |    |   |    | |      ___     \\                  ");
	printf("%s\n",
			"            |     | |    |____|    | |    |___|    | |     |___|     |                 ");
	printf("%s\n",
			"            |     | |              | |            /  |               |                 ");
	printf("%s\n",
			"            |_____|  \\____________/  |__________ /   |______________/                  ");
	printf("%s\n",
			"                                                                                   ");
	printf("%s\n",
			"                                   WELCOME TO CLIENT                                        ");
	printf("%s\n",
			"                              Version 0.1 2020.1                                   ");
	printf("%s\n",
			"               **************************************************                  ");
	printf("%s\n",
			"                   https://github.com/swinflowcloud/tudb                           ");
	printf("%s\n",
			"/++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/");
}
//
//char* getMsg(const char *msg, int error) {
//	char c[10] = { 0 };
//	//sprintf(c, "%d", error); // for cross platform
//	itoa(error, c, 10);// for windows platform
//	char *m = "";
//	m = strcat(m, msg);
//	m = strcat(m, c);
//	return m;
//}

/*
 * Parse input command parameters such as user
 * name and password for login
 */
int parseLoginInput(int argc, char *argv[]) {
	char *err_msg =
			"Bad command, correct command: tudbclient [-u root] [-p password]\n";
	char *err_msg1 =
			"User name too long, correct command: tudbclient [-u root] [-p password]\n";
	char *err_msg2 =
			"Password too long, correct command: tudbclient [-u root] [-p password]\n";

	memset(username, '\0', sizeof(username));
	memset(password, '\0', sizeof(password));
	// verify the user name and password
	if (argc == 1) { // command: $>tudbclient (return)
		printf("%s", user);
		gets(username);
		if (strlen(username) > 64) { // user name too long
			printf(err_msg1);
			return 0;
		}
		printf("%s", pass);
		char ch;
		int i = 0;
		while ((ch = getch()) != 13) {
			password[i] = ch;
			if (i > 63) {
				printf(err_msg2); // password too long
				return 0;
			} else {
				i = i + 1;
			}
		}
	} else if (argc == 3) { // command: $>tudbclient [-u root] (return)
		if (strcmp(argv[1], user_opt) == 0) {
			if (strcmp(argv[2], password_opt) != 0) {
				strcpy(username, argv[2]);
				if (strlen(username) > 64) { // user name too long
					printf(err_msg1);
					return 0;
				}
				printf("%s", pass);
				char ch;
				int i = 0;
				while ((ch = getch()) != 13) {
					password[i] = ch;
					if (i > 63) {
						printf(err_msg2);
						return 0;
					} else {
						i = i + 1;
					}
				}
			} else {
				printf(err_msg);
			}
		} else {
			printf(err_msg);
		}
	} else if (argc == 5) { // command: $>tudbclient [-u root] [-p password] (return)
		strcpy(username, argv[2]);
		if (strlen(username) > 64) { // user name too long
			printf(err_msg1);
			return 0;
		}
		if (strcmp(argv[3], password_opt) == 0) {
			strcpy(password, argv[4]);
			if (strlen(password) > 64) { // password too long
				printf(err_msg2);
				return 0;
			}
		} else {
			printf(err_msg);
		}
	} else {
		printf(err_msg);
	}
	return 0;
}

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

/**
 * Constructs a login command. The command is protocol on TCP.
 * We use plain code to construct it due to this client runs
 * with server.
 *
 * The request protocol is:
 * request name(4 Bytes integer)|length(4 Bytes integer)|request content\0
 *
 * The response protocol is:
 * response name(4 Bytes integer)|length(4 Bytes integer)|response content\0
 *
 */
void contructLoginCmd(char *username, char *password, char *cmd) {
	strcpy(cmd, "0001"); // 0001: login command， 4 Bytes length

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
	strcat(cmd, h);
	strcat(cmd, username);
	strcat(cmd, " ");
	strcat(cmd, password);
	//printf("%s\n", cmd);

}

/**
 *
 */
int doClient() {
	SOCKET conn_sock = createConnection();
	if (conn_sock != 0) {
		char recv[10240] = { 0 }; // 10K
		char recvbuf[512] = { 0 };
		char send[2048] = { 0 }; // 2K
		char sendbuf[512] = { 0 }; //
		if (connectServer(conn_sock)) {
			contructLoginCmd("root_uri_url_microsoft", "password_passport_key_log_server", sendbuf);
			int r = sendRequest(conn_sock, sendbuf);
			memset(sendbuf, 0, sizeof(sendbuf));
			if (r != -1) { // send request successfully
				// receive login response
				int rsp0 = receiveResponse(conn_sock, recvbuf);
				if (rsp0 > 0) {
					//printf("%s\n", recvbuf);
					printf("%s", promptStr);
					printf("%s\n", "Login successfully");
					//logwrite("CLT", INFO, "%s","kkkkkkkkkkkkk");
					printf("%s", promptStr);
					while (1) {
						gets(send);
						int r = sendRequest(conn_sock, send);
						memset(send, 0, sizeof(send));
						if (r != -1) {
							// receive other command response after login
							memset(recv, 0, sizeof(recv));
							int rsp = receiveResponse(conn_sock, recv);
							if (rsp <= 0) {
								break;
							}
							printf("%s\n", recv);
							printf("%s", promptStr);
						} else {
							break;
						}
					}
				}
			}
		}
		// close client socket
		closeConnection(conn_sock);
	}
	return 0;
}

/*
 * tudbclient is Tu DB's client program to connect local TuDB system.
 * The format of the command line is tudbclient [-u root] [-p password], that is:
 * $>tudbclient [-u root] [-p password]
 * or
 * $>tudbclient [-u root]
 * password:
 * or
 * $>tudbclient
 * user:
 * password:
 *
 * Created on: 2020-01-01 17:24:40
 * Author: Dahai CAO
 */
int main(int argc, char *argv[]) {
	setvbuf(stdout, NULL, _IONBF, 0);
	printWelcomeMesssage();
	// parse command line:
	//int r = parseLoginInput(argc, argv);
	//if (r != 0)
	//	return 0;
	doClient();
	return 0;
}

