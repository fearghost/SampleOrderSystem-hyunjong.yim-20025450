#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../test/MockServices.h"
#include "../../test/MockViews.h"
#include "../../src/controller/ProductionController.h"

using ::testing::Return;
using ::testing::_;
using ::testing::Throw;

class ProductionControllerTest : public ::testing::Test {
protected:
    MockProductionService  mockService;
    MockProductionView     mockView;
    ProductionController   controller{mockService, mockView};

    ProductionJob makeJob() {
        return {"ORD-001", "S-001", 170, 206, 164.8};
    }
};

// 1. getCurrentJob + getWaitingJobs 호출 후 showProductionStatus 전달
TEST_F(ProductionControllerTest, run_showsCurrentAndWaiting) {
    std::optional<ProductionJob> current = makeJob();
    std::vector<ProductionJob>   waiting = {};

    EXPECT_CALL(mockService, getCurrentJob()).WillOnce(Return(current));
    EXPECT_CALL(mockService, getWaitingJobs()).WillOnce(Return(waiting));
    EXPECT_CALL(mockView, showProductionStatus(current, waiting)).Times(1);
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(0));

    controller.run();
}

// 2. 메뉴 1 → completeCurrentJob 호출
TEST_F(ProductionControllerTest, completeJob_callsService) {
    EXPECT_CALL(mockService, getCurrentJob()).WillOnce(Return(std::nullopt));
    EXPECT_CALL(mockService, getWaitingJobs()).WillOnce(Return(std::vector<ProductionJob>{}));
    EXPECT_CALL(mockView, showProductionStatus(_, _));
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(1));
    EXPECT_CALL(mockService, completeCurrentJob()).Times(1);
    EXPECT_CALL(mockView, showSuccess(_));

    controller.run();
}
