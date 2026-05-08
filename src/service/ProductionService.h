#pragma once
#include "../repository/IProductionRepository.h"
#include "../repository/IOrderRepository.h"
#include "../repository/ISampleRepository.h"
#include <optional>
#include <vector>
#include <string>

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

    // FIFO 완료: 큐 맨 앞 작업을 완료 처리하고 주문을 CONFIRMED로 전환
    virtual void completeCurrentJob();

    // 취소: 지정 주문의 생산 작업을 큐에서 제거하고 주문을 REJECTED로 전환
    virtual void cancelJob(const std::string& orderId);

private:
    IProductionRepository& production_;
    IOrderRepository&      orders_;
    ISampleRepository&     samples_;
};
