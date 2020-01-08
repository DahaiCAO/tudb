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
 * test.c
 *
 * Created on: 2020年1月6日 上午7:18:15
 * Author: Dahai CAO 
 */

#include<stdio.h>
#include<setjmp.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

jmp_buf mark;

int Div(int a,int b){
    if(b==0){
        longjmp(mark,1);
    }
    return a/b;
}
int main(){
//    int State = setjmp(mark);
//    if(State==0){
//        Div(4,0);
//    }else{
//        switch(State){
//            case 1:
//                printf("除0异常!\n");
//        }
//    }
//    return 0;

	errno = 0;
	if (NULL == fopen("d:\\tudata\\d.txt", "a+"))
	{
	printf("----Msg:%s\n", strerror(errno));
	perror("----msg");
	}
	else
	{
	 printf("Msg:%s\n", strerror(errno));
	 perror("msg");
	}
	return 0;


}
