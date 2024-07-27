#ifndef _V_HLL_
#define _V_HLL_

#include<vector>
#include<iostream>
#include<array>
#include<algorithm>
#include<set>
#include"../abstrach.h"
#include"../Couper/Couper.h"

#define HASH_SEED_1 92317
#define HASH_SEED_2 37361
class VHS :public SketchBase {
public:
	uint32_t memory;
	static const uint32_t HLL_size = 128;
	static const uint32_t glb_HLL_size = 1024;
	static const uint32_t register_size = 5;
	uint32_t register_num;
	std::vector<uint32_t> raw;
	std::vector<uint8_t> global_HLL;

	VHS(uint32_t mem, double _heap_ratio);
	virtual void insert(uint32_t flowid, uint32_t element);
	uint8_t get_register(uint32_t reg_pos);
	void set_register(uint32_t reg_pos, uint8_t val);
	uint32_t get_spread(std::vector<uint8_t> virtual_HLL);
	virtual uint32_t get_flow_cardinality(uint32_t flowID);
	virtual std::map<uint32_t, uint32_t> get_heavy_hitter(double threshold) { return inserted; }
	bool DETECT_SUPERSPREADER = true;    //detect superspreaders
	uint32_t heap_size;
	/*std::set<uint32_t> inserted;
	std::vector<IdSpread> heap;*/
	std::map<uint32_t, uint32_t> inserted;
	std::set<IdSpread> heap;
};

#endif