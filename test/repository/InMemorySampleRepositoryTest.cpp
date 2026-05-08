#include <gtest/gtest.h>
#include "../../src/repository/InMemorySampleRepository.h"

class InMemorySampleRepositoryTest : public ::testing::Test {
protected:
    InMemorySampleRepository repo;

    Sample makeSample(const std::string& id, const std::string& name, int stock = 100) {
        return {id, name, 0.5, 0.92, stock};
    }
};

TEST_F(InMemorySampleRepositoryTest, save_and_findById) {
    repo.save(makeSample("S-001", "실리콘 웨이퍼-8인치"));
    auto result = repo.findById("S-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id, "S-001");
    EXPECT_EQ(result->name, "실리콘 웨이퍼-8인치");
}

TEST_F(InMemorySampleRepositoryTest, findById_not_found) {
    auto result = repo.findById("X-999");
    EXPECT_FALSE(result.has_value());
}

TEST_F(InMemorySampleRepositoryTest, existsById_true) {
    repo.save(makeSample("S-001", "실리콘 웨이퍼-8인치"));
    EXPECT_TRUE(repo.existsById("S-001"));
}

TEST_F(InMemorySampleRepositoryTest, existsById_false) {
    EXPECT_FALSE(repo.existsById("S-999"));
}

TEST_F(InMemorySampleRepositoryTest, findAll_returns_all) {
    repo.save(makeSample("S-001", "A"));
    repo.save(makeSample("S-002", "B"));
    repo.save(makeSample("S-003", "C"));
    EXPECT_EQ(repo.findAll().size(), 3u);
}

TEST_F(InMemorySampleRepositoryTest, findByName_partial_match) {
    repo.save(makeSample("S-001", "실리콘 웨이퍼-8인치"));
    repo.save(makeSample("S-002", "GaN 에피택셀-4인치"));
    repo.save(makeSample("S-003", "산화막 웨이퍼-SiO2"));

    auto result = repo.findByName("웨이퍼");
    EXPECT_EQ(result.size(), 2u);
}

TEST_F(InMemorySampleRepositoryTest, updateStock_positive) {
    repo.save(makeSample("S-001", "A", 100));
    repo.updateStock("S-001", 50);
    EXPECT_EQ(repo.findById("S-001")->stock, 150);
}

TEST_F(InMemorySampleRepositoryTest, updateStock_negative) {
    repo.save(makeSample("S-001", "A", 100));
    repo.updateStock("S-001", -30);
    EXPECT_EQ(repo.findById("S-001")->stock, 70);
}

TEST_F(InMemorySampleRepositoryTest, count_reflects_saves) {
    repo.save(makeSample("S-001", "A"));
    repo.save(makeSample("S-002", "B"));
    repo.save(makeSample("S-003", "C"));
    EXPECT_EQ(repo.count(), 3);
}

TEST_F(InMemorySampleRepositoryTest, save_upsert_updates_existing) {
    repo.save(makeSample("S-001", "Original", 100));
    repo.save(makeSample("S-001", "Updated", 200));
    EXPECT_EQ(repo.count(), 1);
    EXPECT_EQ(repo.findById("S-001")->name, "Updated");
}
