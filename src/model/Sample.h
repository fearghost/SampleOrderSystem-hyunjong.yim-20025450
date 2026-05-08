#pragma once
#include <string>

struct Sample {
    std::string id;
    std::string name;
    double      avgProductionTime; // min/ea
    double      yieldRate;         // 0 < yieldRate <= 1.0
    int         stock;

    bool operator==(const Sample& o) const {
        return id == o.id && name == o.name && stock == o.stock
            && yieldRate == o.yieldRate && avgProductionTime == o.avgProductionTime;
    }
};
