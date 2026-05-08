#pragma once
#include "../repository/IProductionRepository.h"
#include "../repository/IOrderRepository.h"
#include "../repository/ISampleRepository.h"
#include <optional>
#include <vector>

class ProductionService {
public:
    ProductionService(IProductionRepository& production,
                      IOrderRepository&      orders,
                      ISampleRepository&     samples);
    virtual ~ProductionService() = default;

    [[nodiscard]] virtual std::optional<ProductionJob> getCurrentJob() const;
    [[nodiscard]] virtual std::vector<ProductionJob>   getWaitingJobs() const;
    [[nodiscard]] virtual bool isIdle() const;
    [[nodiscard]] virtual int  queueSize() const;

    virtual void completeCurrentJob();

private:
    IProductionRepository& production_;
    IOrderRepository&      orders_;
    ISampleRepository&     samples_;
};
