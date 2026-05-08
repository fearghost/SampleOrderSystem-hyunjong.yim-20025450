#pragma once
#include "../model/ProductionJob.h"
#include <vector>
#include <optional>

class IProductionRepository {
public:
    virtual ~IProductionRepository() = default;
    virtual void                         enqueue(const ProductionJob& job) = 0;
    virtual std::optional<ProductionJob> front() = 0;
    virtual void                         dequeue() = 0;
    virtual void                         remove(const std::string& orderId) = 0;
    virtual std::vector<ProductionJob>   waitingJobs() = 0;
    virtual bool                         isEmpty() = 0;
    virtual int                          size() = 0;
};
