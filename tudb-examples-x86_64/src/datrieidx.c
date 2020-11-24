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
 * datrieidx.c
 *
 * Created on: 2020年11月13日
 * Author: Dahai CAO
 */
#include "datrieidx.h"

#define MIN_CODE 1
#define MAX_CODE 255
#define BC_INC 10
#define TAIL_INC 10
#define TEMP_INC 5
#define CHAR_NUM 26

int *BC; // 数组的头指针
char *TAIL;
char *TEMP;
int BC_POS; // 数组的指针
int TAIL_POS;
int BC_MAX; // 数组的最大长度
int TAIL_MAX;
int TEMP_MAX;



void realloc_bc();
void separate(int s, char *b, int tail_pos);
int change_bc(int current, int s, char *list, char ch);

void printBC() {
	int i = 0;
	while (BC[i] != 0) {
		printf("BC[%d]%d\n", i, BC[i]);
		i++;
	}
}

int base(int n) {
	if (n > BC_POS) {
		return 0;
	} else {
		printf("read base index= %d :value= %d\n", n, BC[2 * n]);
		return BC[2 * n];
	}
}

int check(int n) {
	if (n > BC_POS) {
		return 0;
	} else {
		printf("read check index= %d :value= %d\n", n, BC[2 * n + 1]);
		return BC[2 * n + 1];
	}
}

void w_base(int n, int node) {
	while (n >= BC_MAX) {
		realloc_bc();
	}
	if (n > BC_POS) {
		BC_POS = n;
	}
	BC[2 * n] = node;
	 printBC();
	printf("write base index= %d: value= %d\n", n, BC[2 * n]);
}

void w_check(int n, int node) {
	while (n >= BC_MAX) {
		realloc_bc();
	}
	if (n > BC_POS) {
		BC_POS = n;
	}
	BC[2 * n + 1] = node;
	printf("write check index = %d :value= %d\n", n, BC[2 * n + 1]);
}

char* mem_str(char *area_name, int *max, int init) {
	*max = init;
	char *area = (char*) malloc(sizeof(char) * (*max));
	if (area == NULL) {
		//cout << area_name << " malloc error!" << endl;
	}
	//memset(ids, 0, LONG_LONG);
	memset(area, '\0', *max);
	return area;
}
int arc_index(char ch) {
	return ch - 'a' + 2;
}
void realloc_bc() {
	int i, pre_bc;
	pre_bc = BC_MAX;
	BC_MAX += BC_INC;
	BC = (int*) realloc(BC, sizeof(int) * 2 * BC_MAX);
	if (BC == NULL) {
		//cout << "realloc bc error!" << endl;
		return;
	}
	for (i = 2 * pre_bc; i < 2 * BC_MAX; i++) {
		BC[i] = 0;
	}
	//cout << "realloc bc!" << endl;
}

char* realloc_str(char *area_name, char *area, int *max, int inc) {
	int pre_size;
	int i;
	pre_size = *max;
	*max += inc;
	area = (char*) realloc(area, sizeof(char) * (*max));
	if (area == NULL) {
		printf("%s realloc error!", area_name);
		exit(-1);
	}
	for (i = pre_size; i < *max; i++) {
		area[i] = '\0';
	}

	printf("%s realloc ok!\n", area_name);
	return area;
}

void read_tail(int p) {
	int i = 0;
	while (TAIL[p] != '#')
		TEMP[i++] = TAIL[p++];
	TEMP[i++] = '#';
	TEMP[i] = '\0';
	printf("%s\n", "read tail!");
}

void write_tail(char *temp, int p) {
	int i = 0;
	int tail_index;

	tail_index = p;
	while ((p + strlen(temp)) >= TAIL_MAX - 1) {
		TAIL = realloc_str("TAIL", TAIL, &TAIL_MAX, TAIL_INC);
	}

	while (*(temp + i) != '\0') {
		TAIL[tail_index++] = *(temp + i);
		i++;
	}

	if (p + i + 1 > TAIL_POS) {
		TAIL_POS = p + i;
	}

	printf("%s\n", "write tail!");
}

int x_check(char *list) {
	int i, base_pos = 1, check_pos;
	unsigned char ch;
	i = 0;
	printf("%s\n", "x_check start:");
	do {
		ch = list[i++];
		check_pos = base_pos + ch;  //change
		if (check(check_pos) != 0) {
			base_pos++;
			i = 0;
			continue;
		}
	} while (list[i] != '\0');
	printf("%s\n", "x_check end!");
	return base_pos;
}

char* set_list(int s) {
	char *list = (char*) malloc(MAX_CODE + 1 + 1); // 256个字符 + 1 '\0'
	int i, j = 0, t;
	for (i = MIN_CODE; i < MAX_CODE; i++) {
		t = base(s) + i;
		if (check(t) == s) {
			list[j] = (unsigned char) i; //change
			j++;
		}
	}
	list[j] = '\0';
	printf("set_list: %s\n", list);
	return list;
}

void separate(int s, char *b, int tail_pos) {
	int t = base(s) + (unsigned char) (*b); // change
	b++;
	w_check(t, s);
	w_base(t, (-1) * tail_pos);
	write_tail(b, tail_pos);
}

void bc_insert(int s, char *b) {
	int t;
	char list_s[MAX_CODE + 2];
	char list_t[MAX_CODE + 2];
	printf("%s\n", "bc_insert start:");
	t = base(s) + (unsigned char) (*b); // change
	printf("t= %s\n", t);
	printf(" check(t)= %d\n", check(t));
	if (check(t) != 0) {
		strcpy(list_s, set_list(s));
		strcpy(list_t, set_list(check(t)));
		if (strlen(list_s) + 1 < strlen(list_t)) {
			printf("list_s= %s\n", list_s);
			s = change_bc(s, s, list_s, *b);
		} else {
			printf("list_t= %s\n", list_t);
			s = change_bc(s, check(t), list_t, '\0');
		}
	}

	separate(s, b, TAIL_POS);
	printf("%s\n", "bc_insert end.");
}

int change_bc(int current, int s, char *list, char ch) {
	int i, k, old_node, new_node, old_base;
	char a_list[MAX_CODE + 2];
	old_base = base(s);
	if (ch != '\0') {
		strcpy(a_list, list);
		i = strlen(a_list);
		a_list[i] = ch;
		a_list[i + 1] = '\0';
	} else {
		strcpy(a_list, list);
	}
	w_base(s, x_check(a_list));
	i = 0;
	do {
		old_node = old_base + (unsigned char) (*list); //change
		new_node = base(s) + (unsigned char) (*list);
		printf("old_node= %s ,new_node= %s\n", old_node, new_node);
		w_base(new_node, base(old_node));
		w_check(new_node, s);
		if (base(old_node) > 0) {
			k = base(old_node) + 1;
			while (k - base(old_node) <= MAX_CODE || k < BC_POS) {
				if (check(k) == old_node) {
					w_check(k, new_node);
				}
				++k;
			}
		}
		if (current != s && old_node == current) {
			current = new_node;
		}
		w_base(old_node, 0);
		w_check(old_node, 0);
		list++;
	} while (*list != '\0');
	return current;
}

void tail_insert(int s, char *a, char *b) {
	char list[3];
	unsigned char ch;
	int i = 0;
	int length = 0;
	int t;
	int old_tail_pos;
	old_tail_pos = (-1) * base(s);
	printf("tail_insert: s=%s a= %d b= %d\n", s, a, b);
	while (a[length] == b[length])
		length++;
	while (i < length) {
		ch = a[i++];
		list[0] = ch;
		list[1] = '\0';
		w_base(s, x_check(list));
		t = base(s) + (unsigned char) (ch);
		w_check(t, s);
		s = t;
	}
	list[0] = a[length];
	list[1] = b[length];
	list[2] = '\0';
	w_base(s, x_check(list));
	separate(s, a + length, old_tail_pos);
	separate(s, b + length, TAIL_POS);
}

int search_word(char *p_word) { // if found word, return its base index, if not, return -1
	unsigned char ch;
	int h = -1;
	int s = 1;
	int t;
	printf("begin-search word: %s\n", p_word);
	do {
		++h;
		ch = p_word[h];
		t = base(s) + (unsigned char) (ch);
		if (check(t) != s) {
			printf("end-search word: %s\n", p_word);
			return -1;
		}
		if (base(t) < 0) {
			break;
		}
		s = t;
	} while (*(p_word + h));
	if (p_word[h] != '#')
		read_tail((-1) * base(t));
	if (p_word[h] == '#' || strcmp(TEMP, p_word + h + 1) == 0) {
		printf("end-search word: %s\n", p_word);
		return t;
	} else {
		printf("end-search word: %s\n", p_word);
		return -1;
	}
}

int delete_word(char *p_word) { // if delete given word, return 1, else return 0
	int t = search_word(p_word);
	if (t == -1) {
		return 0;
	} else {
		w_base(t, 0);
		w_check(t, 0);
		return 1;
	}
}

int insert_word(char *p_word) {
	unsigned char ch;
	int h = -1;
	int s = 1;
	int t;
	printf("begin-insert word :%s\n", p_word);
	strcat(p_word, "#");
	do {
		++h;
		ch = *(p_word + h);
		t = base(s) + (unsigned char) (ch);
		if (check(t) != s) {
			printf("s= %s ,t= %s, check(t)= %d\n", s, t, check(t));
			bc_insert(s, p_word + h);
			printf("end-insert word: %s\n", p_word);
			return 1;
		}
		if (base(t) < 0) {
			break;
		}
		s = t;
	} while (*(p_word + h));

	if (p_word[h] != '#')
		read_tail((-1) * base(t));
	if (p_word[h] == '#' || strcmp(TEMP, p_word + h + 1) == 0) {
		return 1;
	}
	if (base(t) != 0) {
		tail_insert(t, TEMP, p_word + h + 1);
		printf("end-insert word : %s\n", p_word);
	}
	return 1;
}

void initialize() {
	BC_MAX = BC_INC;
	BC_POS = 1;
	TAIL_POS = 1;

//	BC = (int*) malloc(sizeof(int) * 2 * BC_MAX);
//	if (BC == NULL) {
//		//cout << "BC malloc error!" << endl;
//		return;
//	}
//	for (int i = 0; i < 2 * BC_MAX; i++) {
//		BC[i] = 0;
//	}
	BC = (int*) calloc(2 * BC_MAX, sizeof(int));
	w_base(1, 1);
	BC_POS = 1;

	printBC(BC);

	TAIL = mem_str("TAIL", &TAIL_MAX, TAIL_INC);
	TAIL[0] = '#';
	TEMP = mem_str("TEMP", &TEMP_MAX, TEMP_INC);
}

