#include "SampleService.h"
#include <stdexcept>

SampleService::SampleService(ISampleRepository& repo) : repo_(repo) {}

void SampleService::registerSample(const std::string& id, const std::string& name,
                                   double avgProductionTime, double yieldRate, int initialStock) {
    if (repo_.existsById(id))
        throw std::invalid_argument("이미 등록된 시료 ID입니다: " + id);

    Sample s;
    s.id                = id;
    s.name              = name;
    s.avgProductionTime = avgProductionTime;
    s.yieldRate         = yieldRate;
    s.stock             = initialStock;
    repo_.save(s);
}

std::vector<Sample> SampleService::listAll() const {
    return repo_.findAll();
}

std::vector<Sample> SampleService::searchByName(const std::string& keyword) const {
    return repo_.findByName(keyword);
}

std::optional<Sample> SampleService::findById(const std::string& id) const {
    return repo_.findById(id);
}

int SampleService::totalSampleCount() const {
    return repo_.count();
}

void SampleService::deductStock(const std::string& sampleId, int quantity) {
    repo_.updateStock(sampleId, -quantity);
}

void SampleService::addStock(const std::string& sampleId, int quantity) {
    repo_.updateStock(sampleId, quantity);
}
