#include <gtest/gtest.h>
#include "../../src/repository/InMemoryProductionRepository.h"

class InMemoryProductionRepositoryTest : public ::testing::Test {
protected:
    InMemoryProductionRepository repo;

    ProductionJob makeJob(const std::string& orderId, int actualQty = 100) {
        return {orderId, "S-001", 80, actualQty, 50.0};
    }
};

TEST_F(InMemoryProductionRepositoryTest, enqueue_and_front) {
    repo.enqueue(makeJob("ORD-001"));
    auto result = repo.front();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->orderId, "ORD-001");
}

TEST_F(InMemoryProductionRepositoryTest, fifo_order) {
    repo.enqueue(makeJob("ORD-A"));
    repo.enqueue(makeJob("ORD-B"));
    repo.enqueue(makeJob("ORD-C"));
    EXPECT_EQ(repo.front()->orderId, "ORD-A");
}

TEST_F(InMemoryProductionRepositoryTest, dequeue_advances_front) {
    repo.enqueue(makeJob("ORD-A"));
    repo.enqueue(makeJob("ORD-B"));
    repo.dequeue();
    EXPECT_EQ(repo.front()->orderId, "ORD-B");
}

TEST_F(InMemoryProductionRepositoryTest, isEmpty_true_when_empty) {
    EXPECT_TRUE(repo.isEmpty());
}

TEST_F(InMemoryProductionRepositoryTest, isEmpty_false_after_enqueue) {
    repo.enqueue(makeJob("ORD-001"));
    EXPECT_FALSE(repo.isEmpty());
}

TEST_F(InMemoryProductionRepositoryTest, waitingJobs_excludes_front) {
    repo.enqueue(makeJob("ORD-A"));
    repo.enqueue(makeJob("ORD-B"));
    repo.enqueue(makeJob("ORD-C"));
    auto waiting = repo.waitingJobs();
    ASSERT_EQ(waiting.size(), 2u);
    EXPECT_EQ(waiting[0].orderId, "ORD-B");
    EXPECT_EQ(waiting[1].orderId, "ORD-C");
}

TEST_F(InMemoryProductionRepositoryTest, front_returns_nullopt_when_empty) {
    EXPECT_FALSE(repo.front().has_value());
}

TEST_F(InMemoryProductionRepositoryTest, size_reflects_queue) {
    repo.enqueue(makeJob("ORD-A"));
    repo.enqueue(makeJob("ORD-B"));
    EXPECT_EQ(repo.size(), 2);
    repo.dequeue();
    EXPECT_EQ(repo.size(), 1);
}

TEST_F(InMemoryProductionRepositoryTest, waitingJobs_empty_when_single) {
    repo.enqueue(makeJob("ORD-A"));
    EXPECT_TRUE(repo.waitingJobs().empty());
}
