#pragma once
#include <string>

enum class OrderStatus {
    RESERVED,
    PRODUCING,
    CONFIRMED,
    RELEASE,
    REJECTED
};

std::string statusToString(OrderStatus s);

struct Order {
    std::string orderId;       // ORD-YYYYMMDD-NNNN
    std::string sampleId;
    std::string customerName;
    int         quantity;
    OrderStatus status;
    std::string createdAt;     // ISO 8601
};
