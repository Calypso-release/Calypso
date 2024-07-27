#pragma once
#include<map>
#include<vector>
#include<string>
#include<sstream>
#include<pugixml.hpp>
#include<algorithm>
#include<random>
#include"util.h"
template<class data_type>
data_type* read_standard_data(const char* PATH, uint32_t interval, uint32_t* cnt, const uint32_t length = 0x7FFFFFFF) {
	FILE* data = fopen(PATH, "rb");
	if (data == NULL)
	{
		std::cout << "File don't exist\n";
		exit(0);
	}
	*cnt = 0;
	uint32_t t = sizeof(data_type);
	std::vector<data_type> ret;
	TIMESTAMP timestamp;
	uint8_t* it = new uint8_t[interval];
	while (fread(it, interval, 1, data) > 0 && *cnt < length)
	{
		ret.push_back(*(data_type*)it);
		(*cnt)++;
	}
	data_type* ret1 = new data_type[*cnt];
	std::copy(ret.begin(), ret.end(), ret1);
	fclose(data);
	return ret1;
}
std::vector<std::pair<uint32_t, uint32_t>> read_zipf(const char* PATH, uint32_t MAX_LENGTH = 0xFFFFFFFF)
{
	std::cout << "Reading File " << PATH << std::endl;
	uint32_t cnt = 0, cnt1 = 0;
	std::vector<std::pair<uint32_t, uint32_t>> ret;
	auto ret1 = read_standard_data<uint32_t>(PATH, 4, &cnt, MAX_LENGTH);
	uint32_t* ret2 = new uint32_t[cnt];
	std::copy(ret1, ret1 + cnt, ret2);
	std::random_shuffle(ret2, ret2 + cnt);
	for (size_t i = 0; i < cnt; i++)
	{
		ret.push_back(std::make_pair(ret1[i], ret2[i]));
	}
	delete[] ret1;
	std::cout << " total " << ret.size() << " pair \n";
	return ret;
}
std::vector<std::pair<uint32_t, uint32_t>> read_CAIDA(const char* PATH, uint32_t MAX_LENGTH = 0xFFFFFFFF)
{
	std::cout << "Reading File " << PATH << std::endl;
	uint32_t cnt = 0, cnt1 = 0;
	std::vector<std::pair<uint32_t, uint32_t>> ret;
	auto ret1 = read_standard_data<CAIDA_TUPLES>(PATH, 21, &cnt, MAX_LENGTH);
	for (size_t i = 0; i < cnt; i++)
	{
		uint32_t src = *(uint32_t*)ret1[i].array, dst = *(uint32_t*)(ret1[i].array + 4);
		ret.push_back(std::make_pair(src, dst));
	}
	delete[] ret1;
	std::cout << " total " << ret.size() << " pair \n";
	return ret;
}
std::vector<std::pair<uint32_t, uint32_t>> read_twitter(const char* PATH, uint32_t MAX_LENGTH = 0xFFFFFFFF)
{
	std::ifstream fin(PATH);
	if (!fin)
	{
		std::cout << "File don't exist\n";
		exit(0);
	}
	else
	{
		std::cout << "Reading File " << PATH << std::endl;
	}
	uint32_t src, dst, cnt = 0, cnt1 = 0;
	std::vector<std::pair<uint32_t, uint32_t>> ret;
	std::string s;
	while (getline(fin, s))
	{
		if (s.size() == 0) break;
		if (s[0] < 48 || s[0]>57) continue;
		std::stringstream is(s);
		is >> src >> dst;
		ret.push_back(std::make_pair(src, dst));
		cnt++;
		if (cnt == MAX_LENGTH) break;
	}
	//std::mt19937 gen(3);
	//std::shuffle(ret.begin(), ret.end(), gen);
	fin.close();
	std::cout << " total " << ret.size() << " pair \n";
	return ret;
}
std::vector<std::pair<uint32_t, uint32_t>> read_data(const char* PATH, uint32_t type)
{
	switch (type)
	{
	case 0:return read_CAIDA(PATH);
	case 1:return read_zipf(PATH);
	case 2:return read_twitter(PATH);
		break;
	}
}