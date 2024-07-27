#include"utils.h"
uint32_t get_one_num(uint8_t val) {
	std::bitset<8> tmp(val);
	return tmp.count();
}

/* A min-heap is used to record super spreaeder candidates for piggyback strategy in gSkt, rerSkt and VHS*/

std::string Uint32toIPstr(uint32_t val) {
	std::string ret = "";
	for (size_t i = 0; i < 4; i++) {
		uint8_t tmpval = (val >> (i * 8)) & 255;
		std::string tmpstr = std::to_string(tmpval);
		ret = (std::string(3 - tmpstr.length(), '0') + tmpstr) + ret;
	}
	return ret;
}

uint32_t IPstrtoUint32(std::string IPstr) {
	uint32_t ret = 0;
	for (size_t i = 0; i < 4; i++) {
		uint32_t tmp = std::stoi(IPstr.substr(i * 3, 3));
		ret = (ret << 8) + tmp;
	}
	return ret;
}

uint32_t get_leading_zeros(uint32_t bitstr) {
	uint32_t tmp = 2147483648;   //1<<31
	for (size_t i = 0; i < 32; i++) {
		if ((bitstr & tmp) != 0)
			return i;
		tmp >>= 1;
	}
	return 32;
}
bool IdSpreadComp(IdSpread& a, IdSpread& b) {
	return a.spread > b.spread;
}