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

#include "log.h"

/*
 * Log utility is used to support to  write log file.
 *
 * Created on: 2020-01-07 18:06:46
 * Author: Dahai CAO 
 */

#define MAXLEVELNUM (3)

LOGSET logsetting;
LOG loging;

const static char LogLevelText[4][10] = { "INFO", "DEBUG", "WARN", "ERROR" };

static char* getdate(char *date);

/**
 * parse the parameter from a code for log level recognization.
 */
static unsigned char getcode(char *path) {
	unsigned char code = 255;
	if (strcmp("INFO", path) == 0)
		code = 1;
	else if (strcmp("WARN", path) == 0)
		code = 3;
	else if (strcmp("ERROR", path) == 0)
		code = 4;
	else if (strcmp("NONE", path) == 0)
		code = 0;
	else if (strcmp("DEBUG", path) == 0)
		code = 2;
	return code;
}

/**
 * create the multiple level directories.
 */
void createdirs(char *muldir) {
	int i, len;
	char str[512] = { 0x0 };
	strncpy(str, muldir, 512);
	len = strlen(str);
	for (i = 0; i < len; i++) {
		if (str[i] == '/') {
			str[i] = '\0';
			if (access(str, 0) != 0) {
				mkdir(str);
			}
			str[i] = '/';
		}
	}
	if (len > 0 && access(str, 0) != 0) {
		mkdir(str);
	}
	return;
}

/**
 * read configurations from config file to set the config struct.
 */
static unsigned char readconf(char *logname, char *confpath) {
	char logpath[512] = { 0x0 };
	char logdate[50] = { 0x0 };
	char logfilename[50] = { 0x0 };

	FILE *fpath = fopen(confpath, "r");
	if (fpath == NULL)
		return -1;
	fscanf(fpath, "path=%s\n", logpath);
	if (access(logpath, 0) != 0) {
		createdirs(logpath);
	}
	getdate(logdate);
	strcpy(logfilename, logname);
	strcat(logfilename, logdate);
	strcat(logfilename, ".log");
	strcat(logpath, "/");
	strcat(logpath, logfilename);
	if (strcmp(logpath, logsetting.filepath) != 0)
		memcpy(logsetting.filepath, logpath, strlen(logpath));
	memset(logpath, 0, sizeof(logpath));

	fscanf(fpath, "level=%s\n", logpath);
	logsetting.loglevel = getcode(logpath);
	fclose(fpath);
	return 0;
}

/**
 * get configuration entry
 */
static LOGSET* getlogset(char *logname) {
	char path[512] = { 0x0 };
	getcwd(path, sizeof(path));
	strcat(path, "/conf/log.conf");
	if (access(path, F_OK) == 0) {
		if (readconf(logname, path) != 0) {
			logsetting.loglevel = INFO;
			logsetting.maxfilelen = 4096;
		}
	} else {
		logsetting.loglevel = INFO;
		logsetting.maxfilelen = 4096;
	}
	return &logsetting;
}

/**
 * get the current date to generate log file name.
 */
static char* getdate(char *date) {
	time_t timer = time(NULL);
	strftime(date, 11, "%Y%m%d", localtime(&timer));
	return date;
}

/**
 * get the current date time to generate the time stamp into log entry.
 */
static void settime() {
	time_t timer = time(NULL);
	strftime(loging.logtime, 20, "%Y-%m-%d %H:%M:%S", localtime(&timer));
}

/**
 * print the parameters according to format
 */
static void printflog(char *format, va_list args) {
	int d;
	char c, *s;
	while (*format) {
		switch (*format) {
		case 's': {
			s = va_arg(args, char*);
			fprintf(loging.logfile, "%s", s);
			break;
		}
		case 'd': {
			d = va_arg(args, int);
			fprintf(loging.logfile, "%d", d);
			break;
		}
		case 'c': {
			c = (char) va_arg(args, int);
			fprintf(loging.logfile, "%c", c);
			break;
		}
		default: {
			if (*format != '%' && *format != '\n')
				fprintf(loging.logfile, "%c", *format);
			break;
		}
		}
		format++;
	}
	fprintf(loging.logfile, "%s", "\n");
}

/**
 * initialize the log configurations.
 */
static int initlog(char *logname, unsigned char loglevel) {
	char strdate[30] = { 0x0 };
	LOGSET *logsetting;
	//获取日志配置信息
	if ((logsetting = getlogset(logname)) == NULL) {
		perror("Get Log Set Fail!");
		return -1;
	}
	if ((loglevel & (logsetting->loglevel)) != loglevel)
		return -1;

	memset(&loging, 0, sizeof(LOG));
	//获取日志时间
	settime();
	if (strlen(logsetting->filepath) == 0) {
		char *path = getenv("HOME");
		memcpy(logsetting->filepath, path, strlen(path));
		getdate(strdate);
		char logfilename[50] = { 0x0 };
		strcpy(logfilename, logname);
		strcat(logfilename, strdate);
		strcat(logfilename, ".log");
		strcat(logsetting->filepath, "/");
		strcat(logsetting->filepath, logfilename);
	}
	memcpy(loging.filepath, logsetting->filepath, MAXFILEPATH);
	if (loging.logfile == NULL) {
		if (access(loging.filepath, F_OK) == 0) {

		}
		loging.logfile = fopen(loging.filepath, "a+");
	}
	if (loging.logfile == NULL) {
		perror("Open Log File Fail!");
		return -1;
	}
	fprintf(loging.logfile, "[%s] [%s] ", LogLevelText[loglevel - 1],
			loging.logtime);
	return 0;
}

/**
 * write log to log file.
 */
int logwrite(char *logname, unsigned char loglevel, char *fromat, ...) {
	int rtv = -1;
	va_list args;

	//[为支持多线程需要加锁] pthread_mutex_lock(&mutex_log); //lock.

	do {
		//初始化日志
		if (initlog(logname, loglevel) != 0) {
			rtv = -1;
			break;
		}
		//打印日志信息
		va_start(args, fromat);
		printflog(fromat, args);
		va_end(args);
		//文件刷出
		fflush(loging.logfile);
		//日志关闭
		if (loging.logfile != NULL)
			fclose(loging.logfile);
		loging.logfile = NULL;
		rtv = 0;
	} while (0);

	//[为支持多线程需要加锁] pthread_mutex_unlock(&mutex_log); //unlock.

	return rtv;
}

