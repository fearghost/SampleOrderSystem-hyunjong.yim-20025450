#include "ProductionService.h"
#include "../model/Order.h"
#include <stdexcept>

ProductionService::ProductionService(IProductionRepository& production,
                                     IOrderRepository&      orders,
                                     ISampleRepository&     samples)
    : production_(production), orders_(orders), samples_(samples) {}

std::optional<ProductionJob> ProductionService::getCurrentJob() const {
    if (production_.isEmpty())
        return std::nullopt;
    return production_.front();
}

std::vector<ProductionJob> ProductionService::getWaitingJobs() const {
    return production_.waitingJobs();
}

bool ProductionService::isIdle() const {
    return production_.isEmpty();
}

int ProductionService::queueSize() const {
    return production_.size();
}

void ProductionService::completeCurrentJob() {
    if (production_.isEmpty())
        throw std::logic_error("생산 중인 작업이 없습니다.");

    const ProductionJob job = *production_.front();
    production_.dequeue();
    samples_.updateStock(job.sampleId, job.actualQty);
    orders_.updateStatus(job.orderId, OrderStatus::CONFIRMED);
}

void ProductionService::cancelJob(const std::string& orderId) {
    production_.remove(orderId);
    orders_.updateStatus(orderId, OrderStatus::REJECTED);
}
