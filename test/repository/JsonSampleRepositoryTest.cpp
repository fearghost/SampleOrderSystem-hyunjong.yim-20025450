#include <gtest/gtest.h>
#include <cstdio>
#include "../../src/repository/JsonSampleRepository.h"

class JsonSampleRepositoryTest : public ::testing::Test {
protected:
    std::string testFile = "test_samples_tmp.json";
    JsonSampleRepository repo{testFile};

    void TearDown() override { std::remove(testFile.c_str()); }

    Sample makeSample(const std::string& id = "S-001",
                      const std::string& name = "실리콘 웨이퍼-8인치",
                      int stock = 480) {
        return {id, name, 0.5, 0.92, stock};
    }
};

// 1. 저장 후 새 인스턴스로 재로드 → 동일 객체
TEST_F(JsonSampleRepositoryTest, save_persistsAcrossReload) {
    repo.save(makeSample());
    JsonSampleRepository repo2{testFile};
    auto result = repo2.findById("S-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id,    "S-001");
    EXPECT_EQ(result->name,  "실리콘 웨이퍼-8인치");
    EXPECT_EQ(result->stock, 480);
}

// 2. updateStock 후 재로드 → stock 반영
TEST_F(JsonSampleRepositoryTest, updateStock_persistsDelta) {
    repo.save(makeSample("S-001", "A", 100));
    repo.updateStock("S-001", -30);
    JsonSampleRepository repo2{testFile};
    EXPECT_EQ(repo2.findById("S-001")->stock, 70);
}

// 3. 동일 ID로 2회 save → findAll() 1개만
TEST_F(JsonSampleRepositoryTest, save_upsert_updatesExisting) {
    repo.save(makeSample("S-001", "Original", 100));
    repo.save(makeSample("S-001", "Updated",  200));
    EXPECT_EQ(repo.count(), 1);
    EXPECT_EQ(repo.findById("S-001")->name, "Updated");
}

// 4. "GaN" 저장 후 "gaN" 검색 (대소문자 무관)
TEST_F(JsonSampleRepositoryTest, findByName_partialMatch) {
    repo.save(makeSample("S-001", "GaN 에피택셀-4인치", 220));
    repo.save(makeSample("S-002", "실리콘 웨이퍼-8인치", 480));
    auto result = repo.findByName("gaN");
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].id, "S-001");
}

// 5. 빈 파일 → findAll() == []
TEST_F(JsonSampleRepositoryTest, emptyFile_findAll_returnsEmpty) {
    // 파일 없는 상태의 새 repo
    JsonSampleRepository emptyRepo{"test_empty_tmp.json"};
    auto result = emptyRepo.findAll();
    EXPECT_TRUE(result.empty());
    std::remove("test_empty_tmp.json");
}
