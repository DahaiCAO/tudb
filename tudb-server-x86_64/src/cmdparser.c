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

/*
 * cmdparser.c
 *
 * Created on: 2020-01-03 17:50:17
 * Author: Dahai CAO 
 */

void parseCommandHeader(char *cmd, char *rqname, char* rqlen) {
	rqname[0] = cmd[0];
	rqname[1] = cmd[1];
	rqname[2] = cmd[2];
	rqname[3] = cmd[3];
	printf("%s\n", rqname);
	rqlen[0] = cmd[0];
	rqlen[1] = cmd[1];
	rqlen[2] = cmd[2];
	rqlen[3] = cmd[3];
	rqlen[4] = cmd[4];
	rqlen[5] = cmd[5];
	rqlen[6] = cmd[6];
	rqlen[7] = cmd[7];
	printf("%s\n", rqlen);
}

