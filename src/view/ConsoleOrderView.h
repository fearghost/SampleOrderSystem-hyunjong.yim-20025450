#pragma once
#include "OrderView.h"

class ConsoleOrderView : public OrderView {
public:
    void        showSubMenu() override;
    int         getMenuChoice() override;
    OrderInput  getOrderInput() override;
    std::string getOrderId(const std::string& prompt) override;
    void        showOrderList(const std::vector<Order>&, const std::string& header) override;
    void        showSuccess(const std::string& msg) override;
    void        showError(const std::string& msg) override;
};
