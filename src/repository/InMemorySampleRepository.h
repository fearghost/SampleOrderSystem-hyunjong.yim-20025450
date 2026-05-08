#pragma once
#include "ISampleRepository.h"
#include <vector>

class InMemorySampleRepository : public ISampleRepository {
public:
    void                    save(const Sample& sample) override;
    std::optional<Sample>   findById(const std::string& id) override;
    std::vector<Sample>     findAll() override;
    std::vector<Sample>     findByName(const std::string& keyword) override;
    bool                    existsById(const std::string& id) override;
    void                    updateStock(const std::string& id, int delta) override;
    int                     count() override;

private:
    std::vector<Sample> data_;
};
