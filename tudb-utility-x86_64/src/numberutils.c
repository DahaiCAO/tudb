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
#include "numberutils.h"
/*
 * numberutils.c
 *
 * Created on: 2020-01-18 10:03:09
 * Author: Dahai CAO 
 */

/**
 * this function convert the hex decimal string to decimal integer.
 * The format of parameter is '0000000c' convert to 12,
 * '0000000e' -> 14. But, the convert may have some issues for the big number.
 * You need to test this algorithm. This function is only used to parse
 * the length of message between tudb client and tudb server.
 *
 * @author Dahai Cao created at 13:50 on 2020-01-18
 */
long htoi(char *s, size_t size) {
	long n = 0;
	int i = 0;
	while (s[i] != '\0' && i < size) {
		if (s[i] != '0') {
			if (s[i] >= '0' && s[i] <= '9') {
				n = n * 16 + (s[i] - '0');
			} else if (s[i] >= 'a' && s[i] <= 'f') {
				n = n * 16 + (s[i] - 'a') + 10;
			} else if (s[i] >= 'A' && s[i] <= 'F') {
				n = n * 16 + (s[i] - 'A') + 10;
			}
		}
		i++;
	}
	return n;
}
