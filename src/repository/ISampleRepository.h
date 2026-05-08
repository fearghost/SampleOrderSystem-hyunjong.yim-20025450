#pragma once
#include "../model/Sample.h"
#include <vector>
#include <string>
#include <optional>

class ISampleRepository {
public:
    virtual ~ISampleRepository() = default;
    virtual void                    save(const Sample& sample) = 0;
    virtual std::optional<Sample>   findById(const std::string& id) = 0;
    virtual std::vector<Sample>     findAll() = 0;
    virtual std::vector<Sample>     findByName(const std::string& keyword) = 0;
    virtual bool                    existsById(const std::string& id) = 0;
    virtual void                    updateStock(const std::string& id, int delta) = 0;
    virtual int                     count() = 0;
};
