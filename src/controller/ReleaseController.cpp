#include "ReleaseController.h"
#include "../model/Order.h"
#include <stdexcept>

ReleaseController::ReleaseController(OrderService& service, ReleaseView& view)
    : service_(service), view_(view) {}

void ReleaseController::run() {
    auto confirmed = service_.listByStatus(OrderStatus::CONFIRMED);
    view_.showConfirmedOrders(confirmed);
    if (confirmed.empty()) {
        view_.showSuccess("출고 가능한 주문이 없습니다.");
        return;
    }

    int idx = view_.getMenuChoice();
    if (idx <= 0 || idx > static_cast<int>(confirmed.size())) return;

    const std::string& orderId = confirmed[idx - 1].orderId;
    try {
        service_.processShipment(orderId);
        view_.showSuccess("출고 처리됐습니다.");
    } catch (const std::exception& e) {
        view_.showError(e.what());
    }
}
