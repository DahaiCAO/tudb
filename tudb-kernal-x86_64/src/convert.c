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
 * converter.c
 *
 * Created on: 2020年6月16日
 * Author: Dahai CAO
 */
const long LONG_LONG_SIZE = 8L; //
// Convert long long to byte array
void LongToByteArray(long long value, unsigned char buffer[]) {
	memset(buffer, 0, LONG_LONG_SIZE);
	for (int i = 0; i < LONG_LONG_SIZE; i++) {
		buffer[i] = ((value >> (LONG_LONG_SIZE * i)) & 0XFF);
	}
}

// Convert byte array to long long
long long ByteArrayToLong(unsigned char buffer[]) {
	long long recoveredValue = 0;
	for (int i = 0; i < LONG_LONG_SIZE; i++) {
		long long byteVal = (((long long) buffer[i]) << (LONG_LONG_SIZE * i));
		recoveredValue = recoveredValue | byteVal;
	}
	return recoveredValue;
}

#ifndef CONVERTER_C_
#define CONVERTER_C_



#endif /* CONVERTER_C_ */
