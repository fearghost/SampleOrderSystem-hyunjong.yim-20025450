#pragma once
#include <gmock/gmock.h>
#include "../src/repository/ISampleRepository.h"
#include "../src/repository/IOrderRepository.h"
#include "../src/repository/IProductionRepository.h"

class MockSampleRepository : public ISampleRepository {
public:
    MOCK_METHOD(void, save, (const Sample&), (override));
    MOCK_METHOD(std::optional<Sample>, findById, (const std::string&), (override));
    MOCK_METHOD(std::vector<Sample>, findAll, (), (override));
    MOCK_METHOD(std::vector<Sample>, findByName, (const std::string&), (override));
    MOCK_METHOD(bool, existsById, (const std::string&), (override));
    MOCK_METHOD(void, updateStock, (const std::string&, int), (override));
    MOCK_METHOD(int, count, (), (override));
};

class MockOrderRepository : public IOrderRepository {
public:
    MOCK_METHOD(void, save, (const Order&), (override));
    MOCK_METHOD(std::optional<Order>, findById, (const std::string&), (override));
    MOCK_METHOD(std::vector<Order>, findAll, (), (override));
    MOCK_METHOD(std::vector<Order>, findByStatus, (OrderStatus), (override));
    MOCK_METHOD(void, updateStatus, (const std::string&, OrderStatus), (override));
    MOCK_METHOD(int, count, (), (override));
};

class MockProductionRepository : public IProductionRepository {
public:
    MOCK_METHOD(void, enqueue, (const ProductionJob&), (override));
    MOCK_METHOD(std::optional<ProductionJob>, front, (), (override));
    MOCK_METHOD(void, dequeue, (), (override));
    MOCK_METHOD(void, remove, (const std::string&), (override));
    MOCK_METHOD(std::vector<ProductionJob>, waitingJobs, (), (override));
    MOCK_METHOD(bool, isEmpty, (), (override));
    MOCK_METHOD(int, size, (), (override));
};
