#include "InMemorySampleRepository.h"
#include <algorithm>

void InMemorySampleRepository::save(const Sample& sample) {
    auto it = std::find_if(data_.begin(), data_.end(),
        [&](const Sample& s) { return s.id == sample.id; });
    if (it != data_.end())
        *it = sample;
    else
        data_.push_back(sample);
}

std::optional<Sample> InMemorySampleRepository::findById(const std::string& id) {
    auto it = std::find_if(data_.begin(), data_.end(),
        [&](const Sample& s) { return s.id == id; });
    if (it != data_.end())
        return *it;
    return std::nullopt;
}

std::vector<Sample> InMemorySampleRepository::findAll() {
    return data_;
}

std::vector<Sample> InMemorySampleRepository::findByName(const std::string& keyword) {
    std::vector<Sample> result;
    for (const auto& s : data_)
        if (s.name.find(keyword) != std::string::npos)
            result.push_back(s);
    return result;
}

bool InMemorySampleRepository::existsById(const std::string& id) {
    return std::any_of(data_.begin(), data_.end(),
        [&](const Sample& s) { return s.id == id; });
}

void InMemorySampleRepository::updateStock(const std::string& id, int delta) {
    auto it = std::find_if(data_.begin(), data_.end(),
        [&](const Sample& s) { return s.id == id; });
    if (it != data_.end())
        it->stock += delta;
}

int InMemorySampleRepository::count() {
    return static_cast<int>(data_.size());
}
