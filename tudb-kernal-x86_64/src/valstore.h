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
#include <stdbool.h>
#include <iconv.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "tuidstore.h"
#include "structvaldef.h"
#include "macrodef.h"

/**
 * 这里的value只是小的值存储。如果值超长，就转到big value数据库中存储。
 * valstore.h
 *
 * Created on: 2020年10月7日
 * Author: Dahai CAO
 */

#ifndef VALSTORE_H_
#define VALSTORE_H_

// value pages by default.
val_page_t *val_pages;
// bytes in one value record
size_t val_record_bytes;
// records in one page with value records, configurable in .conf file
size_t VALUE_PAGE_RECORDS;
// bytes in one page with value records
size_t val_page_bytes;
// value page's expire time in memory
int VALUE_PAGE_EXPIRE_TIME;

void initValueDBMemPages(val_page_t *pages, FILE *val_db_fp);

#endif /* VALSTORE_H_ */
