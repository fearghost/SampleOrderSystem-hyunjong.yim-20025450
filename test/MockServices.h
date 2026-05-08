#pragma once
#include <gmock/gmock.h>
#include "../src/service/SampleService.h"
#include "../src/service/OrderService.h"
#include "../src/service/ProductionService.h"
#include "../src/repository/InMemorySampleRepository.h"
#include "../src/repository/InMemoryOrderRepository.h"
#include "../src/repository/InMemoryProductionRepository.h"

// 베이스 생성자용 더미 레포지토리 (정적 지역변수로 안전하게 초기화)
class MockSampleService : public SampleService {
    static ISampleRepository& dummy() {
        static InMemorySampleRepository r; return r;
    }
public:
    MockSampleService() : SampleService(dummy()) {}

    MOCK_METHOD(void, registerSample,
        (const std::string&, const std::string&, double, double, int), (override));
    MOCK_METHOD(std::vector<Sample>,   listAll,           (),                   (const, override));
    MOCK_METHOD(std::vector<Sample>,   searchByName,      (const std::string&), (const, override));
    MOCK_METHOD(std::optional<Sample>, findById,          (const std::string&), (const, override));
    MOCK_METHOD(int,                   totalSampleCount,  (),                   (const, override));
    MOCK_METHOD(void,                  deductStock,       (const std::string&, int), (override));
    MOCK_METHOD(void,                  addStock,          (const std::string&, int), (override));
};

class MockOrderService : public OrderService {
    static ISampleRepository&     dummyS() { static InMemorySampleRepository    r; return r; }
    static IOrderRepository&      dummyO() { static InMemoryOrderRepository     r; return r; }
    static IProductionRepository& dummyP() { static InMemoryProductionRepository r; return r; }
public:
    MockOrderService() : OrderService(dummyS(), dummyO(), dummyP()) {}

    MOCK_METHOD(void, placeOrder,
        (const std::string&, const std::string&, int),
        (override));
    MOCK_METHOD(void, approveOrder,       (const std::string&), (override));
    MOCK_METHOD(void, rejectOrder,        (const std::string&), (override));
    MOCK_METHOD(void, processShipment,    (const std::string&), (override));
    MOCK_METHOD(void, completeProduction, (const std::string&), (override));
    MOCK_METHOD(std::vector<Order>, listByStatus, (OrderStatus),  (const, override));
    MOCK_METHOD(std::vector<Order>, listAll,       (),            (const, override));
    MOCK_METHOD(int,                totalOrderCount, (),          (const, override));
};

class MockProductionService : public ProductionService {
    static IProductionRepository& dummyPr() { static InMemoryProductionRepository r; return r; }
    static IOrderRepository&      dummyO()  { static InMemoryOrderRepository      r; return r; }
    static ISampleRepository&     dummyS()  { static InMemorySampleRepository     r; return r; }
public:
    MockProductionService() : ProductionService(dummyPr(), dummyO(), dummyS()) {}

    MOCK_METHOD(std::optional<ProductionJob>, getCurrentJob,       (), (const, override));
    MOCK_METHOD(std::vector<ProductionJob>,   getWaitingJobs,      (), (const, override));
    MOCK_METHOD(bool,                         isIdle,              (), (const, override));
    MOCK_METHOD(int,                          queueSize,           (), (const, override));
    MOCK_METHOD(void,                         completeCurrentJob,  (), (override));
    MOCK_METHOD(void,                         cancelJob, (const std::string&), (override));
};
