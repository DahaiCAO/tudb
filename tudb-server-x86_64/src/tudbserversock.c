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

#include "tudbserversock.h"
#include "tudbserver.h"

#define RECV_SIZE     4096 // 4K receive size for receiving client requests
#define BUF_SIZE      15   // 512 buffer size
#define DEFAULT_PORT  9088
#define DEFAULT_ADDR  "127.0.0.1"

char recv_dat[RECV_SIZE];
char buf[BUF_SIZE];

/*
 * tudbserver.c
 *
 *  Created on: 2019-12-29 20:48
 *  Last updated at: 2020-01-01 12:21 at Beijing home. It works now!
 *  Author: Dahai Cao
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
	}
	closesocket(clt_socket);
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
		closeServerSocket(srv_socket);
		return FALSE;
	}
	return TRUE;
}

int listenPort(SOCKET srv_socket) {
	// listen to socket connections...
	int iErrorMsg = listen(srv_socket, SOMAXCONN);
	if (iErrorMsg == SOCKET_ERROR) {
		printf("listen fault, error: %d\n", WSAGetLastError());
		closeServerSocket(srv_socket);
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
		closeClientSocket(clt_socket);
		return FALSE;
	}
	return clt_socket;
}

int receiveMsg(SOCKET clt_socket, char *req_name,
		char *req_body, const int buf_size) {
	int nbytes;
	char buf[buf_size];
	char req_len[9] = { 0 }; // request length
	memset(buf, 0, sizeof(buf)); // clean receive buffer
	memset(req_len, 0, sizeof(req_len)); // clean receive buffer
	nbytes = recv(clt_socket, buf, sizeof(buf), 0); //
	if (nbytes <= 0) {
		return nbytes;
	}
	// ----- parse message header
	strncpy(req_name, &buf[0], 4);
	if (strcmp(req_name, "0003") == 0) { // recognize request name: 0003: logout
		printf("%s\n", "client request to close client connection");
		return -1; // log out
	}
	strncpy(req_len, &buf[4], 8); // &buf[4] is the pointer of fourth element
	//memcpy(req_len, &buf[4], 4);// another method
	long msg_len = htoi(req_len, sizeof(req_len));
	// ----- parse message header
	if (nbytes > 12) {
		strncpy(req_body, &buf[12], (nbytes - 12));
	}
	if (buf_size < msg_len) {
		int c = 2;
		while ((c * buf_size) < msg_len) {
			memset(buf, 0, sizeof(buf));
			nbytes = recv(clt_socket, buf, sizeof(buf), 0); //
			strcat(req_body, buf);
			c++;
		}
		memset(buf, 0, sizeof(buf));
		nbytes = recv(clt_socket, buf, sizeof(buf), 0); //
		if (nbytes <= 0) {
			return nbytes;
		}
		strcat(req_body, buf);
	}
	return nbytes;
}

/*
 * recv一般是读完就进入阻塞状态，因此需要一次就把所有的字节都读完。
 */
int handleRequest(SOCKET clt_socket) {
	int nbytes;
	while (1) {
		char req_name[5]; // request name, like http request, get/post
		memset(req_name, 0, sizeof(req_name));
		nbytes = receiveMsg(clt_socket, req_name, recv_dat, BUF_SIZE);
		if (nbytes > 0) {
			char resp_buf[10240] = {0};
			handleMsg(resp_buf, req_name, recv_dat);
			int sent_len = send(clt_socket, resp_buf, strlen(resp_buf), 0);
			memset(resp_buf, 0, sizeof(resp_buf)); // clean the response buffer
			memset(recv_dat, 0, sizeof(recv_dat)); // clean the request buffer
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
