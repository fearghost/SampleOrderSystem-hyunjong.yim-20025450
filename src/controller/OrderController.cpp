#include "OrderController.h"
#include <stdexcept>

OrderController::OrderController(OrderService& service, OrderView& view)
    : service_(service), view_(view) {}

void OrderController::run() {
    auto input = view_.getOrderInput();
    try {
        service_.placeOrder(input.sampleId, input.customerName, input.quantity);
        view_.showSuccess("주문이 접수됐습니다.");
    } catch (const std::exception& e) {
        view_.showError(e.what());
    }
}

void OrderController::runApproval() {
    auto reserved = service_.listByStatus(OrderStatus::RESERVED);
    if (reserved.empty()) {
        view_.showSuccess("승인 대기 중인 주문이 없습니다.");
        return;
    }

    // Step 1: 번호로 주문 선택
    view_.showOrderList(reserved, "승인 대기 주문");
    int idx = view_.getMenuChoice();
    if (idx <= 0 || idx > static_cast<int>(reserved.size())) return;

    // Step 2: 선택된 주문에 대해 승인/거절 결정
    const std::string& orderId = reserved[idx - 1].orderId;
    view_.showSubMenu();
    int action = view_.getMenuChoice();

    try {
        if (action == 1) {
            service_.approveOrder(orderId);
            view_.showSuccess("승인 완료됐습니다.");
        } else if (action == 2) {
            service_.rejectOrder(orderId);
            view_.showSuccess("거절 처리됐습니다.");
        }
    } catch (const std::exception& e) {
        view_.showError(e.what());
    }
}
