#pragma once
#include "LC.h"
#include "HLL.h"
#include <immintrin.h>
#include <smmintrin.h>
#include <new>
constexpr uint32_t COUNTER_PER_BUCKET = 8;
class Calypso;

struct alignas(32) Bucket
{
	uint32_t key[COUNTER_PER_BUCKET];
	uint32_t val[COUNTER_PER_BUCKET];
	HyperLogLog1* estimator[COUNTER_PER_BUCKET + 1];
	Bucket(uint32_t est_memory_in_byte)
	{
		memset(key, 0, sizeof(uint32_t) * COUNTER_PER_BUCKET);
		memset(val, 0, sizeof(uint32_t) * COUNTER_PER_BUCKET);
		for (size_t i = 0; i < COUNTER_PER_BUCKET + 1; i++)
		{
			estimator[i] = new HyperLogLog1(est_memory_in_byte, 4);
		}
	}
	~Bucket()
	{
		for (size_t i = 0; i < COUNTER_PER_BUCKET + 1; i++)
		{
			delete estimator[i];
		}
	}
	uint32_t match(uint32_t x, uint32_t& pos, bool find_min, bool use_simd)
	{
		uint32_t minVal = 0xFFFFFFFF;

		if (use_simd)
		{
			const __m256i item = _mm256_set1_epi32((int)x);
			__m256i* keys_p = (__m256i*)(key);
			__m256i a_comp = _mm256_cmpeq_epi32(item, keys_p[0]);
			int matched = _mm256_movemask_ps(*(__m256*) & a_comp);

			/* if matched */
			if (matched != 0) {
				//return 32 if input is zero;
				pos = ilog2_kf(matched & (-matched));

				return 0;
			}
			if (find_min) {
				/* find the minimal bucket */
				const uint32_t mask_base = 0x7FFFFFFF;
				const __m256i* counters = (__m256i*)(val);
				__m256i masks = _mm256_set1_epi32(mask_base);
				__m256 results = (_mm256_and_ps(*(__m256*)counters, *(__m256*) & masks));

				__m128i low_part = _mm_castps_si128(_mm256_extractf128_ps(results, 0));
				__m128i high_part = _mm_castps_si128(_mm256_extractf128_ps(results, 1));

				__m128i x = _mm_min_epi32(low_part, high_part);
				__m128i min1 = _mm_shuffle_epi32(x, _MM_SHUFFLE(0, 0, 3, 2));
				__m128i min2 = _mm_min_epi32(x, min1);
				__m128i min3 = _mm_shuffle_epi32(min2, _MM_SHUFFLE(0, 0, 0, 1));
				__m128i min4 = _mm_min_epi32(min2, min3);
				uint32_t min_counter_val = _mm_cvtsi128_si32(min4);

				const __m256i ct_item = _mm256_set1_epi32(min_counter_val);
				int ct_matched = 0;

				a_comp = _mm256_cmpeq_epi32(ct_item, *(__m256i*) & results);
				matched = _mm256_movemask_ps(*(__m256*) & a_comp);

				pos = ilog2_kf(matched & (-matched));

				/* if there has empty bucket */
				return min_counter_val != 0;
			}
			else
			{
				return 1;
			}
		}
		else // USING_SIMD_ACCELERATION
		{
			for (size_t i = 0; i < COUNTER_PER_BUCKET; i++)
			{
				if (key[i] == x)
				{
					pos = i;
					return 0;
				}
				else if (find_min && (val[i] & 0x7FFFFFFF) < minVal)
				{
					pos = i;
					minVal = (val[i] & 0x7FFFFFFF);
				}
			}
			if (find_min)
			{
				return minVal != 0;
			}
			else
			{
				return 1;
			}
		}
	}
};
class Classifier
{
public:
	Classifier(uint32_t memory_in_byte, uint32_t _T, uint32_t _single_flow_mem_in_byte, bool _use_simd = true, bool _use_mul_hash = true)
		: T(_T), single_flow_mem_in_byte(_single_flow_mem_in_byte), use_simd(_use_simd), use_mul_hash(_use_mul_hash)
	{
		length = memory_in_byte / (single_flow_mem_in_byte * (COUNTER_PER_BUCKET + 1) + sizeof(uint32_t) * COUNTER_PER_BUCKET);
		buckets = new Bucket * [length];
		for (size_t i = 0; i < length; i++)
		{
			buckets[i] = new Bucket(single_flow_mem_in_byte);
		}
	}

	~Classifier()
	{
		for (size_t i = 0; i < length; i++)
		{
			delete buckets[i];
		}
		delete[] buckets;
	}
	std::pair<uint32_t, HyperLogLog1*>* insert(std::pair<uint32_t, uint32_t> p, uint32_t& ret)
	{
		uint32_t x = p.first, y = p.second;
		auto q = str_hash128(x, 0);
		uint32_t pos = q[0] % length, min_counter = 0, min_counter1 = 0, minVal = 0, pos1 = (use_mul_hash) ? q[1] % length : 0;
		if (!buckets[pos]->match(x, min_counter, true, use_simd))
		{
			buckets[pos]->key[min_counter] = x;
			buckets[pos]->estimator[min_counter]->insert(((uint64_t)x << 32llu) + y);
			buckets[pos]->val[min_counter] = (buckets[pos]->val[min_counter] & 0x80000000) + buckets[pos]->estimator[min_counter]->get_cardinality();
			ret = 0;
			return NULL;
		}
		else if (use_mul_hash && !buckets[pos1]->match(x, min_counter1, true, use_simd))
		{
			buckets[pos1]->key[min_counter1] = x;
			buckets[pos1]->estimator[min_counter1]->insert(((uint64_t)x << 32llu) + y);
			buckets[pos1]->val[min_counter1] = (buckets[pos1]->val[min_counter1] & 0x80000000) + buckets[pos1]->estimator[min_counter1]->get_cardinality();
			ret = 0;
			return NULL;
		}
		else
		{
			if (use_mul_hash && buckets[pos]->val[min_counter] > buckets[pos1]->val[min_counter1])
			{
				pos = pos1;
				min_counter = min_counter1;
				minVal = buckets[pos1]->val[min_counter1];
			}
			else
			{
				minVal = buckets[pos]->val[min_counter];
			}
			buckets[pos]->estimator[COUNTER_PER_BUCKET]->insert(((uint64_t)x << 32llu) + y);
			if (T * minVal < buckets[pos]->estimator[COUNTER_PER_BUCKET]->get_cardinality())
			{
				ret = 1;
				auto ret1 = new std::pair<uint32_t, HyperLogLog1*>(std::make_pair(buckets[pos]->key[min_counter], buckets[pos]->estimator[min_counter]));
				buckets[pos]->key[min_counter] = x;
				buckets[pos]->estimator[min_counter] = new HyperLogLog1(single_flow_mem_in_byte, 4);
				buckets[pos]->estimator[min_counter]->insert(((uint64_t)x << 32llu) + y);
				buckets[pos]->val[min_counter] = 0x80000001;
				buckets[pos]->estimator[COUNTER_PER_BUCKET]->clear();
				return ret1;
			}
			else
			{
				ret = 2;
				return NULL;
			}
		}
	}
	uint32_t query(uint32_t x) const
	{
		auto q = str_hash128(x, 0);
		uint32_t pos = q[0] % length, min_counter = 0, min_counter1 = 0, minVal = 0, pos1 = (use_mul_hash) ? q[1] % length : 0;
		if (!buckets[pos]->match(x, min_counter, false, use_simd))
		{
			return buckets[pos]->val[min_counter];
		}
		if (use_mul_hash && !buckets[pos1]->match(x, min_counter1, false, use_simd))
		{
			return buckets[pos1]->val[min_counter1];
		}
		return 0;
	}
	LinearCount* get_estimator_in_LC(uint32_t x)
	{
		auto q = str_hash128(x, 0);
		uint32_t pos = q[0] % length, min_counter = 0, min_counter1 = 0, minVal = 0, pos1 = (use_mul_hash) ? q[1] % length : 0;
		if (!buckets[pos]->match(x, min_counter, false, use_simd))
		{
			return LinearCount::transfer_to_LC(buckets[pos]->estimator[min_counter]);
		}
		if (use_mul_hash && !buckets[pos1]->match(x, min_counter1, false, use_simd))
		{
			return LinearCount::transfer_to_LC(buckets[pos1]->estimator[min_counter1]);
		}
		return NULL;
	}
private:
	uint32_t length, T, single_flow_mem_in_byte;
	Bucket** buckets;
	bool use_simd, use_mul_hash;
	friend class Calypso;
};
