#pragma once
#include "ProductionView.h"

class ConsoleProductionView : public ProductionView {
public:
    void showProductionStatus(
        const std::optional<ProductionJob>& currentJob,
        const std::vector<ProductionJob>&   waitingJobs) override;
    int  getMenuChoice() override;
    void showSuccess(const std::string& msg) override;
    void showError(const std::string& msg) override;
};
