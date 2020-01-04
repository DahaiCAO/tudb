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

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT 9088           // REMOTE PORT
#define DEFAULT_ADDR "127.0.0.1"    // server address
char recvbuf[DEFAULT_BUFLEN];

/**
 * tudbclt.c
 *
 *  Created on: 2019-12-30 18:33
 *  Last updated at: 2020-01-01 12:21 at Beijing home. It works now!
 *      Author: Dahai Cao
 */
SOCKET createConnection() {
	int iErrorMsg;
	// initialize win sock
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	iErrorMsg = WSAStartup(wVersionRequested, &wsaData);
	if (iErrorMsg != NO_ERROR) {
		printf("WSAStartup fault, error: %d\n", iErrorMsg);
		return FALSE;
	}

	// create a socket
	SOCKET conn_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (conn_sock == INVALID_SOCKET) {
		printf("Socket fault, error: %d\n", WSAGetLastError());
		WSACleanup();
		return FALSE;
	}
	return conn_sock;
}

/**
 *
 */
int connectServer(SOCKET conn_sock) {
	int iErrorMsg;
	// prepare network communication address
	struct sockaddr_in addrClient;
	memset(&addrClient, 0, sizeof(struct sockaddr_in));
	addrClient.sin_family = AF_INET;
	addrClient.sin_port = htons(DEFAULT_PORT);
	addrClient.sin_addr.s_addr = inet_addr(DEFAULT_ADDR);

	// connect to server
	iErrorMsg = connect(conn_sock, (struct sockaddr*) &addrClient,
			sizeof(struct sockaddr));
	if (iErrorMsg == SOCKET_ERROR) {
		printf("Fails connect the server, error: %d\n", WSAGetLastError());
		closesocket(conn_sock);
		conn_sock = INVALID_SOCKET;
	}
	return TRUE;
}

/**
 *
 */
int sendRequest(SOCKET conn_sock, char *sendbuf) {
	int iErrorMsg;
	// send data to server
	iErrorMsg = send(conn_sock, sendbuf, (int) strlen(sendbuf), 0);
	if (iErrorMsg == SOCKET_ERROR) {
		printf("Fails sending, error: %d\n", WSAGetLastError());
		closesocket(conn_sock);
		WSACleanup();
		return FALSE;
	}
	return EXIT_SUCCESS;
}

/**
 * Receive data from server,
 * current process can send bytes to sever continuously and
 * receive bytes from server continuously.
 *
 */
char* receiveResponse(SOCKET conn_sock) {
	int iErrorMsg;
	do {
		iErrorMsg = recv(conn_sock, recvbuf, DEFAULT_BUFLEN, 0);
		if (iErrorMsg > 0) {
			printf("%d Bytes received : %s\n", iErrorMsg, recvbuf);
			return recvbuf;
		} else if (iErrorMsg == 0) {
			printf("No bytes received\n");
		} else {
			printf("Received faults, error: %d\n", WSAGetLastError());
		}
	} while (iErrorMsg > 0);

	return EXIT_SUCCESS;
}

int closeConnection(SOCKET conn_sock) {
	shutdown(conn_sock, SD_BOTH);
	closesocket(conn_sock);
	WSACleanup();
	return EXIT_SUCCESS;
}
