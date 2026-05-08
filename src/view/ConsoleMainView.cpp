#include "ConsoleMainView.h"
#include <iostream>
#include <string>

void ConsoleMainView::showMenu(int sampleCount, int orderCount) {
    std::cout << "\n================================================================\n";
    std::cout << "           반도체 시료 생산주문관리 시스템\n";
    std::cout << "================================================================\n";
    std::cout << " 등록 시료: " << sampleCount << "종    전체 주문: " << orderCount << "건\n";
    std::cout << "----------------------------------------------------------------\n";
    std::cout << " [1] 시료 관리        [2] 시료 주문\n";
    std::cout << " [3] 주문 승인/거절   [4] 모니터링\n";
    std::cout << " [5] 생산라인 조회    [6] 출고 처리\n";
    std::cout << " [0] 종료\n";
    std::cout << "----------------------------------------------------------------\n";
}

int ConsoleMainView::getMenuChoice() {
    std::cout << "선택 > ";
    int choice = 0;
    std::cin >> choice;
    std::cin.ignore();
    return choice;
}

void ConsoleMainView::showError(const std::string& msg) {
    std::cout << "[오류] " << msg << "\n";
}
