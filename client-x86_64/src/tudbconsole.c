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
			"                                   WELCOME                                         ");
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
 * tudbconsole is Tu DB's console of client program to connect local TuDB system.
 * The format of the command line is tudbconsole [-u root] [-p password], that is:
 * $>tudbconsole [-u root] [-p password]
 * or
 * $>tudbconsole [-u root]
 * password:
 * or
 * $>tudbconsole
 * user:
 * password:
 *
 * Created on: 2020-01-01 17:24:40
 * Author: Dahai CAO
 */
int main(int argc, char *argv[]) {
	setvbuf(stdout, NULL, _IONBF, 0);
//	printWelcomeMesssage();
//	// parse command line:
//	char *error_msg = "Correct command: tudbconsole [-u root] [-p password]\n";
//	char *user = "User login:";
//	char *pass = "Password:";
//	char *user_opt = "-u";
//	char *password_opt = "-p";
//	char username[50] = { '\0' };
//	char password[50] = { '\0' };
//	memset(username, '\0', sizeof(username));
//	memset(password, '\0', sizeof(password));
//	// verify the user name and password
//	if (argc == 1) {
//		printf("%s", user);
//		//fflush(stdout);
//		gets(username);
//		printf("%s", pass);
//		//fflush(stdout);
//		char ch;
//		int i = 0;
//		while ((ch = getch()) != 13) {
//			password[i] = ch;
//			i = i + 1;
//		}
//		// Note:it is strange: if input 2 characters, it will incorrect chars
//		// if input 4 characters, it will correct.
//	} else if (argc == 3) {
//		if (strcmp(argv[1], user_opt) == 0) {
//			if (strcmp(argv[2], password_opt) != 0) {
//				strcpy(username, argv[2]);
//				printf("%s", pass);
//				char ch;
//				int i = 0;
//				while ((ch = getch()) != 13) {
//					password[i] = ch;
//					i = i + 1;
//				}
//				// Note: it is strange: if input 2 characters, it will incorrect chars
//				// if input 4 characters, it will correct.
//			} else {
//				printf(error_msg);
//				return EXIT_SUCCESS;
//			}
//		} else {
//			printf(error_msg);
//			return EXIT_SUCCESS;
//		}
//	} else if (argc == 5) {
//		strcpy(username, argv[2]);
//		if (strcmp(argv[3], password_opt) == 0) {
//			strcpy(password, argv[4]);
//		} else {
//			printf(error_msg);
//			return EXIT_SUCCESS;
//		}
//	} else {
//		printf(error_msg);
//		return EXIT_SUCCESS;
//	}
	char *promptStr = "tudb>";
//	char *bye = "bye";
//	char *quit = "quit";
//	char *exit = "exit";
	SOCKET conn_sock = createConnection();
	if (conn_sock != 0) {
		if (connectServer(conn_sock)) {
			char *login = "1 uuuuuu pppppppp";
			//memset(login, 0, sizeof(login));
			//strcpy(login, "1 ");	// 1: login command
			//strcat(login, username);
			//strcat(login, " ");
			//strcat(login, password);
			int r = sendRequest(conn_sock, login);
			if (r != -1) { // send request successfully
				char recvbuf[1024] = { 0 };
				int rsp0 = receiveResponse(conn_sock, recvbuf);
				if (rsp0 > 0) {
					//printf("%s\n", recvbuf);
					printf("%s", promptStr);
					printf("%s\n", "Login successfully");
					//logwrite("CLT", INFO, "%s","kkkkkkkkkkkkk");
					printf("%s", promptStr);
					while (1) {
						char cmd_in[512] = { 0 };
						gets(cmd_in);
						int r = sendRequest(conn_sock, cmd_in);
						if (r != -1) {

							int rsp = receiveResponse(conn_sock, recvbuf);
							if (rsp <= 0) {
								printf("%s\n", "Connection has error.");
								break;
							}
							printf("%s\n", recvbuf);
							printf("%s", promptStr);
						}
//						while (strcmp(cmd_in, bye) != 0
//								&& strcmp(cmd_in, quit) != 0
//								&& strcmp(cmd_in, exit) != 0) {
//							char p[5] = { 0 }, cmd[512] = { 0 };
//							strcpy(p, promptStr);
//							strcpy(cmd, cmd_in);
//							strcat(p, cmd);
//							//printf("%s\n", p);
//							printf("%s", promptStr);
//							gets(cmd_in);
//							int r = sendRequest(conn_sock, cmd_in);
//							if (r == 0) {
//								char *recvbuf = receiveResponse(conn_sock);
//								printf("%s", recvbuf);
//							}
//						}
					}
				} else {
					closeConnection(conn_sock);
					printf("%s\n", "TuDb client exited");
				}
			}

		}

	}

	return 0;
}

