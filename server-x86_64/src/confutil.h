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
 * Configuration utility is used to get the entries in configuration file. The
 * file is always read by program and not written, so we don't provide the write
 * functionality.
 *
 * Created on: 2020-01-08 16:33:28
 * Author: Dahai CAO 
 */

#ifndef CONFUTIL_H_
#define CONFUTIL_H_

#define filename "./conf/tudb.conf"

void trim(char *strIn, char *strOut);

void getValue(char *keyAndValue, const char *key, char *value);

void readconfigfile(const char *fname/*in*/, const char *key/*in*/,
		char **value/*out*/);

char* getconfentry(const char *key);

#endif /* CONFUTIL_H_ */
