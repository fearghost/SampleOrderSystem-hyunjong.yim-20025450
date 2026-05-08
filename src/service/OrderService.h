#pragma once
#include "../repository/ISampleRepository.h"
#include "../repository/IOrderRepository.h"
#include "../repository/IProductionRepository.h"
#include <string>
#include <vector>
#include <cmath>

class OrderService {
public:
    OrderService(ISampleRepository&     samples,
                 IOrderRepository&      orders,
                 IProductionRepository& production);
    virtual ~OrderService() = default;

    // 주문 접수 → orderId·createdAt은 서비스가 내부 생성
    virtual void placeOrder(const std::string& sampleId,
                            const std::string& customerName,
                            int                quantity);

    virtual void approveOrder(const std::string& orderId);
    virtual void rejectOrder(const std::string& orderId);
    virtual void processShipment(const std::string& orderId);
    virtual void completeProduction(const std::string& orderId);

    [[nodiscard]] virtual std::vector<Order> listByStatus(OrderStatus status) const;
    [[nodiscard]] virtual std::vector<Order> listAll() const;
    [[nodiscard]] virtual int                totalOrderCount() const;

    static int calcActualQty(int shortage, double yieldRate);

private:
    ISampleRepository&     samples_;
    IOrderRepository&      orders_;
    IProductionRepository& production_;

    static std::string generateOrderId();
    static std::string generateTimestamp();
};
