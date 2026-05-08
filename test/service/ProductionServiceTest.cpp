#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../test/MockRepositories.h"
#include "../../src/service/ProductionService.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;

class ProductionServiceTest : public ::testing::Test {
protected:
    MockProductionRepository mockProduction;
    MockOrderRepository      mockOrders;
    MockSampleRepository     mockSamples;
    ProductionService service{mockProduction, mockOrders, mockSamples};

    ProductionJob makeJob(const std::string& orderId  = "ORD-001",
                          const std::string& sampleId = "S-001",
                          int actualQty = 206) {
        return {orderId, sampleId, 170, actualQty, 164.8};
    }
};

// 1. isIdle — 빈 큐
TEST_F(ProductionServiceTest, isIdle_trueWhenEmpty) {
    EXPECT_CALL(mockProduction, isEmpty()).WillOnce(Return(true));
    EXPECT_TRUE(service.isIdle());
}

// 2. isIdle — 작업 있음
TEST_F(ProductionServiceTest, isIdle_falseWhenJobExists) {
    EXPECT_CALL(mockProduction, isEmpty()).WillOnce(Return(false));
    EXPECT_FALSE(service.isIdle());
}

// 3. getCurrentJob — 작업 있음
TEST_F(ProductionServiceTest, getCurrentJob_returnsJob) {
    auto job = makeJob();
    EXPECT_CALL(mockProduction, isEmpty()).WillOnce(Return(false));
    EXPECT_CALL(mockProduction, front()).WillOnce(Return(job));

    auto result = service.getCurrentJob();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->orderId, "ORD-001");
    EXPECT_EQ(result->actualQty, 206);
}

// 4. getCurrentJob — 빈 큐 → nullopt
TEST_F(ProductionServiceTest, getCurrentJob_nulloptWhenEmpty) {
    EXPECT_CALL(mockProduction, isEmpty()).WillOnce(Return(true));

    auto result = service.getCurrentJob();
    EXPECT_FALSE(result.has_value());
}

// 5. getWaitingJobs — repo 위임
TEST_F(ProductionServiceTest, getWaitingJobs_delegatesToRepo) {
    std::vector<ProductionJob> waiting = {makeJob("ORD-002"), makeJob("ORD-003")};
    EXPECT_CALL(mockProduction, waitingJobs()).WillOnce(Return(waiting));

    auto result = service.getWaitingJobs();
    ASSERT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0].orderId, "ORD-002");
}

// 6. queueSize — repo 위임
TEST_F(ProductionServiceTest, queueSize_delegatesToRepo) {
    EXPECT_CALL(mockProduction, size()).WillOnce(Return(3));
    EXPECT_EQ(service.queueSize(), 3);
}

// 7. completeCurrentJob — dequeue 1회 호출
TEST_F(ProductionServiceTest, completeCurrentJob_callsDequeue) {
    EXPECT_CALL(mockProduction, isEmpty()).WillOnce(Return(false));
    EXPECT_CALL(mockProduction, front()).WillOnce(Return(makeJob()));
    EXPECT_CALL(mockProduction, dequeue()).Times(1);
    EXPECT_CALL(mockSamples, updateStock(_, _));
    EXPECT_CALL(mockOrders,   updateStatus(_, _));

    service.completeCurrentJob();
}

// 8. completeCurrentJob — updateStock(+actualQty)
TEST_F(ProductionServiceTest, completeCurrentJob_addsStock) {
    EXPECT_CALL(mockProduction, isEmpty()).WillOnce(Return(false));
    EXPECT_CALL(mockProduction, front()).WillOnce(Return(makeJob("ORD-001", "S-001", 206)));
    EXPECT_CALL(mockProduction, dequeue());
    EXPECT_CALL(mockSamples, updateStock("S-001", 206)).Times(1);
    EXPECT_CALL(mockOrders,   updateStatus(_, _));

    service.completeCurrentJob();
}

// 9. completeCurrentJob — updateStatus(CONFIRMED)
TEST_F(ProductionServiceTest, completeCurrentJob_setsConfirmed) {
    EXPECT_CALL(mockProduction, isEmpty()).WillOnce(Return(false));
    EXPECT_CALL(mockProduction, front()).WillOnce(Return(makeJob("ORD-001")));
    EXPECT_CALL(mockProduction, dequeue());
    EXPECT_CALL(mockSamples, updateStock(_, _));
    EXPECT_CALL(mockOrders, updateStatus("ORD-001", OrderStatus::CONFIRMED)).Times(1);

    service.completeCurrentJob();
}

// 10. completeCurrentJob — dequeue → updateStock → updateStatus 순서 (InOrder)
TEST_F(ProductionServiceTest, completeCurrentJob_orderOfCalls) {
    auto job = makeJob();
    EXPECT_CALL(mockProduction, isEmpty()).WillOnce(Return(false));
    EXPECT_CALL(mockProduction, front()).WillOnce(Return(job));

    InSequence seq;
    EXPECT_CALL(mockProduction, dequeue());
    EXPECT_CALL(mockSamples, updateStock("S-001", 206));
    EXPECT_CALL(mockOrders,  updateStatus("ORD-001", OrderStatus::CONFIRMED));

    service.completeCurrentJob();
}

// 11. completeCurrentJob — 빈 큐 → std::logic_error
TEST_F(ProductionServiceTest, completeCurrentJob_emptyQueue_throws) {
    EXPECT_CALL(mockProduction, isEmpty()).WillOnce(Return(true));

    EXPECT_THROW(service.completeCurrentJob(), std::logic_error);
}

// 12. cancelJob — production_.remove() + orders_.updateStatus(REJECTED)
TEST_F(ProductionServiceTest, cancelJob_removesAndRejects) {
    EXPECT_CALL(mockProduction, remove("ORD-001")).Times(1);
    EXPECT_CALL(mockOrders, updateStatus("ORD-001", OrderStatus::REJECTED)).Times(1);

    service.cancelJob("ORD-001");
}

// 13. cancelJob — 대기 작업도 동일하게 처리
TEST_F(ProductionServiceTest, cancelJob_waitingJob_removesAndRejects) {
    EXPECT_CALL(mockProduction, remove("ORD-002")).Times(1);
    EXPECT_CALL(mockOrders, updateStatus("ORD-002", OrderStatus::REJECTED)).Times(1);

    service.cancelJob("ORD-002");
}
