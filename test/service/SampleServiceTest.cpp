#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../test/MockRepositories.h"
#include "../../src/service/SampleService.h"

using ::testing::Return;
using ::testing::_;
using ::testing::SaveArg;

class SampleServiceTest : public ::testing::Test {
protected:
    MockSampleRepository mockRepo;
    SampleService        service{mockRepo};

    Sample makeSample(const std::string& id = "S-001",
                      const std::string& name = "실리콘 웨이퍼-8인치") {
        return {id, name, 0.5, 0.92, 480};
    }
};

// 1. 정상 등록 시 save() 1회 호출, ID 일치
TEST_F(SampleServiceTest, registerSample_callsSave) {
    EXPECT_CALL(mockRepo, existsById("S-001")).WillOnce(Return(false));
    EXPECT_CALL(mockRepo, save(::testing::Field(&Sample::id, "S-001"))).Times(1);

    service.registerSample("S-001", "실리콘 웨이퍼-8인치", 0.5, 0.92, 480);
}

// 2. 중복 ID → std::invalid_argument
TEST_F(SampleServiceTest, registerSample_duplicateId_throws) {
    EXPECT_CALL(mockRepo, existsById("S-001")).WillOnce(Return(true));

    EXPECT_THROW(
        service.registerSample("S-001", "실리콘 웨이퍼-8인치", 0.5, 0.92, 480),
        std::invalid_argument
    );
}

// 3. save() 인자의 name, yieldRate 정확성
TEST_F(SampleServiceTest, registerSample_setsFieldsCorrectly) {
    Sample captured;
    EXPECT_CALL(mockRepo, existsById("S-002")).WillOnce(Return(false));
    EXPECT_CALL(mockRepo, save(_)).WillOnce(SaveArg<0>(&captured));

    service.registerSample("S-002", "GaN 에피택셀-4인치", 0.3, 0.78, 220);

    EXPECT_EQ(captured.name, "GaN 에피택셀-4인치");
    EXPECT_DOUBLE_EQ(captured.yieldRate, 0.78);
    EXPECT_DOUBLE_EQ(captured.avgProductionTime, 0.3);
    EXPECT_EQ(captured.stock, 220);
}

// 4. listAll → repo.findAll() 결과 그대로
TEST_F(SampleServiceTest, listAll_delegatesToRepo) {
    std::vector<Sample> fakeList = {makeSample("S-001"), makeSample("S-002"), makeSample("S-003")};
    EXPECT_CALL(mockRepo, findAll()).WillOnce(Return(fakeList));

    auto result = service.listAll();
    EXPECT_EQ(result.size(), 3u);
}

// 5. searchByName → repo.findByName() 결과 그대로
TEST_F(SampleServiceTest, searchByName_delegatesToRepo) {
    std::vector<Sample> fakeList = {makeSample("S-002", "GaN 에피택셀-4인치")};
    EXPECT_CALL(mockRepo, findByName("GaN")).WillOnce(Return(fakeList));

    auto result = service.searchByName("GaN");
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].name, "GaN 에피택셀-4인치");
}

// 6. findById — 존재
TEST_F(SampleServiceTest, findById_found) {
    EXPECT_CALL(mockRepo, findById("S-001")).WillOnce(Return(makeSample()));

    auto result = service.findById("S-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id, "S-001");
}

// 7. findById — 미존재
TEST_F(SampleServiceTest, findById_notFound) {
    EXPECT_CALL(mockRepo, findById("X-999")).WillOnce(Return(std::nullopt));

    auto result = service.findById("X-999");
    EXPECT_FALSE(result.has_value());
}

// 8. deductStock → updateStock(id, -quantity)
TEST_F(SampleServiceTest, deductStock_callsUpdateStockNegative) {
    EXPECT_CALL(mockRepo, updateStock("S-001", -50)).Times(1);

    service.deductStock("S-001", 50);
}

// 9. addStock → updateStock(id, +quantity)
TEST_F(SampleServiceTest, addStock_callsUpdateStockPositive) {
    EXPECT_CALL(mockRepo, updateStock("S-001", 100)).Times(1);

    service.addStock("S-001", 100);
}

// 10. totalSampleCount → repo.count()
TEST_F(SampleServiceTest, totalSampleCount_delegatesToRepo) {
    EXPECT_CALL(mockRepo, count()).WillOnce(Return(5));

    EXPECT_EQ(service.totalSampleCount(), 5);
}
