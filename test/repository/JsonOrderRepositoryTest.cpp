#include <gtest/gtest.h>
#include <cstdio>
#include "../../src/repository/JsonOrderRepository.h"

class JsonOrderRepositoryTest : public ::testing::Test {
protected:
    std::string testFile = "test_orders_tmp.json";
    JsonOrderRepository repo{testFile};

    void TearDown() override { std::remove(testFile.c_str()); }

    Order makeOrder(const std::string& id = "ORD-001",
                    OrderStatus s = OrderStatus::RESERVED) {
        return {id, "S-001", "삼성전자 파운드리", 200, s, "2026-05-08T09:00:00"};
    }
};

// 1. 저장 후 재로드 → status 포함 동일
TEST_F(JsonOrderRepositoryTest, save_and_reload) {
    repo.save(makeOrder());
    JsonOrderRepository repo2{testFile};
    auto result = repo2.findById("ORD-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->orderId,      "ORD-001");
    EXPECT_EQ(result->customerName, "삼성전자 파운드리");
    EXPECT_EQ(result->status,       OrderStatus::RESERVED);
}

// 2. updateStatus 후 재로드 → 변경 반영
TEST_F(JsonOrderRepositoryTest, updateStatus_persistsChange) {
    repo.save(makeOrder("ORD-001", OrderStatus::RESERVED));
    repo.updateStatus("ORD-001", OrderStatus::CONFIRMED);
    JsonOrderRepository repo2{testFile};
    EXPECT_EQ(repo2.findById("ORD-001")->status, OrderStatus::CONFIRMED);
}

// 3. PRODUCING 1건 저장 → 재로드 후 findByStatus 1건
TEST_F(JsonOrderRepositoryTest, findByStatus_afterReload) {
    repo.save(makeOrder("ORD-001", OrderStatus::PRODUCING));
    repo.save(makeOrder("ORD-002", OrderStatus::RESERVED));
    JsonOrderRepository repo2{testFile};
    auto result = repo2.findByStatus(OrderStatus::PRODUCING);
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].orderId, "ORD-001");
}
