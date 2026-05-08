#pragma once
#include "IProductionRepository.h"
#include <deque>

class InMemoryProductionRepository : public IProductionRepository {
public:
    void                         enqueue(const ProductionJob& job) override;
    std::optional<ProductionJob> front() override;
    void                         dequeue() override;
    void                         remove(const std::string& orderId) override;
    std::vector<ProductionJob>   waitingJobs() override;
    bool                         isEmpty() override;
    int                          size() override;

private:
    std::deque<ProductionJob> queue_;
};
