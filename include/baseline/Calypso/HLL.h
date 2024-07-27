#pragma once
#include"../../common/packed_array.h"
inline int ilog2_kf(int n) {
	float q = (float)n;
	return ((*(int*)&q) >> 23) - 127;
}
class HyperLogLog1
{
public:
	HyperLogLog1(uint32_t memory_in_byte, uint32_t _bit_width = 5)
	{
		bit_width = _bit_width;
		length = memory_in_byte / sizeof(uint32_t) * (32 / bit_width);
		array = new uint32_t[memory_in_byte / sizeof(uint32_t)];
		max_value = (1 << bit_width) - 1;
		int p = log2(length);
		switch (p) {
		case 4:constant = 0.673; break;
		case 5:constant = 0.697; break;
		case 6:constant = 0.709; break;
		default:constant = (0.7213 / (1 + 1.079 / length)); break;
		}
		clear();
	}

	~HyperLogLog1()
	{
		delete[] array;
	}
	void insert(uint64_t x)
	{
		uint32_t q = str_hash32(x, 0);
		uint32_t pos = q % length;
		int val = (q / length) % (1 << max_value), init_val = ACCESS32(array, bit_width, pos), leading_zero = (val) ? ilog2_kf(val & (-val)) + 1 : max_value;
		if (init_val < leading_zero)
		{
			sum += 1.0 / (1 << leading_zero) - 1.0 / (1 << init_val);
			zero_cnt -= init_val == 0;
			WRITE32(array, bit_width, pos, leading_zero);
			estimate = constant * length * length / sum;
			if (zero_cnt)
			{
				estimate = (std::min)(estimate, length * log(1.0 * length / zero_cnt) + 0.51);
			}
		}
	}
	uint32_t get_cardinality() const
	{
		return estimate;
	}
	void clear()
	{
		memset(array, 0, (length / (32 / bit_width)) * 4);
		sum = length;
		zero_cnt = length;
		estimate = 0;
	}
	uint32_t* array;
	uint64_t length, bit_width, max_value, zero_cnt;
private:
	double constant, sum, estimate;
};
