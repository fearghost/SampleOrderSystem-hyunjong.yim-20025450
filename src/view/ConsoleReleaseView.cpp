#include "ConsoleReleaseView.h"
#include "../model/Order.h"
#include <iostream>
#include <iomanip>

namespace {
    constexpr int kColIndex        = 6;
    constexpr int kColOrderId      = 22;
    constexpr int kColCustomerName = 20;
    constexpr int kColSampleId     = 10;
    constexpr int kColQuantity     = 8;
    constexpr int kSeparatorWidth  = 70;
}

void ConsoleReleaseView::showConfirmedOrders(const std::vector<Order>& orders) {
    std::cout << "\n[출고 처리] 출고 가능 주문 (CONFIRMED) " << orders.size() << "건\n";
    std::cout << std::left
              << std::setw(kColIndex)        << "번호"
              << std::setw(kColOrderId)      << "주문번호"
              << std::setw(kColCustomerName) << "고객명"
              << std::setw(kColSampleId)     << "시료 ID"
              << std::setw(kColQuantity)     << "수량"
              << "상태\n";
    std::cout << std::string(kSeparatorWidth, '-') << "\n";
    for (size_t i = 0; i < orders.size(); ++i) {
        const auto& o = orders[i];
        std::cout << std::left
                  << std::setw(kColIndex)        << ("[" + std::to_string(i + 1) + "]")
                  << std::setw(kColOrderId)      << o.orderId
                  << std::setw(kColCustomerName) << o.customerName
                  << std::setw(kColSampleId)     << o.sampleId
                  << std::setw(kColQuantity)     << o.quantity
                  << statusToString(o.status) << "\n";
    }
    if (!orders.empty())
        std::cout << "\n출고할 주문 번호를 선택하세요 (0: 위로)\n";
}

int ConsoleReleaseView::getMenuChoice() {
    std::cout << "선택 > ";
    int choice = 0;
    std::cin >> choice;
    std::cin.ignore();
    return choice;
}

std::string ConsoleReleaseView::getOrderId() {
    std::cout << "출고할 주문 번호 > ";
    std::string id; std::getline(std::cin, id);
    return id;
}

void ConsoleReleaseView::showSuccess(const std::string& msg) { std::cout << "[완료] " << msg << "\n"; }
void ConsoleReleaseView::showError(const std::string& msg)   { std::cout << "[오류] " << msg << "\n"; }
