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
 * structbptreedef.h
 *
 * Created on: 2020年10月17日
 * Author: Dahai CAO
 */

#ifndef STRUCTBPTREEDEF_H_
#define STRUCTBPTREEDEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


#define M (4)
#define LIMIT_M_2 (M % 2 ? (M + 1)/2 : M/2)

typedef struct BPlusNode *BPlusTree,*Position;
typedef int KeyType;
struct BPlusNode{
    int KeyNum;
    KeyType Key[M + 1];
    BPlusTree Children[M + 1];
    BPlusTree Next;
};


#endif /* STRUCTBPTREEDEF_H_ */
