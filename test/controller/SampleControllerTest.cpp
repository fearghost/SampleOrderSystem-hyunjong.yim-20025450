#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../test/MockServices.h"
#include "../../test/MockViews.h"
#include "../../src/controller/SampleController.h"

using ::testing::Return;
using ::testing::_;
using ::testing::Throw;

class SampleControllerTest : public ::testing::Test {
protected:
    MockSampleService  mockService;
    MockSampleView     mockView;
    SampleController   controller{mockService, mockView};

    SampleInput makeInput(const std::string& id = "S-001") {
        return {id, "GaN 에피택셀", 0.3, 0.78, 100};
    }
};

// 1. 메뉴 1 선택 → registerSample 호출
TEST_F(SampleControllerTest, run_choice1_callsRegister) {
    EXPECT_CALL(mockView, showSubMenu()).Times(2);
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(1)).WillOnce(Return(0));
    EXPECT_CALL(mockView, getRegisterInput()).WillOnce(Return(makeInput()));
    EXPECT_CALL(mockService, registerSample("S-001", "GaN 에피택셀", 0.3, 0.78, 100)).Times(1);
    EXPECT_CALL(mockView, showSuccess(_));

    controller.run();
}

// 2. 메뉴 2 선택 → listAll + showSampleList 호출
TEST_F(SampleControllerTest, run_choice2_callsList) {
    std::vector<Sample> fake = {{"S-001", "GaN", 0.3, 0.78, 100}};
    EXPECT_CALL(mockView, showSubMenu()).Times(2);
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(2)).WillOnce(Return(0));
    EXPECT_CALL(mockService, listAll()).WillOnce(Return(fake));
    EXPECT_CALL(mockView, showSampleList(fake, _)).Times(1);

    controller.run();
}

// 3. 메뉴 3 선택 → getSearchKeyword + searchByName + showSampleList 호출
TEST_F(SampleControllerTest, run_choice3_callsSearch) {
    std::vector<Sample> result = {{"S-002", "GaN 에피택셀", 0.3, 0.78, 220}};
    EXPECT_CALL(mockView, showSubMenu()).Times(2);
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(3)).WillOnce(Return(0));
    EXPECT_CALL(mockView, getSearchKeyword()).WillOnce(Return("GaN"));
    EXPECT_CALL(mockService, searchByName("GaN")).WillOnce(Return(result));
    EXPECT_CALL(mockView, showSampleList(result, _)).Times(1);

    controller.run();
}

// 4. registerSample throws → showError 호출
TEST_F(SampleControllerTest, register_duplicateId_showsError) {
    EXPECT_CALL(mockView, showSubMenu()).Times(2);
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(1)).WillOnce(Return(0));
    EXPECT_CALL(mockView, getRegisterInput()).WillOnce(Return(makeInput()));
    EXPECT_CALL(mockService, registerSample(_, _, _, _, _))
        .WillOnce(Throw(std::invalid_argument("중복 ID")));
    EXPECT_CALL(mockView, showError(_)).Times(1);

    controller.run();
}

// 5. 메뉴 0 → 즉시 종료
TEST_F(SampleControllerTest, run_choice0_exits) {
    EXPECT_CALL(mockView, showSubMenu()).Times(1);
    EXPECT_CALL(mockView, getMenuChoice()).WillOnce(Return(0));

    controller.run();
}
