#pragma once
#ifndef _COUPER_H_
#define _COUPER_H_

#include<iostream>
#include<bitset>
#include<cmath>
#include<string>
#include<fstream>
#include<array>
#include<memory>
#include<vector>
#include<set>
#include<bitset>
#include<map>
#include<unordered_map>
#include"../abstrach.h"
using std::cin;
using std::unordered_map;
class Bitmap_Arr {       // layer1: T_B
public:
	uint32_t memory;        //kB
	static const uint32_t bitmap_size = 12;      // b=12 (bits)
	static const uint32_t capacity = 9;          // tau = 9
	uint32_t bitmap_num;      //L_1
	std::vector<uint32_t> raw;
	std::array<uint32_t, bitmap_size> patterns;
	std::array<double, bitmap_size + 1> card_dict;
	static const uint32_t FULL_PAT = (1 << bitmap_size) - 1;
	static constexpr double thresh_ratio = 1.256 / 2;  //error removal
#define BITMAP_FULL_FLAG -1

	Bitmap_Arr(uint32_t memory_);
	uint32_t get_bitmap_1(uint32_t bitmap_pos);
	uint32_t get_bitmap_2(uint32_t bitmap_pos);
	bool check_flow_full(std::array<uint64_t, 2>& hash_flowid);
	bool set_bit_1(uint32_t bit_pos);
	bool set_bit_2(uint32_t bit_pos);
	bool process_packet(std::array<uint64_t, 2>& hash_flowid, uint32_t l2_update_pos);    // For Couper.
	bool process_packet(std::array<uint64_t, 2>& hash_flowid, std::array<uint64_t, 2>& hash_element);    // For existing supser spreader detection algorithms.
	int get_cardinality(uint64_t flowid, std::array<uint64_t, 2>& hash_flowid, uint32_t& bm_merged);
};
class Estimator {
public:
	uint16_t digest;
	static uint32_t get_size();
	virtual int update(uint32_t bitstr, uint32_t inner_update_pos) = 0;
	virtual int query() = 0;
	static uint32_t query_virtual_Estimator(Estimator& estimator1, Estimator& estimator2);
	static uint32_t query_merge_Estimator(Estimator& estimator1, Estimator& estimator2);
	virtual uint32_t generate_bitmap() = 0;
};

class HyperLogLog : public Estimator {
public:
	static const uint32_t register_num = 64;    //m
	static const uint32_t LEN = register_num;
	static const uint32_t register_size = 4;
	static const uint32_t HLL_size = register_num * register_size;
	static std::array<double, 1 << register_size> exp_table;
	static constexpr double thresh_ratio = 2.103 / 2;
	static double alpha_m, alpha_m_sqm, LC_thresh;
	std::vector<uint8_t> HLL_raw;

public:
	HyperLogLog();
	static void shared_param_Init();
	static uint32_t get_size();
	int update(uint32_t bitstr, uint32_t inner_update_pos);
	int query();
	static HyperLogLog query_merged_Estimator(HyperLogLog& HLL1, HyperLogLog& HLL2);
	uint32_t get_counter_val(uint32_t index);
	int try2update_counter(uint32_t index, uint32_t val_);
	uint32_t generate_bitmap();
};

class MultiResBitmap : public Estimator {
public:
	static constexpr double sigma = 0.2;
	static const uint32_t b = 0.6367 / (sigma * sigma);
	static const uint32_t LEN = b;
	static const uint32_t b_hat = 2 * b;
	static const uint32_t C = 120000;
	static uint32_t c; // = log2(C / (2.6744 * b)); //== 9.47
	static constexpr double setmax_ratio = 0.9311;
	static uint32_t mrbitmap_size;  // = b * (c - 1) + b_hat;
	std::vector<std::vector<uint8_t>> bitmaps;

public:
	MultiResBitmap();
	static void shared_param_Init();
	static uint32_t get_size();
	uint32_t get_ones_num(uint32_t layer);
	int update(uint32_t bitstr, uint32_t inner_update_pos);
	int query();
	static MultiResBitmap query_merged_Estimator(MultiResBitmap& mrb1, MultiResBitmap& mrb2);
	uint32_t generate_bitmap();
};

template<class EstimatorType>
class Layer2 {
public:
	uint32_t memory;
	uint32_t esti_num;
	std::vector<EstimatorType> estimators;
	uint32_t inner_update_pos;
	//T_sc
	class Table_Entry {
	public:
		uint32_t flowid;
		uint16_t digest;
		Table_Entry() : flowid(0), digest(0) {}
	};
	static const uint32_t table_mem = 10; //KB
	static const uint32_t tab_size = 2048; //table_mem * 1024 * 8 / (8 + 32);
	std::vector<Table_Entry> hash_table;

public:
	Layer2(uint32_t memory_);
	int get_inner_update_pos(uint32_t hashres32);
	void process_packet(uint32_t flowid, std::array<uint64_t, 2>& hash_flowid, uint32_t hashres32);
	void insert_hashtab(uint32_t flowid, int flow_digest, uint64_t hahsres64);
	uint32_t get_cardinality(uint32_t flowid, std::array<uint64_t, 2>& hash_flowid, uint32_t& layer2_bm);
};

class Couper :public SketchBase {
public:
	Bitmap_Arr layer1;     //T_B
	Layer2<HyperLogLog> layer2;        //T_H
	double layer1_ratio;   //pi
	//KB
	Couper(uint32_t memory_size, double layer1_ratio_ = 0.6) : SketchBase("Couper"), layer1_ratio(layer1_ratio_),
		layer1(memory_size* layer1_ratio_), layer2(memory_size* (1 - layer1_ratio_)) {
	}
	virtual void insert(uint32_t flowid, uint32_t element);
	int get_overlapping_bias(uint32_t bm1, uint32_t bm2);
	virtual uint32_t get_flow_cardinality(uint32_t flowid);
	void report_superspreaders(std::vector<IdSpread>& superspreaders);
	virtual std::map<uint32_t, uint32_t> get_heavy_hitter(double threshold);
	virtual ~Couper()
	{
	}
};

#endif