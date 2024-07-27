#ifdef WIN32

#include<direct.h>
void mkdir(const char* str, size_t mode)
{
	_mkdir(str);
}
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif // WIN32

#include<iostream>
#include<cmath>
#include<cstring>
#include <unordered_set>
#include<queue>
#include<thread>
#include "../../include/common/benchmark.h"
#include "../../include/baseline/abstrach.h"
#include "../../include/baseline/Calypso/Calypso.h"
#include "../../include/baseline/Couper/Couper.h"
#include "../../include/baseline/VHS/VHS.h"
#include "../../include/baseline/RerSkt/rerskt.h"
constexpr double MB = 1e6;
constexpr double KB = 1e3;
void SuperSpreaderDetection(int dataset_type, const char* dataset_path, uint32_t memory_start, uint32_t memory_step, uint32_t memory_stop,double threshold_ratio)
{
	printf("\033[0m\033[1;4;33m# Testing function: BenchSuperSpreaderDetection\n\033[0m");
	std::vector<std::pair<uint32_t, uint32_t>> dataset = read_data(dataset_path, dataset_type);
	std::map<uint32_t, std::unordered_set<uint32_t>> mp;
	for (auto& p : dataset)
	{
		mp[p.first].insert(p.second);
	}
	int sum = 0;
	for (auto& p : mp)
	{
		sum += p.second.size();
	}
	int threshold = threshold_ratio * sum;
	printf("Set Super Spreader threshold=%d\n", threshold);
	constexpr uint32_t cmp = 4;
	SketchBase* sketches[cmp];
	uint32_t memory = memory_start;
	sketches[0] = new Couper(memory / 1024);
	sketches[1] = new Calypso(memory, 0.4, 4, 12, 1, 3, true, false, true);
	sketches[2] = new RerSkt<HLL>(memory / 1024, 0.2);
	sketches[3] = new VHS(memory / 1024, 0.2);
	for (size_t memory = memory_start; memory <= memory_stop; memory += memory_step)
	{
		printf("\033[0m\033[1;4;36m> Memory size: %dKB\n\033[0m", int(memory / KB));
		sketches[0] = new Couper(memory / 1024);
		sketches[1] = new Calypso(memory, 0.4, 4, 12, 1, 3, true, false, true);
		sketches[2] = new RerSkt<HLL>(memory / 1024, 0.2);
		sketches[3] = new VHS(memory / 1024, 0.2);
		for (auto& sketch : sketches)
		{
			for (auto& p : dataset)
			{
				sketch->insert(p.first, p.second);
			}
			double topk_AAE = 0, topk_ARE = 0;
			int topk_num = 0, hit = 0, predict_num = 0;
			for (auto& f : mp)
			{
				uint32_t x = f.first, truth = f.second.size();
				auto predict = sketch->get_flow_cardinality(x);
				if (truth >= threshold)
				{
					topk_AAE += abs(1.0 * predict - truth);
					topk_ARE += abs(1.0 * predict - truth) / truth;
					topk_num++;
					if (predict >= threshold)
					{
						hit++;
					}
				}
				if (predict >= threshold)
				{
					predict_num++;
				}
			}
			topk_AAE /= topk_num;
			topk_ARE /= topk_num;
			double pr = hit * 1.0 / predict_num, cr = hit * 1.0 / topk_num, f1 = 2 * pr * cr / (pr + cr);
			printf("%s :\tAAE: %.4f\tARE: %.4f\tRR: %.4f\tPR: %.4f\tF1: %.4f\n",
				sketch->name.c_str(), topk_AAE, topk_ARE, cr, pr, f1
				);
			delete sketch;
		}
	}

}
int main(int argc, char* argv[])
{
	int file_type;
	std::string file_path;
	file_type = std::stoi(argv[1]);
	file_path = argv[2];

	printf(
		"\033[0m\033[1;32m|                                Calypso "
		"Experiments                                |\n\033[0m");
	printf(
		"\033[0m\033[1;32m======================================================="
		"==============================\n\033[0m");
	printf(
		"\033[0m\033[1;32m|                           Test on "
		"SuperSpreaderDetection                          |\n\033[0m");
	printf(
		"\033[0m\033[1;32m======================================================="
		"==============================\n\033[0m");
	SuperSpreaderDetection(file_type, file_path.c_str(), 100000, 100000, 1000000,2e-5);
	printf(
		"\033[0m\033[1;32m======================================================="
		"==============================\n\033[0m");
}