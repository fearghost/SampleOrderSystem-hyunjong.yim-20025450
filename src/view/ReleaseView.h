#pragma once
#include "../model/Order.h"
#include <vector>
#include <string>

class ReleaseView {
public:
    virtual ~ReleaseView() = default;
    virtual void        showConfirmedOrders(const std::vector<Order>& orders) = 0;
    virtual int         getMenuChoice() = 0;
    virtual std::string getOrderId() = 0;
    virtual void        showSuccess(const std::string& msg) = 0;
    virtual void        showError(const std::string& msg) = 0;
};
