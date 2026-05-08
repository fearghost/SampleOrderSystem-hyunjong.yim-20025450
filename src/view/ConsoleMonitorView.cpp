#include "ConsoleMonitorView.h"
#include "../model/Order.h"
#include <iostream>
#include <iomanip>
#include <map>

void ConsoleMonitorView::showMonitor(const std::vector<Order>& orders,
                                     const std::vector<Sample>& samples) {
    // 상태별 주문 수
    std::map<OrderStatus, int> counts;
    for (const auto& o : orders)
        if (o.status != OrderStatus::REJECTED) counts[o.status]++;

    std::cout << "\n[모니터링]\n";
    std::cout << "── 상태별 주문 현황 ──────────────────\n";
    std::cout << " RESERVED  : " << counts[OrderStatus::RESERVED]  << "건\n";
    std::cout << " PRODUCING : " << counts[OrderStatus::PRODUCING] << "건\n";
    std::cout << " CONFIRMED : " << counts[OrderStatus::CONFIRMED] << "건\n";
    std::cout << " RELEASE   : " << counts[OrderStatus::RELEASE]   << "건\n";

    // 시료별 재고 현황
    std::cout << "\n── 시료별 재고 현황 ──────────────────\n";
    std::cout << std::left << std::setw(24) << "시료명" << std::setw(8) << "재고" << "상태\n";
    std::cout << std::string(40, '-') << "\n";
    for (const auto& s : samples) {
        std::string status = s.stock == 0 ? "고갈" : (s.stock < 50 ? "부족" : "여유");
        std::cout << std::left << std::setw(24) << s.name
                  << std::setw(8) << s.stock << status << "\n";
    }
}
