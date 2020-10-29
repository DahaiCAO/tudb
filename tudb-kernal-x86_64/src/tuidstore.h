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
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <unistd.h>

#include "convert.h"
#include "macrodef.h"
#include "structiddef.h"

/*
 * idcache.h
 *
 * Created on: 2020年6月16日
 * Author: Dahai CAO
 */

#ifndef TAIDSTORE_H_
#define TAIDSTORE_H_

size_t ID_QUEUE_LENGTH; // default cache Id capacity. 100 by default.
size_t LABEL_ID_QUEUE_LENGTH; // label cache Id capacity.
size_t KEY_ID_QUEUE_LENGTH; // key cache Id capacity.
size_t VALUE_ID_QUEUE_LENGTH; // value cache Id capacity.
size_t TIMEAXIS_ID_QUEUE_LENGTH; // time axis cache Id capacity.
size_t ID_INDEX_QUEUE_LENGTH; // default cache Id capacity. 100 by default.

long long getOneId(FILE *id_fp, id_cache_t *cache, size_t id_length);

void loadAllIds(FILE *id_fp, id_cache_t *cache, size_t id_length);

void loadNewIds(FILE *id_fp, id_cache_t *cache, size_t id_length);

void loadReusedIds(FILE *id_fp, id_cache_t *cache, size_t id_length);

void listAllIds(id_cache_t *cache);

void recycleOneId(long long id, id_cache_t *cache);

void deallocIdCache(id_t *ids);

void initIdCaches(id_caches_t * caches);

void deallocIdCaches(id_caches_t * caches);

//long long getId(FILE *idfp);
//
//void recycleId(FILE *idfp, long long id);
//
//void showAllTaIds(FILE *idfp);

#endif /* IDCACHE_H_ */
