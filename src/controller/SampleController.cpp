#include "SampleController.h"
#include <stdexcept>

SampleController::SampleController(SampleService& service, SampleView& view)
    : service_(service), view_(view) {}

void SampleController::run() {
    int choice;
    do {
        view_.showSubMenu();
        choice = view_.getMenuChoice();
        switch (choice) {
            case 1: handleRegister(); break;
            case 2: handleList();     break;
            case 3: handleSearch();   break;
        }
    } while (choice != 0);
}

void SampleController::handleRegister() {
    auto input = view_.getRegisterInput();
    try {
        service_.registerSample(input.id, input.name,
                                input.avgProductionTime, input.yieldRate, input.initialStock);
        view_.showSuccess("시료 등록이 완료됐습니다.");
    } catch (const std::exception& e) {
        view_.showError(e.what());
    }
}

void SampleController::handleList() {
    auto samples = service_.listAll();
    view_.showSampleList(samples, "등록 시료 목록");
}

void SampleController::handleSearch() {
    auto keyword = view_.getSearchKeyword();
    auto result  = service_.searchByName(keyword);
    view_.showSampleList(result, "검색 결과");
}
