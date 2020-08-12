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
 * lbltokenstore.h
 *
 * Created on: 2020年8月12日
 * Author: Dahai CAO
 */

#ifndef LBLTOKENSTORE_H_
#define LBLTOKENSTORE_H_

typedef struct _label_token {
	long long id; // pointer in DB
	long long taId; //time axis Id, it is for evolution
	unsigned char inUse; // 1: in use; otherwise, 0
	int len;
	struct _label_token * nxtBlkId;// next block
	char * blkContent;
} label_token_t;

typedef struct _label_index {
	long long id; // pointer in DB
	long long lblCount; // the number of this label has been used
	label_token_t *tknId;
	int codingType;//
} label_index_t;


#endif /* LBLTOKENSTORE_H_ */
