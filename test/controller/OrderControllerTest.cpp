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

// 1. run() → placeOrder 호출
TEST_F(OrderControllerTest, placeOrder_callsService) {
    OrderInput input{"S-001", "고객A", 200};
    EXPECT_CALL(mockView, getOrderInput()).WillOnce(Return(input));
    EXPECT_CALL(mockService, placeOrder("S-001", "고객A", 200, _, _)).Times(1);
    EXPECT_CALL(mockView, showSuccess(_));

    controller.run();
}

// 2. runApproval() 메뉴 1 → approveOrder 호출
TEST_F(OrderControllerTest, approveOrder_callsService) {
    std::vector<Order> reserved = {makeOrder()};
    EXPECT_CALL(mockService, listByStatus(OrderStatus::RESERVED)).WillOnce(Return(reserved));
    EXPECT_CALL(mockView, showOrderList(reserved, _));
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(1));
    EXPECT_CALL(mockView, getOrderId(_)).WillOnce(Return("ORD-001"));
    EXPECT_CALL(mockService, approveOrder("ORD-001")).Times(1);
    EXPECT_CALL(mockView, showSuccess(_));

    controller.runApproval();
}

// 3. runApproval() 메뉴 2 → rejectOrder 호출
TEST_F(OrderControllerTest, rejectOrder_callsService) {
    std::vector<Order> reserved = {makeOrder()};
    EXPECT_CALL(mockService, listByStatus(OrderStatus::RESERVED)).WillOnce(Return(reserved));
    EXPECT_CALL(mockView, showOrderList(reserved, _));
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(2));
    EXPECT_CALL(mockView, getOrderId(_)).WillOnce(Return("ORD-001"));
    EXPECT_CALL(mockService, rejectOrder("ORD-001")).Times(1);
    EXPECT_CALL(mockView, showSuccess(_));

    controller.runApproval();
}

// 4. approveOrder throws → showError 호출
TEST_F(OrderControllerTest, approveOrder_serviceThrows_showsError) {
    std::vector<Order> reserved = {makeOrder()};
    EXPECT_CALL(mockService, listByStatus(OrderStatus::RESERVED)).WillOnce(Return(reserved));
    EXPECT_CALL(mockView, showOrderList(_, _));
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(1));
    EXPECT_CALL(mockView, getOrderId(_)).WillOnce(Return("ORD-001"));
    EXPECT_CALL(mockService, approveOrder("ORD-001"))
        .WillOnce(Throw(std::logic_error("상태 오류")));
    EXPECT_CALL(mockView, showError(_)).Times(1);

    controller.runApproval();
}
