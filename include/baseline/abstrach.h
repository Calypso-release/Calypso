#pragma once
#include<map>
#include<string.h>
#include<time.h>
#ifdef WIN32
#include <windows.h>
#endif

#include "../common/hashfunc.h"
#include "../common/utils.h"
class SketchBase
{
public:
	SketchBase(std::string _name) :name(_name)
	{
		insert_time = query_time = 0;
	}

	virtual ~SketchBase()
	{
	}
	virtual void insert(uint32_t flowid, uint32_t element) = 0;
	virtual uint32_t get_flow_cardinality(uint32_t flowid) = 0;
	virtual std::map<uint32_t, uint32_t> get_heavy_hitter(double threshold) = 0;
	std::string name;
	uint64_t insert_time, query_time;
protected:
#ifdef WIN32

	void start_timing()
	{
		QueryPerformanceFrequency(&nFreq);
		QueryPerformanceCounter(&t1);
	}
	double stop_timing()
	{
		QueryPerformanceCounter(&t2);
		return (double)(t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart * 1000000000;
	}
	void start_timing(LARGE_INTEGER& t1)
	{
		QueryPerformanceCounter(&t1);
	}
	double stop_timing(LARGE_INTEGER t1)
	{
		LARGE_INTEGER t2, nFreq;
		QueryPerformanceCounter(&t2);
		QueryPerformanceFrequency(&nFreq);
		return (double)(t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart * 1000000000;
	}
	LARGE_INTEGER nFreq, t1, t2;
#else
	void start_timing()
	{
		clock_gettime(CLOCK_MONOTONIC, &t1);
	}
	double stop_timing()
	{
		clock_gettime(CLOCK_MONOTONIC, &t2);
		return (t2.tv_sec - t1.tv_sec) * 1000000000 + (t2.tv_nsec - t1.tv_nsec);
	}
	void start_timing(timespec& t1)
	{
		clock_gettime(CLOCK_MONOTONIC, &t1);
	}
	double stop_timing(timespec t1)
	{
		timespec t2;
		clock_gettime(CLOCK_MONOTONIC, &t2);
		return (t2.tv_sec - t1.tv_sec) * 1000000000 + (t2.tv_nsec - t1.tv_nsec);
	}
	timespec t1, t2;
#endif
private:
};
