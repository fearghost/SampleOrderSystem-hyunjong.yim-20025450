#pragma once
#include "../model/Order.h"
#include <vector>
#include <string>

struct OrderInput {
    std::string sampleId;
    std::string customerName;
    int         quantity;
};

class OrderView {
public:
    virtual ~OrderView() = default;
    virtual void        showSubMenu() = 0;
    virtual int         getMenuChoice() = 0;
    virtual OrderInput  getOrderInput() = 0;
    virtual std::string getOrderId(const std::string& prompt) = 0;
    virtual void        showOrderList(const std::vector<Order>&, const std::string& header) = 0;
    virtual void        showSuccess(const std::string& msg) = 0;
    virtual void        showError(const std::string& msg) = 0;
};
