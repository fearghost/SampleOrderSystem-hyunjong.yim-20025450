#pragma once
#include <string>

enum class OrderStatus {
    RESERVED,
    PRODUCING,
    CONFIRMED,
    RELEASE,
    REJECTED
};

[[nodiscard]] std::string  statusToString(OrderStatus s);
[[nodiscard]] OrderStatus  statusFromString(const std::string& s);

struct Order {
    std::string orderId;       // ORD-YYYYMMDD-NNNN
    std::string sampleId;
    std::string customerName;
    int         quantity;
    OrderStatus status;
    std::string createdAt;     // ISO 8601

    bool operator==(const Order& o) const {
        return orderId == o.orderId && sampleId == o.sampleId
            && customerName == o.customerName && quantity == o.quantity
            && status == o.status && createdAt == o.createdAt;
    }
};
