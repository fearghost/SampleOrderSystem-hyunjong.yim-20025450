#include <gtest/gtest.h>
#include <cstdio>
#include "../../src/repository/JsonProductionRepository.h"

class JsonProductionRepositoryTest : public ::testing::Test {
protected:
    std::string testFile = "test_production_tmp.json";
    JsonProductionRepository repo{testFile};

    void TearDown() override { std::remove(testFile.c_str()); }

    ProductionJob makeJob(const std::string& orderId, int actualQty = 206) {
        return {orderId, "S-001", 170, actualQty, 164.8};
    }
};

// 1. A, B 추가 후 재로드 → front()==A (FIFO 보존)
TEST_F(JsonProductionRepositoryTest, enqueue_persistsOrder) {
    repo.enqueue(makeJob("ORD-A"));
    repo.enqueue(makeJob("ORD-B"));
    JsonProductionRepository repo2{testFile};
    auto front = repo2.front();
    ASSERT_TRUE(front.has_value());
    EXPECT_EQ(front->orderId, "ORD-A");
}

// 2. dequeue 후 재로드 → size 감소
TEST_F(JsonProductionRepositoryTest, dequeue_removesFromFile) {
    repo.enqueue(makeJob("ORD-A"));
    repo.enqueue(makeJob("ORD-B"));
    repo.dequeue();
    JsonProductionRepository repo2{testFile};
    EXPECT_EQ(repo2.size(), 1);
    EXPECT_EQ(repo2.front()->orderId, "ORD-B");
}
