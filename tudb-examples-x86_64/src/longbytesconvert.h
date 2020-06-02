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
 * longconverter.h
 *
 * Created on: 2020-6-1
 * Author: Dahai CAO
 */


#ifndef LONGBYTESCONVERTER_H_
#define LONGBYTESCONVERTER_H_

void LongToByteArray(long long value, unsigned char buffer[]);

long long ByteArrayToLong(unsigned char buffer[]);

#endif /* LONGCONVERTER_H_ */
