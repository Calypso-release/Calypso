#ifndef _RERSKT_H_
#define _RERSKT_H_

#include<iostream>
#include<array>
#include<string>
#include<math.h>
#include<vector>
#include<algorithm>
#include<set>
#include"../../common/utils.h"
#include"../../common/hashfunc.h"
#include"../abstrach.h"

#define HASH_SEED_1 92317
#define HASH_SEED_2 37361
// #define HASH_SEED_3 52813

class HLL {
public:
	static const uint32_t register_num = 128;
	static const uint32_t register_size = 5;
	static const uint32_t size = register_num * register_size;
	static const uint32_t HLL_size = register_num * register_size;
	static constexpr double alpha_m = 0.7213 / (1 + 1.079 / 128);
	std::array<uint8_t, register_num> HLL_registers{};

public:
	uint8_t get_leading_zeros(uint32_t bitstr);
	void record_element(uint32_t hash_elem, uint32_t unit_pos);
	void record_element(uint32_t hashres);
	static int get_spread(std::array<uint8_t, register_num> virtual_HLL);
	int get_spread();
	void set_unit(uint32_t pos, uint32_t val);
	void reset();
	uint32_t get_unitval(uint32_t pos) { return HLL_registers[pos]; }
	uint32_t get_unit_num() { return register_num; }
	HLL() { reset(); }
};

class Bitmap {
public:
	static const uint32_t bitnum = 5000;
	static const uint32_t size = bitnum;
	std::array<uint8_t, bitnum / 8> raw{};

public:
	Bitmap() { reset(); }
	uint32_t get_unit_num() { return bitnum; }
	void record_element(uint32_t hash_elem, uint32_t unit_pos);
	void record_element(uint32_t hashres);
	uint32_t get_unitval(uint32_t bitpos);
	int get_spread();
	void set_unit(uint32_t pos, uint32_t val);
	void reset();
};

template<class Estimator>
class RerSkt :public SketchBase {
private:
	uint32_t memory;  //kB
	uint32_t table_size;
	std::vector<Estimator> table1;
	std::vector<Estimator> table2;

	bool DETECT_SUPERSPREADER = true;    //detect super spreaders with piggyback
	uint32_t heap_size;
	/*std::set<uint32_t> inserted;
	std::vector<IdSpread> heap;*/
	std::map<uint32_t, uint32_t> inserted;
	std::set<IdSpread> heap;

public:
	void insert(uint32_t flowid, uint32_t element);
	uint32_t get_flow_cardinality(uint32_t flowid);
	RerSkt(uint32_t memory_, double _heap_ratio);
	std::map<uint32_t, uint32_t> get_heavy_hitter(double threshold) { return inserted; }
};

#endif