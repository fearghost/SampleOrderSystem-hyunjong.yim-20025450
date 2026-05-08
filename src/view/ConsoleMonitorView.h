#pragma once
#include "MonitorView.h"

class ConsoleMonitorView : public MonitorView {
public:
    void showMonitor(const std::vector<Order>& orders,
                     const std::vector<Sample>& samples) override;
};
