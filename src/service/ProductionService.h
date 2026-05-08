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

    [[nodiscard]] std::optional<ProductionJob> getCurrentJob() const;
    [[nodiscard]] std::vector<ProductionJob>   getWaitingJobs() const;
    [[nodiscard]] bool isIdle() const;
    [[nodiscard]] int  queueSize() const;

    // 완료: dequeue → updateStock(+actualQty) → updateStatus(CONFIRMED)
    void completeCurrentJob();

private:
    IProductionRepository& production_;
    IOrderRepository&      orders_;
    ISampleRepository&     samples_;
};
