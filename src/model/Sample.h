#pragma once
#include <string>

struct Sample {
    std::string id;
    std::string name;
    double      avgProductionTime; // min/ea
    double      yieldRate;         // 0 < yieldRate <= 1.0
    int         stock;
};
