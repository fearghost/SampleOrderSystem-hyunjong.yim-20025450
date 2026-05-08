#pragma once
#include <gmock/gmock.h>
#include "../src/view/SampleView.h"
#include "../src/view/OrderView.h"
#include "../src/view/MonitorView.h"
#include "../src/view/ProductionView.h"
#include "../src/view/ReleaseView.h"

class MockSampleView : public SampleView {
public:
    MOCK_METHOD(void,        showSubMenu,      (),                                              (override));
    MOCK_METHOD(int,         getMenuChoice,    (),                                              (override));
    MOCK_METHOD(SampleInput, getRegisterInput, (),                                              (override));
    MOCK_METHOD(std::string, getSearchKeyword, (),                                              (override));
    MOCK_METHOD(void,        showSampleList,   (const std::vector<Sample>&, const std::string&),(override));
    MOCK_METHOD(void,        showSuccess,      (const std::string&),                            (override));
    MOCK_METHOD(void,        showError,        (const std::string&),                            (override));
};

class MockOrderView : public OrderView {
public:
    MOCK_METHOD(void,       showSubMenu,   (),                                              (override));
    MOCK_METHOD(int,        getMenuChoice, (),                                              (override));
    MOCK_METHOD(OrderInput, getOrderInput, (),                                              (override));
    MOCK_METHOD(std::string,getOrderId,    (const std::string&),                            (override));
    MOCK_METHOD(void,       showOrderList, (const std::vector<Order>&, const std::string&), (override));
    MOCK_METHOD(void,       showSuccess,   (const std::string&),                            (override));
    MOCK_METHOD(void,       showError,     (const std::string&),                            (override));
};

class MockMonitorView : public MonitorView {
public:
    MOCK_METHOD(void, showMonitor,
        (const std::vector<Order>&, const std::vector<Sample>&), (override));
};

class MockProductionView : public ProductionView {
public:
    MOCK_METHOD(void, showProductionStatus,
        (const std::optional<ProductionJob>&, const std::vector<ProductionJob>&), (override));
    MOCK_METHOD(int,  getMenuChoice, (),                   (override));
    MOCK_METHOD(void, showSuccess,   (const std::string&), (override));
    MOCK_METHOD(void, showError,     (const std::string&), (override));
};

class MockReleaseView : public ReleaseView {
public:
    MOCK_METHOD(void,        showConfirmedOrders, (const std::vector<Order>&), (override));
    MOCK_METHOD(std::string, getOrderId,          (),                          (override));
    MOCK_METHOD(void,        showSuccess,         (const std::string&),        (override));
    MOCK_METHOD(void,        showError,           (const std::string&),        (override));
};
