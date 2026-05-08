#pragma once
#include "../service/OrderService.h"
#include "../view/OrderView.h"

class OrderController {
public:
    OrderController(OrderService& service, OrderView& view);
    void run();          // 주문 접수 (메인메뉴 [2])
    void runApproval();  // 승인/거절  (메인메뉴 [3])

private:
    OrderService& service_;
    OrderView&    view_;
};
