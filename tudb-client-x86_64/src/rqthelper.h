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
/*
 * rqthelper.h
 *
 * Created on: 2020年1月20日
 * Author: great
 */

#ifndef RQTHELPER_H_
#define RQTHELPER_H_

void hexconcat(char *buf, char *t);

void createRequest(char * sd_buf);

void createLoginRequest(char *username, char *password, char *sd_buf);

#endif /* RQTHELPER_H_ */
