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
    view_.showOrderList(reserved, "승인 대기 주문");
    int choice = view_.getMenuChoice();
    try {
        if (choice == 1) {
            auto orderId = view_.getOrderId("승인할 주문 번호:");
            service_.approveOrder(orderId);
            view_.showSuccess("승인 완료됐습니다.");
        } else if (choice == 2) {
            auto orderId = view_.getOrderId("거절할 주문 번호:");
            service_.rejectOrder(orderId);
            view_.showSuccess("거절 처리됐습니다.");
        }
    } catch (const std::exception& e) {
        view_.showError(e.what());
    }
}
