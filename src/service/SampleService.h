#pragma once
#include "../repository/ISampleRepository.h"
#include <string>
#include <vector>
#include <optional>

class SampleService {
public:
    explicit SampleService(ISampleRepository& repo);

    // 시료 등록 — 중복 ID면 std::invalid_argument
    void registerSample(const std::string& id, const std::string& name,
                        double avgProductionTime, double yieldRate, int initialStock);

    [[nodiscard]] std::vector<Sample>     listAll() const;
    [[nodiscard]] std::vector<Sample>     searchByName(const std::string& keyword) const;
    [[nodiscard]] std::optional<Sample>   findById(const std::string& id) const;
    [[nodiscard]] int                     totalSampleCount() const;

    void deductStock(const std::string& sampleId, int quantity);
    void addStock(const std::string& sampleId, int quantity);

private:
    ISampleRepository& repo_;
};
