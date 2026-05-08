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

    ProductionJob makeJob(const std::string& orderId = "ORD-001") {
        return {orderId, "S-001", 170, 206, 164.8};
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

// 2. [1] → completeCurrentJob 호출 (항상 큐 앞 FIFO 처리)
TEST_F(ProductionControllerTest, completeJob_callsService) {
    EXPECT_CALL(mockService, getCurrentJob()).WillOnce(Return(std::nullopt));
    EXPECT_CALL(mockService, getWaitingJobs()).WillOnce(Return(std::vector<ProductionJob>{}));
    EXPECT_CALL(mockView, showProductionStatus(_, _));
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(1));
    EXPECT_CALL(mockService, completeCurrentJob()).Times(1);
    EXPECT_CALL(mockView, showSuccess(_));

    controller.run();
}

// 3. [2] → 취소 번호 선택 → cancelJob 호출
TEST_F(ProductionControllerTest, cancelJob_callsService) {
    std::optional<ProductionJob> current = makeJob("ORD-001");
    std::vector<ProductionJob>   waiting = {makeJob("ORD-002")};

    EXPECT_CALL(mockService, getCurrentJob()).WillOnce(Return(current));
    EXPECT_CALL(mockService, getWaitingJobs()).WillOnce(Return(waiting));
    EXPECT_CALL(mockView, showProductionStatus(_, _));
    EXPECT_CALL(mockView, getMenuChoice())
        .WillOnce(Return(2))   // 취소 선택
        .WillOnce(Return(1));  // 첫 번째 작업(현재 진행 중) 취소
    EXPECT_CALL(mockView, showCancelPrompt(2));  // current(1) + waiting(1) = 2건
    EXPECT_CALL(mockService, cancelJob("ORD-001")).Times(1);
    EXPECT_CALL(mockView, showSuccess(_));

    controller.run();
}

// 4. [2] → 대기 작업 취소
TEST_F(ProductionControllerTest, cancelWaitingJob_callsService) {
    std::optional<ProductionJob> current = makeJob("ORD-001");
    std::vector<ProductionJob>   waiting = {makeJob("ORD-002")};

    EXPECT_CALL(mockService, getCurrentJob()).WillOnce(Return(current));
    EXPECT_CALL(mockService, getWaitingJobs()).WillOnce(Return(waiting));
    EXPECT_CALL(mockView, showProductionStatus(_, _));
    EXPECT_CALL(mockView, getMenuChoice())
        .WillOnce(Return(2))   // 취소 선택
        .WillOnce(Return(2));  // 두 번째 작업(대기) 취소
    EXPECT_CALL(mockView, showCancelPrompt(2));
    EXPECT_CALL(mockService, cancelJob("ORD-002")).Times(1);
    EXPECT_CALL(mockView, showSuccess(_));

    controller.run();
}

// 5. completeCurrentJob throws → showError
TEST_F(ProductionControllerTest, completeJob_throws_showsError) {
    EXPECT_CALL(mockService, getCurrentJob()).WillOnce(Return(std::nullopt));
    EXPECT_CALL(mockService, getWaitingJobs()).WillOnce(Return(std::vector<ProductionJob>{}));
    EXPECT_CALL(mockView, showProductionStatus(_, _));
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(1));
    EXPECT_CALL(mockService, completeCurrentJob())
        .WillOnce(Throw(std::logic_error("생산 중인 작업이 없습니다.")));
    EXPECT_CALL(mockView, showError(_));

    controller.run();
}
