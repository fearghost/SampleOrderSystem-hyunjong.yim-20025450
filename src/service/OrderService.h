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

    // 주문 접수 → RESERVED
    void placeOrder(const std::string& sampleId,
                    const std::string& customerName,
                    int                quantity,
                    const std::string& orderId,
                    const std::string& createdAt);

    // 승인: 재고 충분 → CONFIRMED + 재고 차감
    //       재고 부족 → PRODUCING + 생산라인 등록
    void approveOrder(const std::string& orderId);

    // 거절 → REJECTED
    void rejectOrder(const std::string& orderId);

    // 출고: CONFIRMED → RELEASE
    void processShipment(const std::string& orderId);

    // 생산 완료: PRODUCING → CONFIRMED
    void completeProduction(const std::string& orderId);

    [[nodiscard]] std::vector<Order> listByStatus(OrderStatus status) const;
    [[nodiscard]] std::vector<Order> listAll() const;
    [[nodiscard]] int                totalOrderCount() const;

    // public: 단위 테스트에서 직접 검증 (순수 계산 함수)
    static int calcActualQty(int shortage, double yieldRate);

private:
    ISampleRepository&     samples_;
    IOrderRepository&      orders_;
    IProductionRepository& production_;

    static std::string generateTimestamp();
};
