#include "ConsoleSampleView.h"
#include <iostream>
#include <iomanip>

void ConsoleSampleView::showSubMenu() {
    std::cout << "\n[1] 시료 관리\n";
    std::cout << " [1] 시료 등록   [2] 시료 목록   [3] 시료 검색   [0] 위로\n";
}

int ConsoleSampleView::getMenuChoice() {
    std::cout << "선택 > ";
    int c = 0; std::cin >> c; std::cin.ignore();
    return c;
}

SampleInput ConsoleSampleView::getRegisterInput() {
    SampleInput input;
    std::cout << "시료 ID > "; std::getline(std::cin, input.id);
    std::cout << "시료명  > "; std::getline(std::cin, input.name);
    std::cout << "평균 생산시간 (min/ea) > "; std::cin >> input.avgProductionTime;
    std::cout << "수율 (0~1.0) > ";           std::cin >> input.yieldRate;
    std::cout << "초기 재고    > ";           std::cin >> input.initialStock;
    std::cin.ignore();
    return input;
}

std::string ConsoleSampleView::getSearchKeyword() {
    std::cout << "검색어 > ";
    std::string kw; std::getline(std::cin, kw);
    return kw;
}

void ConsoleSampleView::showSampleList(const std::vector<Sample>& list, const std::string& header) {
    std::cout << "\n" << header << " (총 " << list.size() << "종)\n";
    std::cout << std::left
              << std::setw(8)  << "ID"
              << std::setw(24) << "시료명"
              << std::setw(12) << "생산시간"
              << std::setw(8)  << "수율"
              << "재고\n";
    std::cout << std::string(60, '-') << "\n";
    for (const auto& s : list) {
        std::cout << std::left
                  << std::setw(8)  << s.id
                  << std::setw(24) << s.name
                  << std::setw(12) << (std::to_string(s.avgProductionTime) + " min")
                  << std::setw(8)  << s.yieldRate
                  << s.stock << " ea\n";
    }
}

void ConsoleSampleView::showSuccess(const std::string& msg) { std::cout << "[완료] " << msg << "\n"; }
void ConsoleSampleView::showError(const std::string& msg)   { std::cout << "[오류] " << msg << "\n"; }
