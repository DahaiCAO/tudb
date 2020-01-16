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
#include <unistd.h>
#include <time.h>

#include "tudbserversock.h"
#include "mythreadpool.h"
#include "confutil.h"

/*
 * tudbserver.c
 *
 * Created on: 2020-01-02 22:57:37
 * Author: Dahai CAO 
 */
char *maximum_task_queue_size;
char *normal_task_queue_size;
char *maximum_thread_number;
char *minimum_thread_number;
char *normal_thread_number;
char *adminport;
char *serverport;
char *serverip;

void printWelcomeMesssage() {
	printf("%s\n", "Starting TuDB server .....");
}

//void* myprocess(void *arg) {
//	printf("thread id is 0x%x, working on task %d\n",
//			(unsigned int) pthread_self(), *(int*) arg);
//	sleep(1);/*休息一秒，延长任务的执行时间*/
//	return NULL;
//}

//int createsocket() {
//	SOCKET svr_socket = createServerSocket();
//	if (bindIpandPort(svr_socket)) {
//		if (listenPort(svr_socket)) {
//			SOCKET clt_socket;
//			if ((clt_socket = acceptRequest(svr_socket)) != FALSE) {
//				handleRequest(clt_socket);
//			}
//		}
//	}
//}

int main(int argc, char **argv) {
	//setvbuf(stdout, NULL, _IOLBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	//setvbuf(stdout, NULL, _IOLBF, 0);
	//setvbuf(stdout, NULL, _IONBF, 0);
	printWelcomeMesssage();

	// read configuration info
	maximum_task_queue_size = getconfentry("maximum_task_queue_size");
	normal_task_queue_size = getconfentry("normal_task_queue_size");
	maximum_thread_number = getconfentry("maximum_thread_number");
	minimum_thread_number = getconfentry("minimum_thread_number");
	normal_thread_number = getconfentry("normal_thread_number");
	adminport = getconfentry("adminport");
	serverport = getconfentry("serverport");
	serverip = getconfentry("serverip");

	int a = atoi(maximum_task_queue_size);
	int b = atoi(normal_task_queue_size);
	int c = atoi(maximum_thread_number);
	int d = atoi(minimum_thread_number);
	int e = atoi(normal_thread_number);
	int sport = atoi(serverport);
	int aport = atoi(adminport);

	thread_pool_t *pool = createthreadpool(c, d, e, b, a);
	if (pool != NULL) {
		SOCKET svr_socket = createServerSocket();
		if (bindIpandPort(svr_socket, serverip, sport)) {
			if (listenPort(svr_socket)) {
				printf("%s\n", "listening......");
				SOCKET clt_socket;
				while ((clt_socket = acceptRequest(svr_socket)) != FALSE) {
					printf("%s\n", "get one connect");
					addtask(pool, (void*) handleRequest, (void*) clt_socket);
				}
			}
		}
		/*等待所有任务完成*/
		//sleep(5);  //这句可能出问题，偷懒写法。
		/*销毁线程池*/

		destroythreadpool(pool);
		printf("%s\n", "TuDB shutdown.");
	}
	return 0;

}
