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
#include "confutil.h"
#include "tudbclientsock.h"
#include "rqthelper.h"

#define RECV_SIZE  10240 // 4K receive size for receiving client requests
#define BUF_SIZE   15 // 512 buffer size

char *user = "user:";
char *pass = "password:";
char *user_opt = "-u";
char *password_opt = "-p";
char username[64] = { '\0' }; // maximum length of user name is 64 Bytes.
char password[64] = { '\0' }; // maximum length of password is 64 Bytes.
char *op_prompt = "tudb>";

char *serverport;
char *serverip;

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

/**
 *
 */
int doClient() {
	serverport = getconfentry("serverport");
	serverip = getconfentry("serverip");
	int sport = atoi(serverport);

	SOCKET conn_sock = createConnection();
	if (conn_sock != 0) {
		char recv_dat[RECV_SIZE] = { 0 }; // 10K
		char send_dat[4096] = { 0 }; // 4K
		char send_buf[512] = { 0 }; //
		if (connectServer(conn_sock, serverip, sport)) {
			createLoginRequest("root", "passwd", send_buf);
			//createLoginRequest(username, password, sendbuf);
			int r = sendRequest(conn_sock, send_buf);
			memset(send_buf, 0, sizeof(send_buf));
			if (r != -1) { // send request successfully
				// receive login response
				int rsp0 = receiveResponse(conn_sock, recv_dat, BUF_SIZE);
				if (rsp0 > 0) {
					if (strcmp(recv_dat, "200") == 0) { // login successfully
						printWelcomeMesssage();
						printf("%s", op_prompt);
						printf("%s\n", "Login successfully");
						logwrite("CLT", INFO, "%s", recv_dat);
						printf("%s", op_prompt);
						while (1) {
							gets(send_dat);
							if (strcmp(send_dat, "") != 0) {
								//char send_dat1[] = "bye";
								createRequest(send_dat);
								int r = sendRequest(conn_sock, send_dat);
								memset(send_dat, 0, sizeof(send_dat));
								if (r != -1) {
									// receive other command response after login
									memset(recv_dat, 0, sizeof(recv_dat));
									int rsp = receiveResponse(conn_sock,
											recv_dat,
											BUF_SIZE);
									if (rsp <= 0) {
										break;
									}
									printf("%s\n", recv_dat);
									printf("%s", op_prompt);
								} else {
									break;
								}
							} else {
								printf("%s", op_prompt);
							}
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
	// parse command line:
	//int r = parseLoginInput(argc, argv);
	//if (r != 0)
	//	return 0;
	doClient();
	return 0;
}

