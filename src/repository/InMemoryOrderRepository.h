#pragma once
#include "IOrderRepository.h"
#include <vector>

class InMemoryOrderRepository : public IOrderRepository {
public:
    void                  save(const Order& order) override;
    std::optional<Order>  findById(const std::string& orderId) override;
    std::vector<Order>    findAll() override;
    std::vector<Order>    findByStatus(OrderStatus status) override;
    void                  updateStatus(const std::string& orderId, OrderStatus status) override;
    int                   count() override;

private:
    std::vector<Order> data_;
};
