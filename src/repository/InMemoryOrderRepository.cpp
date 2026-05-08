#include "InMemoryOrderRepository.h"
#include <algorithm>

void InMemoryOrderRepository::save(const Order& order) {
    auto it = std::find_if(data_.begin(), data_.end(),
        [&](const Order& o) { return o.orderId == order.orderId; });
    if (it != data_.end())
        *it = order;
    else
        data_.push_back(order);
}

std::optional<Order> InMemoryOrderRepository::findById(const std::string& orderId) {
    auto it = std::find_if(data_.begin(), data_.end(),
        [&](const Order& o) { return o.orderId == orderId; });
    if (it != data_.end())
        return *it;
    return std::nullopt;
}

std::vector<Order> InMemoryOrderRepository::findAll() {
    return data_;
}

std::vector<Order> InMemoryOrderRepository::findByStatus(OrderStatus status) {
    std::vector<Order> result;
    for (const auto& o : data_)
        if (o.status == status)
            result.push_back(o);
    return result;
}

void InMemoryOrderRepository::updateStatus(const std::string& orderId, OrderStatus status) {
    auto it = std::find_if(data_.begin(), data_.end(),
        [&](const Order& o) { return o.orderId == orderId; });
    if (it != data_.end())
        it->status = status;
}

int InMemoryOrderRepository::count() {
    return static_cast<int>(data_.size());
}
