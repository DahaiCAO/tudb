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

/*
 * mythreadpool.h
 *
 * Created on: 2020年1月12日 下午1:31:46
 * Author: Dahai CAO 
 */

#ifndef MYTHREADPOOL_H_
#define MYTHREADPOOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>

#define THP "THREADPOOL"
/*
 *线程池里所有运行和等待的任务都是一个CThread_worker
 *由于所有任务都在链表里，所以是一个链表结构
 */
typedef struct task {
	/*回调函数，任务运行时会调用此函数，注意也可声明成其它形式*/
	void* (*process)(void *arg);
	void *arg;/*回调函数的参数*/
	struct task *next;
} task_t;

/*线程池结构*/
typedef struct thread_pool {
	int shutdown; // shutdown the thread pool
	pthread_mutex_t queue_lock; //
	pthread_mutex_t thread_counter; //
	pthread_cond_t queue_not_full; // task queue is not full and has space
	pthread_cond_t queue_ready; // task queue is not empty and has tasks to run

	/*链表结构，线程池中所有等待任务*/
	task_t *task_queue;
	pthread_t *task_threads;
	pthread_t admin_thread; // administration thread
	int maximum_task_queue_size; // maximum capacity size of task queue, 1 by default
	int current_task_queue_size; // number of tasks in queue to run currently, 0 by default
	int normal_task_queue_size; // normal task number in task queue, 0 by default

	int minimum_thread_number; // minimum capacity of active threads in thread pool, 1 by default
	int normal_thread_number; // normal number of threads to run tasks, 1 by default
	int maximum_thread_number; // maximum capacity of active threads in thread pool, 1 by default
	int live_thread_number; // live thread number in thread pool, 0 by default
	int busy_thread_number; // busy thread number in thread pool, 0 by default
	int wait_exit_thread_number; // number of redundant and idle threads that can be destroyed, 0 by default

} thread_pool_t;

thread_pool_t* createthreadpool(int max_thread_num,
		int min_thread_num,
		int nrm_thr_number,
		int nrm_queue_size,
		int queue_max_size);

int addtask(thread_pool_t *pool, void* (*process)(void *arg), void *arg);

int destroythreadpool(thread_pool_t *pool);

void* runthread(thread_pool_t *pool);

int freethreadpool(thread_pool_t *pool);

void* adminthreadpool(thread_pool_t *pool);

int isthreadalive(pthread_t tid);

#endif /* MYTHREADPOOL_H_ */
