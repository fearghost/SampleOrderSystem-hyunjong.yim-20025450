#include "ConsoleProductionView.h"
#include <iostream>
#include <iomanip>

void ConsoleProductionView::showProductionStatus(
    const std::optional<ProductionJob>& current,
    const std::vector<ProductionJob>&   waiting) {

    std::cout << "\n[생산라인]  FIFO 방식\n";
    std::cout << "── 현재 생산 중 ──────────────────────\n";
    if (current.has_value()) {
        std::cout << " 주문번호: " << current->orderId
                  << "  시료: " << current->sampleId
                  << "  실생산량: " << current->actualQty << "ea"
                  << "  예상시간: " << current->totalTime << "min\n";
    } else {
        std::cout << " (대기 중인 작업 없음)\n";
    }

    std::cout << "\n── 대기 큐 (" << waiting.size() << "건) ─────────────────\n";
    for (size_t i = 0; i < waiting.size(); ++i)
        std::cout << " [" << (i + 1) << "] " << waiting[i].orderId
                  << "  실생산량: " << waiting[i].actualQty << "ea\n";

    std::cout << "\n [1] 생산 완료 처리   [0] 위로\n";
}

int  ConsoleProductionView::getMenuChoice() {
    std::cout << "선택 > ";
    int c = 0; std::cin >> c; std::cin.ignore();
    return c;
}
void ConsoleProductionView::showSuccess(const std::string& msg) { std::cout << "[완료] " << msg << "\n"; }
void ConsoleProductionView::showError(const std::string& msg)   { std::cout << "[오류] " << msg << "\n"; }
