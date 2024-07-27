#ifndef _HASHFUNC_H_
#define _HASHFUNC_H_

#include"MurmurHash3.h"
#include<string>
#include<array>
#include<iostream>

#define HASH_SEED_LAYER1 92317
#define HASH_SEED_LAYER2 37361
#define HASH_SEED_LAYER3 52813
template<typename T>
uint32_t str_hash32(T input_str, uint32_t hashseed) {
	uint32_t hash_res;
	MurmurHash3_x86_32(&input_str, sizeof(input_str), hashseed, &hash_res);
	return hash_res;
}
template<typename T>
std::array<uint64_t, 2> str_hash128(T input_str, uint32_t hashseed) {
	uint64_t hash_res[2];
	MurmurHash3_x64_128(&input_str, sizeof(input_str), hashseed, hash_res);
	return std::array<uint64_t, 2>{hash_res[0], hash_res[1]};
}

#endif