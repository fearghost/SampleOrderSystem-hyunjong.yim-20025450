#include "ConsoleMonitorView.h"
#include "../model/Order.h"
#include <iostream>
#include <iomanip>
#include <map>

namespace {
    constexpr int kColName      = 26;
    constexpr int kColStock     = 8;
    constexpr int kSeparatorWidth = kColName + kColStock + 6;  // +6: 상태 컬럼

    // setw는 바이트 기준이므로 한글(3바이트, 시각 2칸)이 섞이면 정렬이 틀어진다.
    // 3바이트 UTF-8 문자 수 K만큼 setw를 보정한다: setw(K + visualWidth)
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

void ConsoleMonitorView::showMonitor(const std::vector<Order>& orders,
                                     const std::vector<Sample>& samples) {
    // ── 상태별 주문 현황 ──
    std::map<OrderStatus, int> counts;
    for (const auto& o : orders)
        if (o.status != OrderStatus::REJECTED) counts[o.status]++;

    std::cout << "\n[모니터링]\n";
    std::cout << "── 상태별 주문 현황 ──────────────────\n";
    std::cout << " RESERVED  : " << counts[OrderStatus::RESERVED]  << "건\n";
    std::cout << " PRODUCING : " << counts[OrderStatus::PRODUCING] << "건\n";
    std::cout << " CONFIRMED : " << counts[OrderStatus::CONFIRMED] << "건\n";
    std::cout << " RELEASE   : " << counts[OrderStatus::RELEASE]   << "건\n";

    // ── 시료별 재고 현황 ──
    std::cout << "\n── 시료별 재고 현황 ──────────────────\n";
    std::cout << std::left
              << std::setw(ksetw("시료명", kColName))  << "시료명"
              << std::setw(ksetw("재고",   kColStock)) << "재고"
              << "상태\n";
    std::cout << std::string(kSeparatorWidth, '-') << "\n";
    for (const auto& s : samples) {
        std::string stockStatus = s.stock == 0 ? "고갈" : (s.stock < 50 ? "부족" : "여유");
        std::cout << std::left
                  << std::setw(ksetw(s.name, kColName)) << s.name
                  << std::setw(kColStock)               << s.stock
                  << stockStatus << "\n";
    }
}
