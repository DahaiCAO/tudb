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
#include "convert.h"
/*
 * convert.c
 *
 * Created on: 2020年6月19日
 * Author: Dahai CAO
 */
// Convert long long to byte array
void LongToByteArray(long long value, unsigned char buffer[]) {
	memset(buffer, 0, LONG_LONG);
	for (int i = 0; i < LONG_LONG; i++) {
		buffer[i] = ((value >> (LONG_LONG * i)) & 0XFF);
	}
}

// Convert byte array to long long
long long ByteArrayToLong(unsigned char buffer[]) {
	long long recoveredValue = 0;
	for (int i = 0; i < LONG_LONG; i++) {
		long long byteVal = (((long long) buffer[i]) << (LONG_LONG * i));
		recoveredValue = recoveredValue | byteVal;
	}
	return recoveredValue;
}
