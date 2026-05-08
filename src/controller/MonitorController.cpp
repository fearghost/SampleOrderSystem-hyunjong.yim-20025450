#include "MonitorController.h"

MonitorController::MonitorController(SampleService& sampleService,
                                     OrderService&  orderService,
                                     MonitorView&   view)
    : sampleService_(sampleService), orderService_(orderService), view_(view) {}

void MonitorController::run() {
    auto samples = sampleService_.listAll();
    auto orders  = orderService_.listAll();
    view_.showMonitor(orders, samples);
}
