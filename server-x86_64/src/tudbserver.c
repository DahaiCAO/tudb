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
//#include <pthread.h>
//#include <windows.h>



//#include "tudbserversock.h"
#include "threadpool.h"

//#pragma comment(lib,"threadpool.dll")
/*
 * tudbserver.c
 *
 * Created on: 2020-01-02 22:57:37
 * Author: Dahai CAO 
 */

void printWelcomeMesssage() {
	printf("%s\n", "Starting TuDB server .....");
}

//int* thread(void *arg) {
//	pthread_t newthid;
//
//	newthid = pthread_self();
//	printf("this is a new thread, thread ID = %d\n", newthid);
//	return NULL;
//}

//int main1(int argc, char *argv[]) {
//	printWelcomeMesssage();
//	SOCKET svr_socket = createServerSocket();
//	if (bindIpandPort(svr_socket)) {
//		if (listenPort(svr_socket)) {
//			SOCKET clt_socket;
//			if ((clt_socket = acceptRequest(svr_socket)) != FALSE) {
//				handleRequest(clt_socket);
//			}
//		}
//	}
//	typedef int (*FunSUM)(int, int);
//	HMODULE DLL = LoadLibraryA("threadpool");
//	//printf("%d\n", getKey());
//	FunSUM sum = (FunSUM)GetProcAddress(DLL,"sum");
//	printf("%s\n", "KKKKKKKKKKKKKK");
//	int c = sum(100, 444);

//	printf("%d\n", c);


//	hello("World");
//	printf("%d\n", dll_int_square(3));


	/*pthread_t thid;
 	printf("main thread ,ID is %d\n",pthread_self());
	if(pthread_create(&thid, NULL, (void *)thread, NULL) != 0) {
		printf("thread creation failed\n");
		exit(1);
	}*/


//}

