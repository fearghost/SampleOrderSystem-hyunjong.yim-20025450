#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../test/MockServices.h"
#include "../../test/MockViews.h"
#include "../../src/controller/MonitorController.h"

using ::testing::Return;
using ::testing::_;

class MonitorControllerTest : public ::testing::Test {
protected:
    MockSampleService  mockSampleService;
    MockOrderService   mockOrderService;
    MockMonitorView    mockView;
    MonitorController  controller{mockSampleService, mockOrderService, mockView};
};

// 1. sampleService.listAll() 1회 호출
TEST_F(MonitorControllerTest, run_callsListAll_onSampleService) {
    EXPECT_CALL(mockSampleService, listAll()).WillOnce(Return(std::vector<Sample>{}));
    EXPECT_CALL(mockOrderService,  listAll()).WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(mockView, showMonitor(_, _));

    controller.run();
}

// 2. orderService.listAll() 1회 호출
TEST_F(MonitorControllerTest, run_callsListAll_onOrderService) {
    EXPECT_CALL(mockSampleService, listAll()).WillOnce(Return(std::vector<Sample>{}));
    EXPECT_CALL(mockOrderService,  listAll()).WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(mockView, showMonitor(_, _));

    controller.run();
}

// 3. showMonitor에 올바른 데이터 전달
TEST_F(MonitorControllerTest, run_passesDataToView) {
    std::vector<Sample> fakeSamples = {{"S-001", "GaN", 0.3, 0.78, 100}};
    std::vector<Order>  fakeOrders  = {{"ORD-001", "S-001", "고객A", 100,
                                        OrderStatus::RESERVED, "2026-05-08"}};
    EXPECT_CALL(mockSampleService, listAll()).WillOnce(Return(fakeSamples));
    EXPECT_CALL(mockOrderService,  listAll()).WillOnce(Return(fakeOrders));
    EXPECT_CALL(mockView, showMonitor(fakeOrders, fakeSamples)).Times(1);

    controller.run();
}
