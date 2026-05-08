#include "JsonProductionRepository.h"
#include "RepositoryUtils.h"
#include "../json/JsonParser.h"

JsonProductionRepository::JsonProductionRepository(const std::string& filePath)
    : filePath_(filePath) {
    RepositoryUtils::ensureDirectoryExists(filePath_);
}

void JsonProductionRepository::enqueue(const ProductionJob& job) {
    auto data = load();
    data.push_back(job);
    flush(data);
}

std::optional<ProductionJob> JsonProductionRepository::front() {
    auto data = load();
    if (data.empty()) return std::nullopt;
    return data.front();
}

void JsonProductionRepository::dequeue() {
    auto data = load();
    if (!data.empty()) {
        data.erase(data.begin());
        flush(data);
    }
}

std::vector<ProductionJob> JsonProductionRepository::waitingJobs() {
    auto data = load();
    if (data.size() <= 1) return {};
    return std::vector<ProductionJob>(data.begin() + 1, data.end());
}

bool JsonProductionRepository::isEmpty() { return load().empty(); }

int  JsonProductionRepository::size()    { return static_cast<int>(load().size()); }

std::vector<ProductionJob> JsonProductionRepository::load() const {
    JsonValue root;
    try { root = JsonParser::parseFile(filePath_); }
    catch (...) { return {}; }
    if (root.isNull() || !root.isArray()) return {};
    std::vector<ProductionJob> result;
    for (size_t i = 0; i < root.size(); ++i)
        result.push_back(fromJson(root[i]));
    return result;
}

void JsonProductionRepository::flush(const std::vector<ProductionJob>& data) const {
    JsonValue arr = JsonValue::makeArray();
    for (const auto& j : data) arr.push(toJson(j));
    JsonParser::writeFile(filePath_, arr);
}

JsonValue JsonProductionRepository::toJson(const ProductionJob& j) {
    JsonValue obj = JsonValue::makeObject();
    obj["orderId"]   = JsonValue::fromString(j.orderId);
    obj["sampleId"]  = JsonValue::fromString(j.sampleId);
    obj["shortage"]  = JsonValue::fromDouble(j.shortage);
    obj["actualQty"] = JsonValue::fromDouble(j.actualQty);
    obj["totalTime"] = JsonValue::fromDouble(j.totalTime);
    return obj;
}

ProductionJob JsonProductionRepository::fromJson(const JsonValue& j) {
    return { j["orderId"].asString(), j["sampleId"].asString(),
             j["shortage"].asInt(), j["actualQty"].asInt(), j["totalTime"].asDouble() };
}
