#pragma once
#include "HLL.h"
class LinearCount
{
public:
	LinearCount(uint32_t memory_in_byte)
	{
		array = new uint8_t[memory_in_byte];
		length = memory_in_byte * 8;
		clear();
	}
	~LinearCount()
	{
		delete[] array;
	}
	void insert(uint64_t x)
	{
		uint32_t pos = str_hash32(x, 0) % length;
		zero_cnt -= (array[pos >> 3] >> (pos & 7)) == 0;
		array[pos >> 3] |= 1 << (pos & 7);
	}
	void insert(uint64_t x, uint32_t pos)
	{
		pos %= length;
		zero_cnt -= (array[pos >> 3] >> (pos & 7)) == 0;
		array[pos >> 3] |= 1 << (pos & 7);
	}
	uint32_t get_cardinality() const
	{
		return length * log(1.0 * length / (std::max)(1u, zero_cnt)) + 0.51;
	}
	double zero_rate() const
	{
		return 1.0 * zero_cnt / length;
	}
	void merge(HyperLogLog1* src)
	{
#pragma unroll
		for (size_t i = 0; i < src->length; i++)
			if (ACCESS32(src->array, src->bit_width, i) > 0)
			{
				array[i % length / 8] |= 1 << (i & 7);
			}
		zero_cnt = 0;
#pragma unroll
		for (size_t i = 0; i < length; i++)
			if ((array[i >> 3] & (1 << (i & 7))) == 0)
			{
				zero_cnt++;
			}
	}
	void clear()
	{
		memset(array, 0, length / 8);
		zero_cnt = length;
	}
	static uint32_t group(std::vector<LinearCount*> alts, LinearCount*& ret1)
	{
		double sum = 0, ret = 0, zero_cnt = 0;
		uint32_t length = 0xFFFFFFFF;
		for (auto& p : alts)
		{
			length = (std::min)(length, p->length);
		}
		ret1 = new LinearCount(length / 8);
		memset(ret1->array, 255, length / 8);
		uint8_t** tmp = new uint8_t * [alts.size()];
		for (size_t i = 0; i < alts.size(); i++)
		{
			tmp[i] = new uint8_t[length / 8];
			memset(tmp[i], 0, length / 8);
			for (size_t j = 0; j < alts[i]->length / 8; j++)
			{
				tmp[i][j % (length / 8)] |= alts[i]->array[j];
			}
		}
		/*for (size_t i = 0; i < p->length / 8; i++)
		{
			uint32_t x = 0;
			for (auto& p : alts)
			{
				x |= p->array[i];
			}
			ret1->array[i % (length / 8)] &= x;
		}*/
		for (size_t i = 0; i < length / 8; i++)
		{
			uint32_t x = 255;
			for (size_t j = 0; j < alts.size(); j++)
			{
				x &= tmp[j][i];
			}
			ret1->array[i] = x;
			for (size_t j = 0; j < 8; j++)
			{
				zero_cnt += 1 - (x & 1);
				x >>= 1;
			}
		}
		/*for (size_t i = 0; i < ret1->length / 8; i++)
		{
			int x = 255;
			for (auto& p : alts)
			{
				x &= p->array[i];
			}
			ret1->array[i] = x;
			for (size_t j = 0; j < 8; j++)
			{
				zero_cnt += 1 - (x & 1);
				x >>= 1;
			}
		}*/
		for (size_t i = 0; i < alts.size(); i++)
		{
			delete[] tmp[i];
		}
		delete[] tmp;
		ret1->zero_cnt = zero_cnt;
		return  length * log(1.0 * length / (std::max)(1.0, zero_cnt)) + 0.51;
	}
	static LinearCount* transfer_to_LC(HyperLogLog1* b)
	{
		LinearCount* ret = new LinearCount(b->length / 8);
		for (size_t i = 0; i < b->length; i++)
			if (ACCESS32(b->array, b->bit_width, i) > 0)
			{
				ret->array[i / 8] |= 1 << (i & 7);
			}
		return ret;
	}
private:
	uint8_t* array;
	uint32_t length, zero_cnt;
};
