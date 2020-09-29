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
#include <stdbool.h>
#include <iconv.h>
#include <fcntl.h>
/*
 * utf8convert.h
 *
 * Created on: 2020年9月29日
 * Author: Dahai CAO
 */

#ifndef UTF8CONVERT_H_
#define UTF8CONVERT_H_

void convert2Utf8(char *fromstr, char *tostr, size_t length);

bool check_utf8(unsigned char *str, size_t length);

bool check_gb2312(unsigned char *str, size_t length);

bool check_gbk(unsigned char *str, size_t length);

int code_convert(char *from_charset, char *to_charset, char *inbuf,
		size_t inlen, char *outbuf, size_t outlen);

int u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

void convert2Utf8(char *fromstr, char *tostr, size_t length);


#endif /* UTF8CONVERT_H_ */
