#pragma once
#include "ISampleRepository.h"
#include "../json/JsonValue.h"
#include <string>

class JsonSampleRepository : public ISampleRepository {
public:
    explicit JsonSampleRepository(const std::string& filePath = "data/samples.json");

    void                  save(const Sample& sample) override;
    std::optional<Sample> findById(const std::string& id) override;
    std::vector<Sample>   findAll() override;
    std::vector<Sample>   findByName(const std::string& keyword) override;
    bool                  existsById(const std::string& id) override;
    void                  updateStock(const std::string& id, int delta) override;
    int                   count() override;

private:
    std::string filePath_;

    std::vector<Sample> load() const;
    void flush(const std::vector<Sample>& data) const;
    static JsonValue toJson(const Sample& s);
    static Sample    fromJson(const JsonValue& j);
};
