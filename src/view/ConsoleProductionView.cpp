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

    // setw 한글 보정 (3바이트 UTF-8 문자 수 K만큼 setw 증가)
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

    // ── 현재 생산 중 ──
    std::cout << "\n[생산라인]  FIFO 방식\n";
    std::cout << "── 현재 생산 중 ──────────────────────\n";
    if (current.has_value()) {
        std::cout << std::left
                  << "  주문번호  : " << current->orderId  << "\n"
                  << "  시료      : " << current->sampleId << "\n"
                  << "  부족분    : " << current->shortage  << " ea\n"
                  << "  실생산량  : " << current->actualQty << " ea\n"
                  << "  소요시간  : " << current->totalTime << " min\n";
    } else {
        std::cout << "  (현재 생산 중인 작업 없음)\n";
    }

    // ── 대기 큐 ──
    std::cout << "\n── 대기 큐 (" << waiting.size() << "건) — FIFO 순 ─────────────\n";
    if (waiting.empty()) {
        std::cout << "  (대기 중인 작업 없음)\n";
    } else {
        std::cout << std::left
                  << std::setw(ksetw("번호",   kColIdx))     << "번호"
                  << std::setw(ksetw("주문번호",kColOrderId)) << "주문번호"
                  << std::setw(ksetw("부족분", kColShortage)) << "부족분"
                  << std::setw(ksetw("실생산량",kColActual))  << "실생산량"
                  << "소요시간\n";
        std::cout << std::string(kQueueSepWidth, '-') << "\n";
        for (size_t i = 0; i < waiting.size(); ++i) {
            const auto& job = waiting[i];
            std::cout << std::left
                      << std::setw(kColIdx)     << ("[" + std::to_string(i + 1) + "]")
                      << std::setw(kColOrderId) << job.orderId
                      << std::setw(kColShortage)<< (std::to_string(job.shortage) + " ea")
                      << std::setw(kColActual)  << (std::to_string(job.actualQty) + " ea")
                      << job.totalTime << " min\n";
        }
    }

    std::cout << "\n [1] 생산 완료 처리   [0] 위로\n";
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
