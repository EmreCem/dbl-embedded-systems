#pragma once
#include <vector>
#include <numeric>


class RGBSensorReader {
	std::vector<int> BeltR;
	std::vector<int> BeltG;
	std::vector<int> BeltB;
	
	std::vector<int> FactoryR;
	std::vector<int> FactoryG;
	std::vector<int> FactoryB;

	int GetAverage(std::vector<int> nums);
public:
	RGBSensorReader();
	Color ReadBelt();
	Color ReadFactory();
};

inline int RGBSensorReader::GetAverage(std::vector<int> nums) {
	return int(std::accumulate(nums.begin(), nums.end(), 0) / nums.size());
}

inline RGBSensorReader::RGBSensorReader() {
	
}

inline Color RGBSensorReader::ReadBelt() {
}

inline Color RGBSensorReader::ReadFactory() {
}
