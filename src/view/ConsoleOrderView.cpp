#include "ConsoleOrderView.h"
#include "../model/Order.h"
#include <iostream>
#include <iomanip>

namespace {
    constexpr int kColIndex        = 6;
    constexpr int kColOrderId      = 26;  // ORD-YYYYMMDD-NNNN(17) + 여유
    constexpr int kColCustomerName = 20;
    constexpr int kColSampleId     = 10;
    constexpr int kColQuantity     = 8;
    constexpr int kSeparatorWidth  = kColIndex + kColOrderId + kColCustomerName
                                     + kColSampleId + kColQuantity + 10;  // +10: 상태 컬럼

    // setw는 바이트 기준이므로 한글(3바이트, 시각 2칸) 헤더는 정렬이 틀어진다.
    // 3바이트 UTF-8 문자(한글) 수 K만큼 setw를 보정한다: setw(K + visualWidth)
    int ksetw(const std::string& text, int visualWidth) {
        int k = 0;
        for (size_t i = 0; i < text.size(); ) {
            unsigned char c = static_cast<unsigned char>(text[i]);
            if      ((c & 0xF0) == 0xE0) { k++; i += 3; }  // 3바이트 UTF-8 (한글)
            else if ((c & 0x80) == 0x00) { i += 1; }         // ASCII
            else if ((c & 0xE0) == 0xC0) { i += 2; }         // 2바이트 UTF-8
            else                          { i += 4; }          // 4바이트 UTF-8
        }
        return k + visualWidth;
    }
}

void ConsoleOrderView::showSubMenu() {
    std::cout << " [1] 승인   [2] 거절   [0] 취소\n";
}

int ConsoleOrderView::getMenuChoice() {
    std::cout << "선택 > ";
    int choice = 0;
    std::cin >> choice;
    std::cin.ignore();
    return choice;
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
              << std::setw(ksetw("번호",   kColIndex))        << "번호"
              << std::setw(ksetw("주문번호",kColOrderId))     << "주문번호"
              << std::setw(ksetw("고객명", kColCustomerName)) << "고객명"
              << std::setw(ksetw("시료 ID",kColSampleId))     << "시료 ID"
              << std::setw(ksetw("수량",   kColQuantity))     << "수량"
              << "상태\n";
    std::cout << std::string(kSeparatorWidth, '-') << "\n";
    for (size_t i = 0; i < list.size(); ++i) {
        const auto& o = list[i];
        std::cout << std::left
                  << std::setw(kColIndex)        << ("[" + std::to_string(i + 1) + "]")
                  << std::setw(kColOrderId)      << o.orderId
                  << std::setw(kColCustomerName) << o.customerName
                  << std::setw(kColSampleId)     << o.sampleId
                  << std::setw(kColQuantity)     << o.quantity
                  << statusToString(o.status) << "\n";
    }
    std::cout << "\n처리할 주문 번호를 선택하세요 (0: 위로)\n";
}

void ConsoleOrderView::showSuccess(const std::string& msg) { std::cout << "[완료] " << msg << "\n"; }
void ConsoleOrderView::showError(const std::string& msg)   { std::cout << "[오류] " << msg << "\n"; }
