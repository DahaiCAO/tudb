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
#include <string.h>
#include "macrodef.h"
/*
 * convert.h
 *
 * Created on: 2020年6月19日
 * Author: Dahai CAO
 */

#ifndef CONVERT_H_
#define CONVERT_H_

// Convert long long to byte array
void LongToByteArray(long long value, unsigned char buffer[]);

// Convert byte array to long long
long long ByteArrayToLong(unsigned char buffer[]);

// convert integer to bytes
void Integer2Bytes(long value, unsigned char bytes[4]);

long Bytes2Integer(unsigned char bytes[4]);

#endif /* CONVERT_H_ */
