#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../test/MockServices.h"
#include "../../test/MockViews.h"
#include "../../src/controller/ReleaseController.h"

using ::testing::Return;
using ::testing::_;
using ::testing::Throw;

class ReleaseControllerTest : public ::testing::Test {
protected:
    MockOrderService  mockService;
    MockReleaseView   mockView;
    ReleaseController controller{mockService, mockView};

    Order makeOrder(const std::string& id = "ORD-001") {
        return {id, "S-001", "고객A", 150, OrderStatus::CONFIRMED, "2026-05-08"};
    }
};

// 1. showConfirmedOrders 호출 후 번호 선택 → processShipment
TEST_F(ReleaseControllerTest, run_showsConfirmedOrders) {
    std::vector<Order> confirmed = {makeOrder()};
    EXPECT_CALL(mockService, listByStatus(OrderStatus::CONFIRMED)).WillOnce(Return(confirmed));
    EXPECT_CALL(mockView, showConfirmedOrders(confirmed)).Times(1);
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(1));
    EXPECT_CALL(mockService, processShipment("ORD-001"));
    EXPECT_CALL(mockView, showSuccess(_));

    controller.run();
}

// 2. 번호 선택(1) → processShipment 호출
TEST_F(ReleaseControllerTest, release_callsProcessShipment) {
    std::vector<Order> confirmed = {makeOrder("ORD-042")};
    EXPECT_CALL(mockService, listByStatus(OrderStatus::CONFIRMED)).WillOnce(Return(confirmed));
    EXPECT_CALL(mockView, showConfirmedOrders(_));
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(1));
    EXPECT_CALL(mockService, processShipment("ORD-042")).Times(1);
    EXPECT_CALL(mockView, showSuccess(_));

    controller.run();
}

// 3. processShipment throws → showError 호출
TEST_F(ReleaseControllerTest, release_serviceThrows_showsError) {
    std::vector<Order> confirmed = {makeOrder()};
    EXPECT_CALL(mockService, listByStatus(OrderStatus::CONFIRMED)).WillOnce(Return(confirmed));
    EXPECT_CALL(mockView, showConfirmedOrders(_));
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(1));
    EXPECT_CALL(mockService, processShipment("ORD-001"))
        .WillOnce(Throw(std::logic_error("출고 오류")));
    EXPECT_CALL(mockView, showError(_)).Times(1);

    controller.run();
}

// 4. 출고 가능 주문 없음 → 안내 메시지
TEST_F(ReleaseControllerTest, run_emptyList_showsMessage) {
    EXPECT_CALL(mockService, listByStatus(OrderStatus::CONFIRMED))
        .WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(mockView, showConfirmedOrders(_));
    EXPECT_CALL(mockView, showSuccess(_));

    controller.run();
}
