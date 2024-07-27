#pragma once
#ifndef _UTILITY_H_
#define _UTILITY_H_
#include<algorithm>
#include<fstream>
#include<cstring>
#include<set>
#include<iostream>
#include"BOBHash.h"
#include<queue>

#define MAX_THREAD 28
typedef uint32_t count_type;
#define sqr(x) ((x)*(x))
#define CAIDA_LEN 13
struct CAIDA_TUPLES {
	uint8_t array[CAIDA_LEN];
};
bool operator == (const CAIDA_TUPLES& a, const CAIDA_TUPLES& b) {
	return memcmp(a.array, b.array, sizeof(CAIDA_TUPLES)) == 0;
}
struct TIMESTAMP {
	uint8_t array[8];
	operator double() { return *(double*)array; }
};
#endif