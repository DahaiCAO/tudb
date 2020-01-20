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

#include "tudbserversock.h"
#include "mythreadpool.h"
#include "confutil.h"
/*
 * tudbserver.h
 *
 * Created on: 2020年1月20日
 * Author: great
 */

#ifndef TUDBSERVER_H_
#define TUDBSERVER_H_

char *maximum_task_queue_size; // the information is from configuration file (tudb.conf)
char *normal_task_queue_size; // the information is from configuration file (tudb.conf)
char *maximum_thread_number; // the information is from configuration file (tudb.conf)
char *minimum_thread_number; // the information is from configuration file (tudb.conf)
char *normal_thread_number; // the information is from configuration file (tudb.conf)
char *adminport; // TuDB administration console's listening port
char *serverport; // TuDB server's listening port
char *serverip; // TuDB server IP address

/**
 * TuDB description information
 */
typedef struct tudbserver {
	thread_pool_t *pool; // TuDB server thread pool
	pthread_mutex_t svr_stat_lock;
	int svr_stat; // server status code, 0: off; 1:on;
	char *ip;
	int port;
} tudbserver_t;

void printWelcomeMesssage();

int createServer(tudbserver_t *svr);

#endif /* TUDBSERVER_H_ */
