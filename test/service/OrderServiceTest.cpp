#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../test/MockRepositories.h"
#include "../../src/service/OrderService.h"

using ::testing::Return;
using ::testing::_;
using ::testing::SaveArg;
using ::testing::Field;
using ::testing::AllOf;

class OrderServiceTest : public ::testing::Test {
protected:
    MockSampleRepository     mockSamples;
    MockOrderRepository      mockOrders;
    MockProductionRepository mockProduction;
    OrderService service{mockSamples, mockOrders, mockProduction};

    Sample makeSample(int stock, double yieldRate = 0.92) {
        return {"S-001", "SiC 파워기판-6인치", 0.8, yieldRate, stock};
    }
    Order makeOrder(OrderStatus s = OrderStatus::RESERVED, int qty = 200) {
        return {"ORD-001", "S-001", "고객A", qty, s, "2026-05-08"};
    }
};

// 1. placeOrder — RESERVED 저장 (orderId·createdAt은 서비스가 내부 생성)
TEST_F(OrderServiceTest, placeOrder_savesReservedOrder) {
    Order captured;
    EXPECT_CALL(mockSamples, existsById("S-001")).WillOnce(Return(true));
    EXPECT_CALL(mockOrders, save(_)).WillOnce(SaveArg<0>(&captured));

    service.placeOrder("S-001", "고객A", 200);

    EXPECT_EQ(captured.status,       OrderStatus::RESERVED);
    EXPECT_EQ(captured.sampleId,     "S-001");
    EXPECT_EQ(captured.customerName, "고객A");
    EXPECT_EQ(captured.quantity,     200);
    EXPECT_FALSE(captured.orderId.empty());   // 서비스가 생성
    EXPECT_FALSE(captured.createdAt.empty()); // 서비스가 생성
}

// 2. placeOrder — 존재하지 않는 시료 ID
TEST_F(OrderServiceTest, placeOrder_unknownSampleId_throws) {
    EXPECT_CALL(mockSamples, existsById("S-999")).WillOnce(Return(false));

    EXPECT_THROW(
        service.placeOrder("S-999", "고객A", 100),
        std::invalid_argument
    );
}

// 3. approveOrder — 재고 충분 → CONFIRMED + 재고 차감
TEST_F(OrderServiceTest, approveOrder_stockSufficient_confirmed) {
    EXPECT_CALL(mockOrders, findById("ORD-001")).WillOnce(Return(makeOrder(OrderStatus::RESERVED, 200)));
    EXPECT_CALL(mockSamples, findById("S-001")).WillOnce(Return(makeSample(500)));
    EXPECT_CALL(mockOrders, updateStatus("ORD-001", OrderStatus::CONFIRMED));
    EXPECT_CALL(mockSamples, updateStock("S-001", -200));

    service.approveOrder("ORD-001");
}

// 4. approveOrder — 재고 부족 → PRODUCING + enqueue(shortage=170, actualQty=206)
TEST_F(OrderServiceTest, approveOrder_stockInsufficient_producing) {
    EXPECT_CALL(mockOrders, findById("ORD-001")).WillOnce(Return(makeOrder(OrderStatus::RESERVED, 200)));
    EXPECT_CALL(mockSamples, findById("S-001")).WillOnce(Return(makeSample(30, 0.92)));
    EXPECT_CALL(mockProduction, enqueue(AllOf(
        Field(&ProductionJob::shortage,  170),
        Field(&ProductionJob::actualQty, 206)
    )));
    EXPECT_CALL(mockOrders, updateStatus("ORD-001", OrderStatus::PRODUCING));
    EXPECT_CALL(mockSamples, updateStock("S-001", -30));

    service.approveOrder("ORD-001");
}

// 5. approveOrder — 재고 0 → PRODUCING + enqueue(shortage=100), updateStock 호출 없음
TEST_F(OrderServiceTest, approveOrder_stockZero_producing) {
    EXPECT_CALL(mockOrders, findById("ORD-001")).WillOnce(Return(makeOrder(OrderStatus::RESERVED, 100)));
    EXPECT_CALL(mockSamples, findById("S-001")).WillOnce(Return(makeSample(0, 0.92)));
    EXPECT_CALL(mockProduction, enqueue(Field(&ProductionJob::shortage, 100)));
    EXPECT_CALL(mockOrders, updateStatus("ORD-001", OrderStatus::PRODUCING));
    // stock==0 이므로 updateStock 호출 없음

    service.approveOrder("ORD-001");
}

// 6. approveOrder — RESERVED 아닌 상태 → std::logic_error
TEST_F(OrderServiceTest, approveOrder_notReserved_throws) {
    EXPECT_CALL(mockOrders, findById("ORD-001")).WillOnce(Return(makeOrder(OrderStatus::CONFIRMED)));

    EXPECT_THROW(service.approveOrder("ORD-001"), std::logic_error);
}

// 7. approveOrder — 재고 부족 시 잔여 재고 전부 소진
TEST_F(OrderServiceTest, approveOrder_stockInsufficient_consumesRemainingStock) {
    EXPECT_CALL(mockOrders, findById("ORD-001")).WillOnce(Return(makeOrder(OrderStatus::RESERVED, 200)));
    EXPECT_CALL(mockSamples, findById("S-001")).WillOnce(Return(makeSample(30, 0.92)));
    EXPECT_CALL(mockProduction, enqueue(_));
    EXPECT_CALL(mockOrders, updateStatus("ORD-001", OrderStatus::PRODUCING));
    EXPECT_CALL(mockSamples, updateStock("S-001", -30));  // 잔여 30 소진

    service.approveOrder("ORD-001");
}

// 8. rejectOrder → REJECTED
TEST_F(OrderServiceTest, rejectOrder_setsRejected) {
    EXPECT_CALL(mockOrders, findById("ORD-001")).WillOnce(Return(makeOrder(OrderStatus::RESERVED)));
    EXPECT_CALL(mockOrders, updateStatus("ORD-001", OrderStatus::REJECTED));

    service.rejectOrder("ORD-001");
}

// 9. processShipment — CONFIRMED → RELEASE
TEST_F(OrderServiceTest, processShipment_confirmedToRelease) {
    EXPECT_CALL(mockOrders, findById("ORD-001")).WillOnce(Return(makeOrder(OrderStatus::CONFIRMED)));
    EXPECT_CALL(mockOrders, updateStatus("ORD-001", OrderStatus::RELEASE));

    service.processShipment("ORD-001");
}

// 10. processShipment — CONFIRMED 아닌 상태 → std::logic_error
TEST_F(OrderServiceTest, processShipment_notConfirmed_throws) {
    EXPECT_CALL(mockOrders, findById("ORD-001")).WillOnce(Return(makeOrder(OrderStatus::RESERVED)));

    EXPECT_THROW(service.processShipment("ORD-001"), std::logic_error);
}

// 11. completeProduction — PRODUCING → CONFIRMED
TEST_F(OrderServiceTest, completeProduction_producingToConfirmed) {
    EXPECT_CALL(mockOrders, findById("ORD-001")).WillOnce(Return(makeOrder(OrderStatus::PRODUCING)));
    EXPECT_CALL(mockOrders, updateStatus("ORD-001", OrderStatus::CONFIRMED));

    service.completeProduction("ORD-001");
}

// 12. calcActualQty — 일반 케이스 (shortage=170, yield=0.92 → 206)
TEST_F(OrderServiceTest, calcActualQty_normalCase) {
    // 170 / (0.92 * 0.9) = 170 / 0.828 = 205.31... → ceil = 206
    EXPECT_EQ(OrderService::calcActualQty(170, 0.92), 206);
}

// 13. calcActualQty — 경계값 (shortage=1, yield=1.0 → 2)
TEST_F(OrderServiceTest, calcActualQty_boundaryOne) {
    // 1 / (1.0 * 0.9) = 1.111... → ceil = 2
    EXPECT_EQ(OrderService::calcActualQty(1, 1.0), 2);
}

// 14. calcActualQty — 정확히 나누어 떨어지는 경우 (shortage=90, yield=1.0 → 100)
TEST_F(OrderServiceTest, calcActualQty_exactDivision) {
    // 90 / (1.0 * 0.9) = 100.0 → ceil = 100
    EXPECT_EQ(OrderService::calcActualQty(90, 1.0), 100);
}

// 15. listByStatus — repo 위임
TEST_F(OrderServiceTest, listByStatus_delegatesToRepo) {
    std::vector<Order> fakeList = {makeOrder(), makeOrder(), makeOrder()};
    EXPECT_CALL(mockOrders, findByStatus(OrderStatus::RESERVED)).WillOnce(Return(fakeList));

    auto result = service.listByStatus(OrderStatus::RESERVED);
    EXPECT_EQ(result.size(), 3u);
}
