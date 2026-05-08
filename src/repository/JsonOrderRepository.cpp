#include "JsonOrderRepository.h"
#include "RepositoryUtils.h"
#include "../json/JsonParser.h"
#include <algorithm>

JsonOrderRepository::JsonOrderRepository(const std::string& filePath)
    : filePath_(filePath) {
    RepositoryUtils::ensureDirectoryExists(filePath_);
}

void JsonOrderRepository::save(const Order& order) {
    auto data = load();
    auto it = std::find_if(data.begin(), data.end(),
        [&](const Order& o) { return o.orderId == order.orderId; });
    if (it != data.end()) *it = order;
    else data.push_back(order);
    flush(data);
}

std::optional<Order> JsonOrderRepository::findById(const std::string& orderId) {
    for (const auto& o : load())
        if (o.orderId == orderId) return o;
    return std::nullopt;
}

std::vector<Order> JsonOrderRepository::findAll() { return load(); }

std::vector<Order> JsonOrderRepository::findByStatus(OrderStatus status) {
    std::vector<Order> result;
    for (const auto& o : load())
        if (o.status == status) result.push_back(o);
    return result;
}

void JsonOrderRepository::updateStatus(const std::string& orderId, OrderStatus status) {
    auto data = load();
    for (auto& o : data)
        if (o.orderId == orderId) { o.status = status; flush(data); return; }
}

int JsonOrderRepository::count() { return static_cast<int>(load().size()); }

std::vector<Order> JsonOrderRepository::load() const {
    JsonValue root;
    try { root = JsonParser::parseFile(filePath_); }
    catch (...) { return {}; }
    if (root.isNull() || !root.contains("orders")) return {};
    std::vector<Order> result;
    const auto& arr = root["orders"];
    for (size_t i = 0; i < arr.size(); ++i)
        result.push_back(fromJson(arr[i]));
    return result;
}

void JsonOrderRepository::flush(const std::vector<Order>& data) const {
    JsonValue root = JsonValue::makeObject();
    JsonValue arr  = JsonValue::makeArray();
    for (const auto& o : data) arr.push(toJson(o));
    root["orders"] = arr;
    JsonParser::writeFile(filePath_, root);
}

JsonValue JsonOrderRepository::toJson(const Order& o) {
    JsonValue obj = JsonValue::makeObject();
    obj["orderId"]      = JsonValue::fromString(o.orderId);
    obj["sampleId"]     = JsonValue::fromString(o.sampleId);
    obj["customerName"] = JsonValue::fromString(o.customerName);
    obj["quantity"]     = JsonValue::fromDouble(o.quantity);
    obj["status"]       = JsonValue::fromString(statusToString(o.status));
    obj["createdAt"]    = JsonValue::fromString(o.createdAt);
    return obj;
}

Order JsonOrderRepository::fromJson(const JsonValue& j) {
    return { j["orderId"].asString(), j["sampleId"].asString(),
             j["customerName"].asString(), j["quantity"].asInt(),
             statusFromString(j["status"].asString()), j["createdAt"].asString() };
}
