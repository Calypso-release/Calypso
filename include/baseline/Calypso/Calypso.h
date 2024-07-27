#pragma once
#include "BloomFilter.h"
#include "Classifier.h"
#include<map>
#include"../abstrach.h"
class Calypso :public SketchBase
{
public:
	Calypso(uint32_t memory_in_byte, double heavy_ratio, uint32_t T, uint32_t single_flow_mem_in_byte,
		uint32_t light_flow_single_mem_in_byte, uint32_t _hsh_num = 3, bool _use_IRO = true, bool _use_simd = true, bool _use_mul_hash = true)
		:SketchBase("Calypso"), light_hsh_num(_hsh_num), use_IRO(_use_IRO)
	{
		heavy_part = new Classifier(memory_in_byte * heavy_ratio, T, single_flow_mem_in_byte, _use_simd, _use_mul_hash);
		light_length = memory_in_byte * (1 - heavy_ratio) / light_flow_single_mem_in_byte;
		light_part = new LinearCount * [light_length];
		for (size_t i = 0; i < light_length; i++)
		{
			light_part[i] = new LinearCount(light_flow_single_mem_in_byte);
		}
		insert_time1 = insert_time2 = 0;
	}

	virtual ~Calypso()
	{
		delete heavy_part;
		for (size_t i = 0; i < light_length; i++)
		{
			delete light_part[i];
		}
		delete[] light_part;
	}
	virtual void insert(uint32_t flowid, uint32_t element)
	{
		start_timing();
		std::pair<uint32_t, uint32_t> p(std::make_pair(flowid, element));
		uint32_t ret;
		auto ret1 = heavy_part->insert(p, ret);
		insert_time1 += stop_timing();
		start_timing();
		if (ret == 0)
		{
			insert_time2 += stop_timing();
			insert_time = insert_time1 + insert_time2;
			return;
		}
		else if (ret == 1)
		{
			for (size_t i = 0; i < light_hsh_num; i++)
			{
				uint32_t pos = str_hash32(ret1->first, i) % light_length;
				light_part[pos]->merge(ret1->second);
			}
			delete ret1->second;
			delete ret1;
		}
		else
		{
			uint64_t x1 = ((uint64_t)p.first << 32llu) + p.second;
			auto q = str_hash32(x1, 0);
			for (size_t i = 0; i < light_hsh_num; i++)
			{
				uint32_t pos = str_hash32(p.first, i) % light_length;
				light_part[pos]->insert(x1, q);
			}
		}
		insert_time2 += stop_timing();
		insert_time = insert_time1 + insert_time2;
	}
	virtual uint32_t get_flow_cardinality(uint32_t x)
	{
		start_timing();
		bool first = false;
		auto ret = heavy_part->query(x);
		if (ret == 0)
		{
			std::vector<LinearCount*> alts;
			for (size_t i = 0; i < light_hsh_num; i++)
			{
				alts.push_back(light_part[str_hash32(x, i) % light_length]);
			}
			LinearCount* ret1 = NULL;
			ret = light_part[0]->group(alts, ret1);
			delete ret1;
		}
		else if (ret & 0x80000000)
		{
			ret ^= 0x80000000;
			std::vector<LinearCount*> alts;
			for (size_t i = 0; i < light_hsh_num; i++)
			{
				alts.push_back(light_part[str_hash32(x, i) % light_length]);
			}
			LinearCount* ret1(NULL), * ret3(NULL), * ret4(NULL);
			auto ret2 = LinearCount::group(alts, ret1);
			ret3 = heavy_part->get_estimator_in_LC(x);
			uint32_t ret5 = 0;
			if (use_IRO)
			{
				alts.clear();
				alts.push_back(ret1);
				alts.push_back(ret3);
				ret5 = LinearCount::group(alts, ret4);
				if (ret5)
					ret5 = (ret5 - 1) / 2;
			}
			else
			{
				ret5 = 0;
			}
			ret = ret + ret2 - ret5;
			delete ret1;
			delete ret3;
			delete ret4;
		}
		query_time += stop_timing();
		return ret;
	}
	std::map<uint32_t, uint32_t> get_heavy_hitter(double threshold)
	{
		std::map<uint32_t, uint32_t> ret;
		for (size_t i = 0; i < heavy_part->length; i++)
		{
			for (size_t j = 0; j < COUNTER_PER_BUCKET; j++)
			{
				//auto est = heavy_part->array[i][j]->estimator->get_cardinality();
				auto est = get_flow_cardinality(heavy_part->buckets[i]->key[j]);
				if (est >= threshold)
				{
					ret[heavy_part->buckets[i]->key[j]] = est;
				}
			}
		}
		return ret;
	}
	uint64_t insert_time1, insert_time2;
private:
	LinearCount** light_part;
	Classifier* heavy_part;
	uint32_t light_length, light_hsh_num;
	bool use_IRO;
};
