#pragma once
#include "../model/Sample.h"
#include <vector>
#include <string>

struct SampleInput {
    std::string id;
    std::string name;
    double      avgProductionTime;
    double      yieldRate;
    int         initialStock;
};

class SampleView {
public:
    virtual ~SampleView() = default;
    virtual void        showSubMenu() = 0;
    virtual int         getMenuChoice() = 0;
    virtual SampleInput getRegisterInput() = 0;
    virtual std::string getSearchKeyword() = 0;
    virtual void        showSampleList(const std::vector<Sample>&, const std::string& header) = 0;
    virtual void        showSuccess(const std::string& msg) = 0;
    virtual void        showError(const std::string& msg) = 0;
};
