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

#include "tudbserver.h"

/*
 * This program is Tu DB server main program. Tu DB
 *
 * Created on: 2020-01-02 22:57:37
 * Author: Dahai CAO 
 */

/*
 * Outputs welcome message to console.
 */
void printWelcomeMesssage() {
	printf("%s\n", "Starting TuDB server .....");
}

int createServer(tudbserver_t *svr) {
	SOCKET svr_socket = createServerSocket();
	if (bindIpandPort(svr_socket, svr->ip, svr->port)) {
		if (listenPort(svr_socket)) {
			printf("%s\n", "listening......");
			SOCKET clt_socket;
			while ((clt_socket = acceptRequest(svr_socket)) != FALSE) {
				printf("%s\n", "get one connect");
				addtask(svr->pool, (void*) handleRequest, (void *)clt_socket);
			}
		}
	}
	return 0;
}

int main(int argc, char **argv) {
	//setvbuf(stdout, NULL, _IOLBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	printWelcomeMesssage();

	// read configuration info
	maximum_task_queue_size = getconfentry("maximum_task_queue_size");
	normal_task_queue_size = getconfentry("normal_task_queue_size");
	maximum_thread_number = getconfentry("maximum_thread_number");
	minimum_thread_number = getconfentry("minimum_thread_number");
	normal_thread_number = getconfentry("normal_thread_number");
	serverport = getconfentry("serverport");
	serverip = getconfentry("serverip");

	int a = atoi(maximum_task_queue_size);
	int b = atoi(normal_task_queue_size);
	int c = atoi(maximum_thread_number);
	int d = atoi(minimum_thread_number);
	int e = atoi(normal_thread_number);
	int sport = atoi(serverport);

	tudbserver_t *svr = (tudbserver_t*) malloc(sizeof(tudbserver_t));
	pthread_mutex_init(&(svr->svr_stat_lock), NULL);
	svr->ip = serverip;
	svr->port = sport;
	svr->pool = createthreadpool(c, d, e, b, a);
	if (svr->pool != NULL) {
		//addtask(svr->pool, (void*) createServer, &svr);
		while (1) {
			createServer(svr);
//			pthread_mutex_lock(&(svr->svr_stat_lock));
//			if (svr->svr_stat == 0) {
//				destroythreadpool(svr->pool);
//				printf("%s\n", "TuDB server shutdown.");
//				break;
//			}
//			pthread_mutex_unlock(&(svr->svr_stat_lock));
		}
	}
	destroythreadpool(svr->pool);
	pthread_mutex_lock(&(svr->svr_stat_lock));
	pthread_mutex_destroy(&(svr->svr_stat_lock));
	free(svr);
	printf("%s\n", "TuDB server shutdown.");
	return 0;
}
