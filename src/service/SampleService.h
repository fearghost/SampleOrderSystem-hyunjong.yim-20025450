#pragma once
#include "../repository/ISampleRepository.h"
#include <string>
#include <vector>
#include <optional>

class SampleService {
public:
    explicit SampleService(ISampleRepository& repo);
    virtual ~SampleService() = default;

    virtual void registerSample(const std::string& id, const std::string& name,
                                double avgProductionTime, double yieldRate, int initialStock);

    [[nodiscard]] virtual std::vector<Sample>   listAll() const;
    [[nodiscard]] virtual std::vector<Sample>   searchByName(const std::string& keyword) const;
    [[nodiscard]] virtual std::optional<Sample> findById(const std::string& id) const;
    [[nodiscard]] virtual int                   totalSampleCount() const;

    virtual void deductStock(const std::string& sampleId, int quantity);
    virtual void addStock(const std::string& sampleId, int quantity);

private:
    ISampleRepository& repo_;
};
