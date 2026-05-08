#include <gtest/gtest.h>
#include "../../src/repository/InMemoryOrderRepository.h"

class InMemoryOrderRepositoryTest : public ::testing::Test {
protected:
    InMemoryOrderRepository repo;

    Order makeOrder(const std::string& id, OrderStatus status = OrderStatus::RESERVED) {
        return {id, "S-001", "고객A", 100, status, "2026-05-08"};
    }
};

TEST_F(InMemoryOrderRepositoryTest, save_and_findById) {
    repo.save(makeOrder("ORD-001"));
    auto result = repo.findById("ORD-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->orderId, "ORD-001");
    EXPECT_EQ(result->status, OrderStatus::RESERVED);
}

TEST_F(InMemoryOrderRepositoryTest, findByStatus_reserved) {
    repo.save(makeOrder("ORD-001", OrderStatus::RESERVED));
    repo.save(makeOrder("ORD-002", OrderStatus::RESERVED));
    repo.save(makeOrder("ORD-003", OrderStatus::CONFIRMED));
    auto result = repo.findByStatus(OrderStatus::RESERVED);
    EXPECT_EQ(result.size(), 2u);
}

TEST_F(InMemoryOrderRepositoryTest, updateStatus_changes_state) {
    repo.save(makeOrder("ORD-001", OrderStatus::RESERVED));
    repo.updateStatus("ORD-001", OrderStatus::CONFIRMED);
    EXPECT_EQ(repo.findById("ORD-001")->status, OrderStatus::CONFIRMED);
}

TEST_F(InMemoryOrderRepositoryTest, findByStatus_excludes_other) {
    repo.save(makeOrder("ORD-001", OrderStatus::REJECTED));
    auto result = repo.findByStatus(OrderStatus::RESERVED);
    EXPECT_EQ(result.size(), 0u);
}

TEST_F(InMemoryOrderRepositoryTest, count_reflects_saves) {
    repo.save(makeOrder("ORD-001"));
    repo.save(makeOrder("ORD-002"));
    EXPECT_EQ(repo.count(), 2);
}

TEST_F(InMemoryOrderRepositoryTest, save_upsert_updates_existing) {
    repo.save(makeOrder("ORD-001", OrderStatus::RESERVED));
    repo.save(makeOrder("ORD-001", OrderStatus::CONFIRMED));
    EXPECT_EQ(repo.count(), 1);
    EXPECT_EQ(repo.findById("ORD-001")->status, OrderStatus::CONFIRMED);
}
