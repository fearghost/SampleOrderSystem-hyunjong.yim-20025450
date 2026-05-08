#pragma once
#include <string>

struct ProductionJob {
    std::string orderId;
    std::string sampleId;
    int         shortage;   // 부족분 = quantity - stock_at_approval
    int         actualQty;  // ceil(shortage / (yieldRate * 0.9))
    double      totalTime;  // avgProductionTime * actualQty
};
