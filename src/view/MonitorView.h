#pragma once
#include "../model/Order.h"
#include "../model/Sample.h"
#include <vector>

class MonitorView {
public:
    virtual ~MonitorView() = default;
    virtual void showMonitor(const std::vector<Order>&  orders,
                             const std::vector<Sample>& samples) = 0;
};
