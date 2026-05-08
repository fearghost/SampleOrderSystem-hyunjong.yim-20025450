#pragma once
#include "../model/Order.h"
#include <vector>
#include <string>
#include <optional>

class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;
    virtual void                  save(const Order& order) = 0;
    virtual std::optional<Order>  findById(const std::string& orderId) = 0;
    virtual std::vector<Order>    findAll() = 0;
    virtual std::vector<Order>    findByStatus(OrderStatus status) = 0;
    virtual void                  updateStatus(const std::string& orderId, OrderStatus status) = 0;
    virtual int                   count() = 0;
};
