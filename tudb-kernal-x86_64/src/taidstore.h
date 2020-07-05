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

long long getOneId();

void loadIds(FILE *idfp);

void listAllTaIds();

void showAllTaIds(FILE *idfp);

void recycleOneId();

long long getId(FILE *idfp);

void recycleId(FILE *idfp, long long id);

#endif /* IDCACHE_H_ */
