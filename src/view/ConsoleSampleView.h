#pragma once
#include "SampleView.h"

class ConsoleSampleView : public SampleView {
public:
    void        showSubMenu() override;
    int         getMenuChoice() override;
    SampleInput getRegisterInput() override;
    std::string getSearchKeyword() override;
    void        showSampleList(const std::vector<Sample>&, const std::string& header) override;
    void        showSuccess(const std::string& msg) override;
    void        showError(const std::string& msg) override;
};
