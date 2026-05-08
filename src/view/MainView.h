#pragma once
#include <string>

class MainView {
public:
    virtual ~MainView() = default;
    virtual void showMenu(int sampleCount, int orderCount) = 0;
    virtual int  getMenuChoice() = 0;
    virtual void showError(const std::string& msg) = 0;
};
