#include "JsonSampleRepository.h"
#include "RepositoryUtils.h"
#include "../json/JsonParser.h"
#include <algorithm>
#include <cctype>

JsonSampleRepository::JsonSampleRepository(const std::string& filePath)
    : filePath_(filePath) {
    RepositoryUtils::ensureDirectoryExists(filePath_);
}

void JsonSampleRepository::save(const Sample& sample) {
    auto data = load();
    auto it = std::find_if(data.begin(), data.end(),
        [&](const Sample& s) { return s.id == sample.id; });
    if (it != data.end()) *it = sample;
    else data.push_back(sample);
    flush(data);
}

std::optional<Sample> JsonSampleRepository::findById(const std::string& id) {
    for (const auto& s : load())
        if (s.id == id) return s;
    return std::nullopt;
}

std::vector<Sample> JsonSampleRepository::findAll() { return load(); }

std::vector<Sample> JsonSampleRepository::findByName(const std::string& keyword) {
    auto toLower = [](std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return s;
    };
    std::string kw = toLower(keyword);
    std::vector<Sample> result;
    for (const auto& s : load())
        if (toLower(s.name).find(kw) != std::string::npos)
            result.push_back(s);
    return result;
}

bool JsonSampleRepository::existsById(const std::string& id) {
    for (const auto& s : load())
        if (s.id == id) return true;
    return false;
}

void JsonSampleRepository::updateStock(const std::string& id, int delta) {
    auto data = load();
    for (auto& s : data)
        if (s.id == id) { s.stock += delta; flush(data); return; }
}

int JsonSampleRepository::count() { return static_cast<int>(load().size()); }

std::vector<Sample> JsonSampleRepository::load() const {
    JsonValue root;
    try { root = JsonParser::parseFile(filePath_); }
    catch (...) { return {}; }
    if (root.isNull() || !root.contains("samples")) return {};
    std::vector<Sample> result;
    const auto& arr = root["samples"];
    for (size_t i = 0; i < arr.size(); ++i)
        result.push_back(fromJson(arr[i]));
    return result;
}

void JsonSampleRepository::flush(const std::vector<Sample>& data) const {
    JsonValue root = JsonValue::makeObject();
    JsonValue arr  = JsonValue::makeArray();
    for (const auto& s : data) arr.push(toJson(s));
    root["samples"] = arr;
    JsonParser::writeFile(filePath_, root);
}

JsonValue JsonSampleRepository::toJson(const Sample& s) {
    JsonValue obj = JsonValue::makeObject();
    obj["id"]                = JsonValue::fromString(s.id);
    obj["name"]              = JsonValue::fromString(s.name);
    obj["avgProductionTime"] = JsonValue::fromDouble(s.avgProductionTime);
    obj["yieldRate"]         = JsonValue::fromDouble(s.yieldRate);
    obj["stock"]             = JsonValue::fromDouble(s.stock);
    return obj;
}

Sample JsonSampleRepository::fromJson(const JsonValue& j) {
    return { j["id"].asString(), j["name"].asString(),
             j["avgProductionTime"].asDouble(), j["yieldRate"].asDouble(),
             j["stock"].asInt() };
}
