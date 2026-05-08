#pragma once
#include "IProductionRepository.h"
#include "../json/JsonValue.h"
#include <string>

class JsonProductionRepository : public IProductionRepository {
public:
    explicit JsonProductionRepository(const std::string& filePath = "data/production.json");

    void                         enqueue(const ProductionJob& job) override;
    std::optional<ProductionJob> front() override;
    void                         dequeue() override;
    std::vector<ProductionJob>   waitingJobs() override;
    bool                         isEmpty() override;
    int                          size() override;

private:
    std::string filePath_;

    std::vector<ProductionJob> load() const;
    void flush(const std::vector<ProductionJob>& data) const;
    static JsonValue      toJson(const ProductionJob& j);
    static ProductionJob  fromJson(const JsonValue& j);
};
