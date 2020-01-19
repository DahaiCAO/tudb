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

#include "tudbclientsock.h"

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
		logwrite("CLT", ERROR, "%s","WSAStartup fault, error 1");
		printf("WSAStartup fault, error: %d\n", iErrorMsg);
		return FALSE;
	}

	// create a socket
	SOCKET conn_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (conn_sock == INVALID_SOCKET) {
		logwrite("CLT", ERROR, "%s","Socket fault, error 2");
		printf("Socket fault, error: %d\n", WSAGetLastError());
		WSACleanup();
		return FALSE;
	}
	return conn_sock;
}

/**
 *
 */
int connectServer(SOCKET conn_sock, char *ip, int port) {
	char *bip = DEFAULT_ADDR;
	if (ip != NULL) {
		bip = ip;
	}
	int bport = DEFAULT_PORT;
	if (port > 0) {
		bport = port;
	}
	int iErrorMsg;
	// prepare network communication address
	struct sockaddr_in addrClient;
	memset(&addrClient, 0, sizeof(struct sockaddr_in));
	addrClient.sin_family = AF_INET;
	addrClient.sin_port = htons(bport);
	addrClient.sin_addr.s_addr = inet_addr(bip);

	// connect to server
	iErrorMsg = connect(conn_sock, (struct sockaddr*) &addrClient,
			sizeof(struct sockaddr));
	if (iErrorMsg == SOCKET_ERROR) {
		logwrite("CLT", ERROR, "%s","Fails connect the server, error 3");
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
	int status;
	// send data to server
	status = send(conn_sock, sendbuf, strlen(sendbuf), 0);
	if (status == SOCKET_ERROR) {
		logwrite("CLT", ERROR, "%s","Fails sending, error 4");
		printf("Fails sending, error: %d\n", WSAGetLastError());
		closesocket(conn_sock);
	}
	return 0;
}

int receiveMsg(SOCKET clt_socket, char *req_name, char *reqbody, const int buf_size) {
	int nbytes;
	char buf[buf_size];
	char req_len[9]; // request length, it is hex decimal number
	memset(buf, 0, sizeof(buf)); // clean receive buffer
	memset(req_len, 0, sizeof(req_len)); // clean receive buffer
	nbytes = recv(clt_socket, buf, sizeof(buf), 0); //
	if (nbytes <= 0) {
		return nbytes;
	}
	strncpy(req_name, &buf[0], 4);
	strncpy(req_len, &buf[4], 8); // &buf[4] is the pointer of fourth element
	//memcpy(req_len, &buf[4], 4);// another method
	long msg_len = htoi(req_len, sizeof(req_len));
	if (nbytes > 12) {
		strncpy(reqbody, &buf[12], (nbytes - 12));
	}
	if (buf_size < msg_len) {
		int c = 2;
		while ((c * buf_size) < msg_len) {
			memset(buf, 0, sizeof(buf));
			nbytes = recv(clt_socket, buf, sizeof(buf), 0); //
			strcat(reqbody, buf);
			c++;
		}
		memset(buf, 0, sizeof(buf));
		nbytes = recv(clt_socket, buf, sizeof(buf), 0); //
		if (nbytes <= 0) {
			return nbytes;
		}
		strcat(reqbody, buf);
	}
	return nbytes;
}


/**
 * Receive data from server,
 * current process can send bytes to sever continuously and
 * receive bytes from server continuously.
 *
 */
int receiveResponse(SOCKET clt_socket, char * recv_buf, const int buf_size) {
	int nbytes = -1;
	while (1) {
		char req_name[5] = { 0 }; // request name, like http request, get/post
		nbytes = receiveMsg(clt_socket, req_name, recv_buf, buf_size);
		if (nbytes > 0) {
			//printf("%d Bytes received : %s\n", bufsize, recvbuf);
			break;// needs to update.
		} else if (nbytes <= 0) {
			closeConnection(clt_socket);
			break;
		}
	}
	return nbytes;
}

int closeConnection(SOCKET conn_sock) {
	shutdown(conn_sock, SD_BOTH);
	closesocket(conn_sock);
	WSACleanup();
	return EXIT_SUCCESS;
}


