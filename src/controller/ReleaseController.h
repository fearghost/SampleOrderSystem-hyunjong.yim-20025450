#pragma once
#include "../service/OrderService.h"
#include "../view/ReleaseView.h"

class ReleaseController {
public:
    ReleaseController(OrderService& service, ReleaseView& view);
    void run();

private:
    OrderService& service_;
    ReleaseView&  view_;
};
