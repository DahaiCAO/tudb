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

#undef UNICODE

#include <stdio.h>
#include <stdlib.h>
//#include <winsock2.h>

/*
 * cmdparser.h
 *
 * Created on: 2020年1月3日 下午5:51:25
 * Author: Dahai CAO 
 */

#ifndef RQTPARSER_H_
#define RQTPARSER_H_

long htoi(char *s, size_t size);

char* handleMsg(char *msghead, char *msgbody);

#endif /* CMDPARSER_H_ */
