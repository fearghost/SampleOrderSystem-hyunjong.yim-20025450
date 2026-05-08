#pragma once
#include "ReleaseView.h"

class ConsoleReleaseView : public ReleaseView {
public:
    void        showConfirmedOrders(const std::vector<Order>& orders) override;
    int         getMenuChoice() override;
    std::string getOrderId() override;
    void        showSuccess(const std::string& msg) override;
    void        showError(const std::string& msg) override;
};
