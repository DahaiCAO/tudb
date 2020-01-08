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
 * Log utility is used to support to  write log file.
 *
 * Created on: 2020-01-07 18:07:00
 * Author: Dahai CAO
 */

#ifndef LOG_H_
#define LOG_H_

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "stdarg.h"
#include "unistd.h"

#define MAXLEN (2048)
#define MAXFILEPATH (512)
#define MAXFILENAME (50)

typedef enum {
	ERROR_1 = -1, ERROR_2 = -2, ERROR_3 = -3
} ERROR0;

typedef enum {
	NONE = 0, INFO = 1, DEBUG = 2, WARN = 3, ERROR = 4, ALL = 255
} LOGLEVEL;

typedef struct log {
	char logtime[20];
	char filepath[MAXFILEPATH];
	FILE *logfile;
} LOG;

typedef struct logseting {
	char filepath[MAXFILEPATH];
	unsigned int maxfilelen;
	unsigned char loglevel;
} LOGSET;

int logwrite(char *logname, unsigned char loglevel, char *fromat, ...);

#endif /* LOG_H_ */
