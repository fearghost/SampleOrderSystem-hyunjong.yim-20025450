#pragma once
#include "../service/SampleService.h"
#include "../service/OrderService.h"
#include "../view/MonitorView.h"

class MonitorController {
public:
    MonitorController(SampleService& sampleService,
                      OrderService&  orderService,
                      MonitorView&   view);
    void run();

private:
    SampleService& sampleService_;
    OrderService&  orderService_;
    MonitorView&   view_;
};
