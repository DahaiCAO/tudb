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
#include <error.h>
#include <string.h>
#include <conio.h>

#include "cmdparser.h"

#define BUFFER_SIZE  4096 // 4K buffer for receiving client requests
#define DEFAULT_PORT    9088
#define DEFAULT_ADDR    "127.0.0.1"

char recvbuf[BUFFER_SIZE];
/*
 * tudbserver.c
 *
 *  Created on: 2019-12-29 20:48
 *  Last updated at: 2020-01-01 12:21 at Beijing home. It works now!
 *      Author: Dahai Cao
 */

SOCKET createServerSocket() {
	WSADATA wsaData;
	SOCKET srv_socket = INVALID_SOCKET;
	// initialize win sock

	WORD wVersionRequested = MAKEWORD(2, 2);
	int iErrorMsg = WSAStartup(wVersionRequested, &wsaData);
	if (iErrorMsg != NO_ERROR) {
		printf("WSAStartup fault, error return : %d\n", WSAGetLastError());
		return FALSE;
	}

	// create a socket
	srv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (srv_socket == INVALID_SOCKET) {
		printf("Socket fault, error: %d\n", WSAGetLastError());
		WSACleanup();
		return FALSE;
	}
	return srv_socket;
}

int closeClientSocket(SOCKET clt_socket) {
	int iErrorMsg = shutdown(clt_socket, SD_BOTH);
	if (iErrorMsg == SOCKET_ERROR) {
		printf("Shutdown fault, error: %d\n", WSAGetLastError());
		closesocket(clt_socket);
	}
	return EXIT_SUCCESS;
}

int closeServerSocket(SOCKET srv_socket) {
	closesocket(srv_socket);
	WSACleanup();
	return EXIT_SUCCESS;
}

int bindIpandPort(SOCKET srv_socket, char *ip, int port) {
	// prepare communication network address
	char *bip = DEFAULT_ADDR;
	if (ip != NULL) {
		bip = ip;
	}
	int bport = DEFAULT_PORT;
	if (port > 0) {
		bport = port;
	}
	struct sockaddr_in addrServer;
	memset(&addrServer, 0, sizeof(struct sockaddr));
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(bport);
	addrServer.sin_addr.s_addr = inet_addr(bip);

	// bind socket to loop back IP
	int iErrorMsg = bind(srv_socket, (const struct sockaddr*) &addrServer,
			sizeof(struct sockaddr));
	if (iErrorMsg == SOCKET_ERROR) {
		printf("Fault to bind interface, error: %d\n", WSAGetLastError());
		closesocket(srv_socket);
		WSACleanup();
		return FALSE;
	}
	return TRUE;
}

int listenPort(SOCKET srv_socket) {
	// listen to socket connections...
	int iErrorMsg = listen(srv_socket, SOMAXCONN);
	if (iErrorMsg == SOCKET_ERROR) {
		printf("listen fault, error: %d\n", WSAGetLastError());
		closesocket(srv_socket);
		WSACleanup();
		return FALSE;
	}
	return TRUE;
}

SOCKET acceptRequest(SOCKET srv_socket) {
	// waiting for client's connection (accept)
	struct sockaddr_in addrClient;
	int addrClientLen = sizeof(addrClient);
	SOCKET clt_socket = accept(srv_socket, (struct sockaddr*) &addrClient,
			&addrClientLen);
	if (INVALID_SOCKET == clt_socket) {
		printf("Failed to accept a client, error: %d\n", WSAGetLastError());
		closesocket(clt_socket);
		return FALSE;
	}
	return clt_socket;
}

/*
 * recv一般是读完就进入阻塞状态，因此需要一次就把所有的字节都读完。
 */
int handleRequest(SOCKET clt_socket) {
	int nbytes;
	while (1) {
		memset(recvbuf, 0, sizeof(recvbuf));
		nbytes = recv(clt_socket, recvbuf, sizeof(recvbuf), 0);
		if (nbytes > 0) {
			if (strcmp(recvbuf, "quit") == 0 || strcmp(recvbuf, "exit") == 0
					|| strcmp(recvbuf, "bye") == 0) {
				printf("%s\n", "close one client connection");
				closeClientSocket(clt_socket);
				break;
			}
			printf("%s\n", recvbuf);
			char *respbuf = parseCommand(recvbuf, sizeof(recvbuf));

			int sent_len = send(clt_socket, respbuf, sizeof(respbuf), 0);
			if (sent_len == SOCKET_ERROR) {
				printf("send error, error: %d\n", WSAGetLastError());
				closeClientSocket(clt_socket);
				break;
			}
		} else if (nbytes <= 0) { // client socket has been normally closed
			closeClientSocket(clt_socket);
			break;
		}
	}
	return 0;
}
