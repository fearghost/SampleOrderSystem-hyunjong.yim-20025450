#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../test/MockServices.h"
#include "../../test/MockViews.h"
#include "../../src/controller/OrderController.h"

using ::testing::Return;
using ::testing::_;
using ::testing::Throw;

class OrderControllerTest : public ::testing::Test {
protected:
    MockOrderService  mockService;
    MockOrderView     mockView;
    OrderController   controller{mockService, mockView};

    Order makeOrder(const std::string& id = "ORD-001",
                    OrderStatus s = OrderStatus::RESERVED) {
        return {id, "S-001", "고객A", 200, s, "2026-05-08"};
    }
};

// 1. run() → placeOrder 호출 (orderId·createdAt은 서비스가 내부 생성)
TEST_F(OrderControllerTest, placeOrder_callsService) {
    OrderInput input{"S-001", "고객A", 200};
    EXPECT_CALL(mockView, getOrderInput()).WillOnce(Return(input));
    EXPECT_CALL(mockService, placeOrder("S-001", "고객A", 200)).Times(1);
    EXPECT_CALL(mockView, showSuccess(_));

    controller.run();
}

// 2. runApproval(): 번호 선택(1) → 승인(1) → approveOrder 호출
TEST_F(OrderControllerTest, approveOrder_callsService) {
    std::vector<Order> reserved = {makeOrder()};
    EXPECT_CALL(mockService, listByStatus(OrderStatus::RESERVED)).WillOnce(Return(reserved));
    EXPECT_CALL(mockView, showOrderList(reserved, _));
    EXPECT_CALL(mockView, getMenuChoice())
        .WillOnce(Return(1))   // 주문 #1 선택
        .WillOnce(Return(1));  // 승인
    EXPECT_CALL(mockView, showSubMenu());
    EXPECT_CALL(mockService, approveOrder("ORD-001")).Times(1);
    EXPECT_CALL(mockView, showSuccess(_));

    controller.runApproval();
}

// 3. runApproval(): 번호 선택(1) → 거절(2) → rejectOrder 호출
TEST_F(OrderControllerTest, rejectOrder_callsService) {
    std::vector<Order> reserved = {makeOrder()};
    EXPECT_CALL(mockService, listByStatus(OrderStatus::RESERVED)).WillOnce(Return(reserved));
    EXPECT_CALL(mockView, showOrderList(reserved, _));
    EXPECT_CALL(mockView, getMenuChoice())
        .WillOnce(Return(1))   // 주문 #1 선택
        .WillOnce(Return(2));  // 거절
    EXPECT_CALL(mockView, showSubMenu());
    EXPECT_CALL(mockService, rejectOrder("ORD-001")).Times(1);
    EXPECT_CALL(mockView, showSuccess(_));

    controller.runApproval();
}

// 4. approveOrder throws → showError 호출
TEST_F(OrderControllerTest, approveOrder_serviceThrows_showsError) {
    std::vector<Order> reserved = {makeOrder()};
    EXPECT_CALL(mockService, listByStatus(OrderStatus::RESERVED)).WillOnce(Return(reserved));
    EXPECT_CALL(mockView, showOrderList(_, _));
    EXPECT_CALL(mockView, getMenuChoice())
        .WillOnce(Return(1))   // 주문 #1 선택
        .WillOnce(Return(1));  // 승인 시도
    EXPECT_CALL(mockView, showSubMenu());
    EXPECT_CALL(mockService, approveOrder("ORD-001"))
        .WillOnce(Throw(std::logic_error("상태 오류")));
    EXPECT_CALL(mockView, showError(_)).Times(1);

    controller.runApproval();
}

// 5. 대기 주문 없음 → 안내 메시지
TEST_F(OrderControllerTest, runApproval_emptyList_showsMessage) {
    EXPECT_CALL(mockService, listByStatus(OrderStatus::RESERVED))
        .WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(mockView, showSuccess(_));

    controller.runApproval();
}
