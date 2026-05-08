#pragma once
#include "../model/ProductionJob.h"
#include <optional>
#include <vector>
#include <string>

class ProductionView {
public:
    virtual ~ProductionView() = default;
    virtual void showProductionStatus(
        const std::optional<ProductionJob>& currentJob,
        const std::vector<ProductionJob>&   waitingJobs) = 0;
    virtual void showCancelPrompt(int jobCount) = 0;
    virtual int  getMenuChoice() = 0;
    virtual void showSuccess(const std::string& msg) = 0;
    virtual void showError(const std::string& msg) = 0;
};
