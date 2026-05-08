#include "ConsoleReleaseView.h"
#include "../model/Order.h"
#include <iostream>
#include <iomanip>

void ConsoleReleaseView::showConfirmedOrders(const std::vector<Order>& orders) {
    std::cout << "\n[출고 처리] 출고 가능 주문 (CONFIRMED) " << orders.size() << "건\n";
    std::cout << std::left
              << std::setw(22) << "주문번호"
              << std::setw(20) << "고객명"
              << "수량\n";
    std::cout << std::string(50, '-') << "\n";
    for (const auto& o : orders)
        std::cout << std::left
                  << std::setw(22) << o.orderId
                  << std::setw(20) << o.customerName
                  << o.quantity << " ea\n";
}

std::string ConsoleReleaseView::getOrderId() {
    std::cout << "출고할 주문 번호 > ";
    std::string id; std::getline(std::cin, id);
    return id;
}

void ConsoleReleaseView::showSuccess(const std::string& msg) { std::cout << "[완료] " << msg << "\n"; }
void ConsoleReleaseView::showError(const std::string& msg)   { std::cout << "[오류] " << msg << "\n"; }
