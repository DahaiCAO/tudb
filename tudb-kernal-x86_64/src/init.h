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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "convert.h"
/*
 * init.h
 *
 * Created on: 2020年6月19日
 * Author: Dahai CAO
 */

#ifndef INIT_H_
#define INIT_H_

// initialize id DB
void initTimeAxisIdDB(char *path);
// initialize DB
void initTimeAxisDB(char *path);

#endif /* INIT_H_ */
