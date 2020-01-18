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
 * mythreadpool.c
 *
 * Created on: 2020年1月12日 下午1:31:28
 * Author: Dahai CAO 
 */

#include "mythreadpool.h"
#include "log.h"

thread_pool_t* createthreadpool(int max_thread_num,
		int min_thread_num,
		int nrm_thr_number,
		int nrm_queue_size,
		int queue_max_size) {
	thread_pool_t *pool = (thread_pool_t*) malloc(sizeof(thread_pool_t));
	if (pool == NULL) {
		logwrite(THP, ERR, "%s", "Create thread pool failed");
		return NULL;
	} else {
		logwrite(THP, INFO, "%s", "Create thread pool successfully");
	}
	pthread_mutex_init(&(pool->queue_lock), NULL);
	pthread_mutex_init(&(pool->thread_counter), NULL);
	pthread_cond_init(&(pool->queue_ready), NULL);
	pthread_cond_init(&(pool->queue_not_full), NULL);
	pool->task_queue = NULL; // linked list, don't need to allocate memory
	pool->minimum_thread_number = min_thread_num;
	pool->maximum_thread_number = max_thread_num;
	pool->normal_thread_number = min_thread_num;
	pool->live_thread_number = 0;
	pool->live_thread_number = 0;
	pool->busy_thread_number = 0;
	pool->wait_exit_thread_number = 0;

	pool->current_task_queue_size = 0;
	pool->normal_task_queue_size = 0;
	pool->maximum_task_queue_size = queue_max_size;
	pool->shutdown = 0; // 0: shutdown; 1:running;
	pool->task_threads = (pthread_t*) malloc(
			max_thread_num * sizeof(pthread_t));
	if (pool->task_threads == NULL) {
		logwrite(THP, DEBUG, "%s", "Create work queue failed");
		free(pool);
		free(pool->task_queue);
		return NULL;
	} else {
		logwrite(THP, INFO, "%s", "Create work queue successfully");
	}
	// clean the thread pool
	memset(pool->task_threads, 0, sizeof(pthread_t) * max_thread_num);

	int i = 0;
	for (i = 0; i < min_thread_num; i++) {
		pthread_create(&(pool->task_threads[i]), NULL, (void*) runthread, pool);
	}
	// administration thread
	pthread_create(&(pool->admin_thread), NULL, (void*) adminthreadpool, pool);
	return pool;
}

/*向线程池中加入任务*/
int addtask(thread_pool_t *pool, void* (*process)(void *arg), void *arg) {
	/*构造一个新任务*/
	task_t *newworker = (task_t*) malloc(sizeof(task_t));
	newworker->process = process;
	newworker->arg = arg;
	newworker->next = NULL;/*别忘置空*/
	pthread_mutex_lock(&(pool->queue_lock));
	// if worker queue is full, then wait...
	while ((pool->current_task_queue_size == pool->maximum_task_queue_size)
			&& (!pool->shutdown))
		pthread_cond_wait(&(pool->queue_not_full), &(pool->queue_lock));

	if (pool->shutdown) {
		pthread_mutex_unlock(&(pool->queue_lock));
		return -1;
	}
	// add the new worker to the worker queue tail
	task_t *member = pool->task_queue;
	if (member != NULL) {
		while (member->next != NULL)
			member = member->next;
		member->next = newworker;
	} else {
		pool->task_queue = newworker;
	}
	assert(pool->task_queue != NULL);
	pool->current_task_queue_size++;
	pthread_mutex_unlock(&(pool->queue_lock));
	/*好了，等待队列中有任务了，唤醒一个等待线程；
	 注意如果所有线程都在忙碌，这句没有任何作用*/
	pthread_cond_signal(&(pool->queue_ready));
	return 0;
}

int freethreadpool(thread_pool_t *pool) {
	if (pool == NULL)
		return -1;
	if (pool->task_threads) {
		free(pool->task_threads);
		pthread_mutex_lock(&(pool->queue_lock)); /*先锁住再销毁*/
		pthread_mutex_destroy(&(pool->queue_lock));
		pthread_mutex_lock(&(pool->thread_counter));
		pthread_mutex_destroy(&(pool->thread_counter));
		pthread_cond_destroy(&(pool->queue_ready));
		pthread_cond_destroy(&(pool->queue_not_full));
	}
	free(pool);
	pool = NULL;

	return 0;
}

/*销毁线程池，等待队列中的任务不会再被执行，但是正在运行的线程会一直
 把任务运行完后再退出*/
int destroythreadpool(thread_pool_t *pool) {
	if (pool->shutdown)
		return -1;/*防止两次调用*/
	pool->shutdown = 1;
	/*唤醒所有等待线程，线程池要销毁了*/
	pthread_cond_broadcast(&(pool->queue_ready));
	/*阻塞等待线程退出，否则就成僵尸了*/
	int i;
	for (i = 0; i < pool->maximum_thread_number; i++)
		pthread_join(pool->task_threads[i], NULL);
	free(pool->task_threads);
	/*销毁等待队列*/
	task_t *head = NULL;
	while (pool->task_queue != NULL) {
		head = pool->task_queue;
		pool->task_queue = pool->task_queue->next;
		free(head);
	}
	freethreadpool(pool);
	return 0;
}

//非常重要的任务接口函数，各子线程统一调用这个函数，
//而这个函数内部检查调用任务队列中的实际任务函数指针。
void* runthread(thread_pool_t *pool) {
//	char *msg = "starting thread 0x%x\n";
//	char m[8];
//	long l = pthread_self();
//	ltoa(l, m, 8);
//	strcat(msg, m);
//	logwrite(THP, INFO, "%s", msg);
	//printf("starting thread 0x%x\n", (unsigned int) pthread_self());
	while (1) {
		pthread_mutex_lock(&(pool->queue_lock));
		/*如果等待队列为0并且不销毁线程池，则处于阻塞状态; 注意
		 pthread_cond_wait是一个原子操作，等待前会解锁，唤醒后会加锁*/
		while (pool->current_task_queue_size == 0 && !pool->shutdown) {
			//char *msg1 = "thread 0x%x is waiting\n";
			//strcat(msg1, ultoa(pthread_self()));
			//logwrite(THP, INFO, msg1);
			//printf("thread 0x%x is waiting\n", (unsigned int) pthread_self());
			pthread_cond_wait(&(pool->queue_ready), &(pool->queue_lock));
			if (pool->wait_exit_thread_number > 0) {
				// destroy the threads
				pool->wait_exit_thread_number--; // ??
				if (pool->live_thread_number > pool->minimum_thread_number) {
					pool->live_thread_number--;
					pthread_mutex_unlock(&(pool->queue_lock));
					pthread_exit(NULL);	//
				}
			}
		}
		/*线程池要销毁了*/
		if (pool->shutdown) {
			/*遇到break,continue,return等跳转语句，千万不要忘记先解锁*/
			pthread_mutex_unlock(&(pool->queue_lock));
			//char *msg2 = "thread 0x%x will exit\n";
			//strcat(msg2, ultoa(pthread_self()));
			//logwrite(THP, INFO, msg2);
			//printf("thread 0x%x will exit\n", (unsigned int) pthread_self());
			pthread_exit(NULL);
		}
		printf("thread 0x%x is starting to work\n",
				(unsigned int) pthread_self());
		/*assert是调试的好帮手*/
		assert(pool->current_task_queue_size != 0);
		assert(pool->task_queue != NULL);

		/*等待队列长度减去1，并取出链表中的头元素*/
		pool->current_task_queue_size--;
		task_t *task_head = pool->task_queue;
		if (task_head != NULL) {
			pool->task_queue = task_head->next;
		}
		pthread_cond_broadcast(&(pool->queue_not_full));
		pthread_mutex_unlock(&(pool->queue_lock));

		pthread_mutex_lock(&(pool->thread_counter));
		pool->busy_thread_number++; // busy thread counting++
		pthread_mutex_unlock(&(pool->thread_counter));

		/*调用回调函数，执行任务*/
		(*(task_head->process))(task_head->arg);
		free(task_head);
		task_head = NULL;

		pthread_mutex_lock(&(pool->thread_counter));
		pool->busy_thread_number--;
		pthread_mutex_unlock(&(pool->thread_counter));
	}
	/*这一句应该是不可达的*/
	pthread_exit(NULL);
}

void* adminthreadpool(thread_pool_t *pool) {
	int i;
	while (!pool->shutdown) {
		sleep(1000); /*隔一段时间再管理*/
		pthread_mutex_lock(&(pool->queue_lock));
		// current waiting queue size.
		int queue_size = pool->current_task_queue_size; // current task queue size
		int live_thr_num = pool->live_thread_number; // live thread number
		pthread_mutex_unlock(&(pool->queue_lock));

		pthread_mutex_lock(&(pool->thread_counter));
		int busy_thr_num = pool->busy_thread_number; // busy thread number
		pthread_mutex_unlock(&(pool->thread_counter));

		// creates new thread to balance task queue workload
		// the thread number will reach the normal thread number in pool
		if (queue_size >= pool->normal_task_queue_size
				&& live_thr_num <= pool->maximum_thread_number) {
			pthread_mutex_lock(&(pool->queue_lock));
			int add = 0;
			// increase size of thread pool to normal thread number to balance
			// task queue workload.
			for (i = 0;
					i < pool->maximum_thread_number
							&& add < pool->normal_thread_number
							&& pool->live_thread_number
									< pool->maximum_thread_number; i++) {
				if (pool->task_threads[i] == 0
						|| !isthreadalive(pool->task_threads[i])) {
					pthread_create(&(pool->task_threads[i]), NULL,
							(void*) runthread, (void*) pool);
					add++;
					pool->live_thread_number++;
				}
			}
			pthread_mutex_unlock(&(pool->queue_lock));
		}

		// destroy the idle and redundant threads if busy threads by 2 less
		// than number of the live threads, and the number is less
		if (busy_thr_num < live_thr_num
				&& live_thr_num > pool->minimum_thread_number) {
			// destroy the (pool->normal_task_queue_size -
			// pool->minimum_thread_number) to destroy themselves
			pthread_mutex_lock(&(pool->queue_lock));
			pool->wait_exit_thread_number = pool->normal_task_queue_size;
			pthread_mutex_unlock(&(pool->queue_lock));
			for (i = 0;
					i
							< (pool->normal_task_queue_size
									- pool->minimum_thread_number); i++) {
				// notify the idle threads to destroy themselves
				pthread_cond_signal(&(pool->queue_ready));
			}
		}
	}
	return NULL;
}

/*线程是否存活*/
int isthreadalive(pthread_t tid) {
	int kill_rc = pthread_kill(tid, 0); //发送0号信号，测试是否存活
	if (kill_rc == ESRCH) { //线程不存在
		return 0;
	}
	return 1;
}

