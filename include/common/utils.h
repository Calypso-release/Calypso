#pragma once
#ifndef UTILS_H
#define UTILS_H

#include<iostream>
#include<sstream>
#include<fstream>
#include<bitset>
#include<string>
#include<unordered_map>
#include<vector>
#include<cmath>

#define MAX_UINT8 255
#define MAX_UINT16 65535
#define MAX_UINT32 4294967295

#define HASH_SEED_1 92317
#define HASH_SEED_2 37361
#define HASH_SEED 92317

uint32_t get_one_num(uint8_t val);

/* A min-heap is used to record super spreaeder candidates for piggyback strategy in gSkt, rerSkt and VHS*/

std::string Uint32toIPstr(uint32_t val);

uint32_t IPstrtoUint32(std::string IPstr);

uint32_t get_leading_zeros(uint32_t bitstr);
struct IdSpread {
public:
	uint32_t flowID;
	uint32_t spread;   // spread means cardinality
	IdSpread() {}
	IdSpread(uint32_t str, uint32_t s) { flowID = str; spread = s; }
};
inline bool operator <(const IdSpread& x, const IdSpread& y) {
	return x.spread < y.spread || (x.spread == y.spread && x.flowID < y.flowID);
}
inline bool operator ==(const IdSpread& x, const IdSpread& y) {
	return x.spread == y.spread && x.flowID == y.flowID;
}
struct MinHeapCmp {
	inline bool operator()(const IdSpread& x, const IdSpread& y) {
		return x.spread > y.spread;
	}
};

bool IdSpreadComp(IdSpread& a, IdSpread& b);

#endif // !UTILS_H