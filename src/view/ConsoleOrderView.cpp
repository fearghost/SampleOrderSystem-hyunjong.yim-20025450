#include "ConsoleOrderView.h"
#include "../model/Order.h"
#include <iostream>
#include <iomanip>

void ConsoleOrderView::showSubMenu() {
    std::cout << "\n[1] 승인   [2] 거절   [0] 위로\n";
}

int ConsoleOrderView::getMenuChoice() {
    std::cout << "선택 > ";
    int c = 0; std::cin >> c; std::cin.ignore();
    return c;
}

OrderInput ConsoleOrderView::getOrderInput() {
    OrderInput input;
    std::cout << "시료 ID > ";  std::getline(std::cin, input.sampleId);
    std::cout << "고객명  > ";  std::getline(std::cin, input.customerName);
    std::cout << "수량    > ";  std::cin >> input.quantity; std::cin.ignore();
    return input;
}

std::string ConsoleOrderView::getOrderId(const std::string& prompt) {
    std::cout << prompt << " ";
    std::string id; std::getline(std::cin, id);
    return id;
}

void ConsoleOrderView::showOrderList(const std::vector<Order>& list, const std::string& header) {
    std::cout << "\n" << header << " (" << list.size() << "건)\n";
    std::cout << std::left
              << std::setw(22) << "주문번호"
              << std::setw(20) << "고객명"
              << std::setw(10) << "시료 ID"
              << std::setw(8)  << "수량"
              << "상태\n";
    std::cout << std::string(64, '-') << "\n";
    for (const auto& o : list)
        std::cout << std::left
                  << std::setw(22) << o.orderId
                  << std::setw(20) << o.customerName
                  << std::setw(10) << o.sampleId
                  << std::setw(8)  << o.quantity
                  << statusToString(o.status) << "\n";
}

void ConsoleOrderView::showSuccess(const std::string& msg) { std::cout << "[완료] " << msg << "\n"; }
void ConsoleOrderView::showError(const std::string& msg)   { std::cout << "[오류] " << msg << "\n"; }
