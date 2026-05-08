#include "ConsoleProductionView.h"
#include <iostream>
#include <iomanip>

namespace {
    constexpr int kColIdx      = 6;
    constexpr int kColOrderId  = 26;
    constexpr int kColShortage = 10;
    constexpr int kColActual   = 10;
    constexpr int kColTime     = 14;
    constexpr int kQueueSepWidth = kColIdx + kColOrderId + kColShortage + kColActual + kColTime;

    int ksetw(const std::string& text, int visualWidth) {
        int k = 0;
        for (size_t i = 0; i < text.size(); ) {
            unsigned char c = static_cast<unsigned char>(text[i]);
            if      ((c & 0xF0) == 0xE0) { k++; i += 3; }
            else if ((c & 0x80) == 0x00) { i += 1; }
            else if ((c & 0xE0) == 0xC0) { i += 2; }
            else                          { i += 4; }
        }
        return k + visualWidth;
    }
}

void ConsoleProductionView::showProductionStatus(
    const std::optional<ProductionJob>& current,
    const std::vector<ProductionJob>&   waiting) {

    const int totalJobs = (current.has_value() ? 1 : 0) + static_cast<int>(waiting.size());

    std::cout << "\n[생산라인]  FIFO 방식  (총 " << totalJobs << "건)\n";

    if (totalJobs == 0) {
        std::cout << "  (대기 중인 생산 작업 없음)\n";
    } else {
        // 헤더: [N] 번호로 표시 — [1]이 현재 진행 중인 작업(FIFO 우선)
        std::cout << std::left
                  << std::setw(ksetw("번호",   kColIdx))     << "번호"
                  << std::setw(ksetw("주문번호",kColOrderId)) << "주문번호"
                  << std::setw(ksetw("부족분", kColShortage)) << "부족분"
                  << std::setw(ksetw("실생산량",kColActual))  << "실생산량"
                  << std::setw(ksetw("소요시간",kColTime))    << "소요시간"
                  << "상태\n";
        std::cout << std::string(kQueueSepWidth + 4, '-') << "\n";

        int idx = 1;
        if (current.has_value()) {
            const auto& job = *current;
            std::cout << std::left
                      << std::setw(kColIdx)     << ("[" + std::to_string(idx++) + "]")
                      << std::setw(kColOrderId) << job.orderId
                      << std::setw(kColShortage)<< (std::to_string(job.shortage)  + " ea")
                      << std::setw(kColActual)  << (std::to_string(job.actualQty) + " ea")
                      << std::setw(kColTime)    << (std::to_string(job.totalTime) + " min")
                      << "▶ 진행 중\n";
        }
        for (const auto& job : waiting) {
            std::cout << std::left
                      << std::setw(kColIdx)     << ("[" + std::to_string(idx++) + "]")
                      << std::setw(kColOrderId) << job.orderId
                      << std::setw(kColShortage)<< (std::to_string(job.shortage)  + " ea")
                      << std::setw(kColActual)  << (std::to_string(job.actualQty) + " ea")
                      << std::setw(kColTime)    << (std::to_string(job.totalTime) + " min")
                      << "대기\n";
        }
    }

    std::cout << "\n [1] 생산 완료 (현재 작업)   [2] 생산 취소   [0] 위로\n";
}

void ConsoleProductionView::showCancelPrompt(int jobCount) {
    std::cout << "\n취소할 작업 번호를 선택하세요 (1-" << jobCount << ", 0: 취소)\n";
}

int ConsoleProductionView::getMenuChoice() {
    std::cout << "선택 > ";
    int choice = 0;
    std::cin >> choice;
    std::cin.ignore();
    return choice;
}

void ConsoleProductionView::showSuccess(const std::string& msg) { std::cout << "[완료] " << msg << "\n"; }
void ConsoleProductionView::showError(const std::string& msg)   { std::cout << "[오류] " << msg << "\n"; }
