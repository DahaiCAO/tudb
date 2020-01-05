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

char *parseCommand(char * cmd, int cmd_len) {
	if (cmd_len > 0) {
		printf(">>>>>>>>>%c\n", cmd[0]);
	}
	//printf("%d Bytes received-------------- : %s\n", cmd_len, cmd);
	char *respbuf = "200";
	return respbuf;
}


