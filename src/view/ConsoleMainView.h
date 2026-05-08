#pragma once
#include "MainView.h"

class ConsoleMainView : public MainView {
public:
    void showMenu(int sampleCount, int orderCount) override;
    int  getMenuChoice() override;
    void showError(const std::string& msg) override;
};
