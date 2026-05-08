# Phase 2: Repository 인터페이스 & 인메모리 구현

## 목표

Service 레이어가 의존할 순수 가상 인터페이스(`I*Repository`)를 정의하고,  
테스트에서 사용할 인메모리 구현체와 Google Mock 기반 Mock 클래스를 준비한다.  
이 단계에서 Repository 구현의 정확성을 단위 테스트로 검증한다.

---

## 구현 파일 목록

```
src/repository/
├── ISampleRepository.h
├── IOrderRepository.h
├── IProductionRepository.h
├── InMemorySampleRepository.h/cpp
├── InMemoryOrderRepository.h/cpp
└── InMemoryProductionRepository.h/cpp
test/
├── MockRepositories.h
├── repository/
│   ├── InMemorySampleRepositoryTest.cpp
│   ├── InMemoryOrderRepositoryTest.cpp
│   └── InMemoryProductionRepositoryTest.cpp
```

---

## 인터페이스 정의

### `src/repository/ISampleRepository.h`

```cpp
#pragma once
#include "../model/Sample.h"
#include <vector>
#include <string>
#include <optional>

class ISampleRepository {
public:
    virtual ~ISampleRepository() = default;
    virtual void        save(const Sample& sample) = 0;
    virtual std::optional<Sample> findById(const std::string& id) = 0;
    virtual std::vector<Sample>   findAll() = 0;
    virtual std::vector<Sample>   findByName(const std::string& keyword) = 0;
    virtual bool        existsById(const std::string& id) = 0;
    virtual void        updateStock(const std::string& id, int delta) = 0;
    virtual int         count() = 0;
};
```

### `src/repository/IOrderRepository.h`

```cpp
#pragma once
#include "../model/Order.h"
#include <vector>
#include <string>
#include <optional>

class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;
    virtual void        save(const Order& order) = 0;
    virtual std::optional<Order> findById(const std::string& orderId) = 0;
    virtual std::vector<Order>   findAll() = 0;
    virtual std::vector<Order>   findByStatus(OrderStatus status) = 0;
    virtual void        updateStatus(const std::string& orderId, OrderStatus status) = 0;
    virtual int         count() = 0;
};
```

### `src/repository/IProductionRepository.h`

```cpp
#pragma once
#include "../model/ProductionJob.h"
#include <vector>
#include <optional>

class IProductionRepository {
public:
    virtual ~IProductionRepository() = default;
    virtual void enqueue(const ProductionJob& job) = 0;
    virtual std::optional<ProductionJob> front() = 0;  // 현재 처리 중인 작업
    virtual void dequeue() = 0;                          // 완료 후 제거
    virtual std::vector<ProductionJob> waitingJobs() = 0;
    virtual bool isEmpty() = 0;
    virtual int  size() = 0;
};
```

---

## Mock 클래스

### `test/MockRepositories.h`

```cpp
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
    MOCK_METHOD(std::vector<ProductionJob>, waitingJobs, (), (override));
    MOCK_METHOD(bool, isEmpty, (), (override));
    MOCK_METHOD(int, size, (), (override));
};
```

---

## TDD 테스트 케이스

### `InMemorySampleRepositoryTest.cpp`

| # | 테스트명 | 검증 내용 |
|---|---|---|
| 1 | `save_and_findById` | 저장 후 동일 ID로 조회 → 동일 객체 반환 |
| 2 | `findById_not_found` | 존재하지 않는 ID → `std::nullopt` 반환 |
| 3 | `existsById_true` | 저장된 ID → `true` |
| 4 | `existsById_false` | 미등록 ID → `false` |
| 5 | `findAll_returns_all` | 3개 저장 → `findAll()` 3개 반환 |
| 6 | `findByName_partial_match` | "웨이퍼" 검색 → 이름에 포함된 시료만 반환 |
| 7 | `updateStock_positive` | stock 100, delta +50 → stock 150 |
| 8 | `updateStock_negative` | stock 100, delta -30 → stock 70 |
| 9 | `count_reflects_saves` | 3번 저장 → count() == 3 |

### `InMemoryOrderRepositoryTest.cpp`

| # | 테스트명 | 검증 내용 |
|---|---|---|
| 1 | `save_and_findById` | 저장 후 조회 |
| 2 | `findByStatus_reserved` | RESERVED 2건 저장 → findByStatus(RESERVED) == 2건 |
| 3 | `updateStatus_changes_state` | RESERVED → CONFIRMED 변경 후 재조회 |
| 4 | `findByStatus_excludes_other` | REJECTED 저장 후 RESERVED 조회 → 0건 |

### `InMemoryProductionRepositoryTest.cpp`

| # | 테스트명 | 검증 내용 |
|---|---|---|
| 1 | `enqueue_and_front` | 1건 추가 → front() 동일 작업 반환 |
| 2 | `fifo_order` | A, B, C 순서로 추가 → front()는 A |
| 3 | `dequeue_advances_front` | A 완료(dequeue) → front()는 B |
| 4 | `isEmpty_true_when_empty` | 초기 상태 → isEmpty() true |
| 5 | `isEmpty_false_after_enqueue` | 1건 추가 → isEmpty() false |
| 6 | `waitingJobs_excludes_front` | 3건 추가 → waitingJobs() 2건 (front 제외) |

---

## 완료 기준

- [ ] 인터페이스 3개 컴파일 오류 없음
- [ ] `MockRepositories.h` 컴파일 오류 없음
- [ ] `InMemory*RepositoryTest` 전체 GREEN
- [ ] Coverage: `OpenCppCoverage.exe --sources C:*.cpp --export_type=html:coverage -- .\x64\Debug\SampleOrderSystem_Test.exe` → `InMemory*Repository` 구현 코드 커버리지 확인
- [ ] `git commit`: "Phase 2: Add repository interfaces, in-memory impls, and mocks"
