#pragma once
#include "IOrderRepository.h"
#include "../json/JsonValue.h"
#include <string>

class JsonOrderRepository : public IOrderRepository {
public:
    explicit JsonOrderRepository(const std::string& filePath = "data/orders.json");

    void                 save(const Order& order) override;
    std::optional<Order> findById(const std::string& orderId) override;
    std::vector<Order>   findAll() override;
    std::vector<Order>   findByStatus(OrderStatus status) override;
    void                 updateStatus(const std::string& orderId, OrderStatus status) override;
    int                  count() override;

private:
    std::string filePath_;

    std::vector<Order> load() const;
    void flush(const std::vector<Order>& data) const;
    static JsonValue toJson(const Order& o);
    static Order     fromJson(const JsonValue& j);
};
