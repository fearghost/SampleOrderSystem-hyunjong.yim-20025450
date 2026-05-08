# Phase 5: ProductionService TDD

## 목표

생산라인 FIFO 큐 관리와 생산 완료 처리를 담당하는 `ProductionService`를 구현한다.  
`IProductionRepository`와 `IOrderRepository`를 Mock하여 큐 동작과 완료 흐름을 검증한다.

---

## 구현 파일 목록

```
src/service/
├── ProductionService.h
└── ProductionService.cpp
test/service/
└── ProductionServiceTest.cpp
```

---

## 인터페이스 설계

### `src/service/ProductionService.h`

```cpp
#pragma once
#include "../repository/IProductionRepository.h"
#include "../repository/IOrderRepository.h"
#include "../repository/ISampleRepository.h"
#include <optional>
#include <vector>

class ProductionService {
public:
    ProductionService(IProductionRepository& production,
                      IOrderRepository&      orders,
                      ISampleRepository&     samples);

    // 현재 생산 중인 작업 (큐 front)
    [[nodiscard]] std::optional<ProductionJob> getCurrentJob() const;

    // FIFO 대기 목록 (front 제외)
    [[nodiscard]] std::vector<ProductionJob> getWaitingJobs() const;

    [[nodiscard]] bool isIdle() const;
    [[nodiscard]] int  queueSize() const;

    // 현재 작업 완료 처리:
    //   1. production_.dequeue()
    //   2. samples_.updateStock(sampleId, +actualQty)
    //   3. orders_.updateStatus(orderId, CONFIRMED)
    void completeCurrentJob();

private:
    IProductionRepository& production_;
    IOrderRepository&      orders_;
    ISampleRepository&     samples_;
};
```

---

## TDD 구현 순서 (Red → Green → Refactor)

### Step 1: `isIdle` — 빈 큐

```
[RED]   production_.isEmpty() == true → isIdle() == true
[GREEN] return production_.isEmpty()
```

### Step 2: `getCurrentJob` — 작업 있음

```
[RED]   front() → ProductionJob{...} → getCurrentJob() 동일 job 반환
[GREEN] return production_.front()
```

### Step 3: `getCurrentJob` — 빈 큐

```
[RED]   isEmpty() == true → getCurrentJob() == std::nullopt
[GREEN] isEmpty() 체크 후 nullopt 반환
```

### Step 4: `getWaitingJobs` 위임

```
[RED]   production_.waitingJobs() → 2건 → getWaitingJobs() 2건 반환
[GREEN] return production_.waitingJobs()
```

### Step 5: `completeCurrentJob` — 정상 완료

```
[RED]   job{orderId="ORD-001", sampleId="S-001", actualQty=206} 존재 시:
        dequeue() 1회 호출
        samples_.updateStock("S-001", +206) 호출
        orders_.updateStatus("ORD-001", CONFIRMED) 호출
[GREEN] 3단계 순서대로 호출
[REFACTOR] 호출 순서 명확히 문서화
```

### Step 6: `completeCurrentJob` — 빈 큐에서 호출

```
[RED]   isEmpty() == true → std::logic_error
[GREEN] isEmpty() 사전 검증
```

---

## 테스트 케이스 상세

### `test/service/ProductionServiceTest.cpp`

```cpp
class ProductionServiceTest : public ::testing::Test {
protected:
    MockProductionRepository mockProduction;
    MockOrderRepository      mockOrders;
    MockSampleRepository     mockSamples;
    ProductionService service{mockProduction, mockOrders, mockSamples};

    ProductionJob makeJob(const std::string& orderId = "ORD-001",
                          const std::string& sampleId = "S-001",
                          int actualQty = 206) {
        return {orderId, sampleId, 170, actualQty, 164.8};
    }
};
```

| # | 테스트명 | Mock 설정 | 검증 |
|---|---|---|---|
| 1 | `isIdle_trueWhenEmpty` | `isEmpty()` → true | `isIdle()` == true |
| 2 | `isIdle_falseWhenJobExists` | `isEmpty()` → false | `isIdle()` == false |
| 3 | `getCurrentJob_returnsJob` | `front()` → job | 반환 optional에 값 있음, orderId 일치 |
| 4 | `getCurrentJob_nulloptWhenEmpty` | `isEmpty()` → true | `std::nullopt` |
| 5 | `getWaitingJobs_delegatesToRepo` | `waitingJobs()` → 2건 | 반환 크기 == 2 |
| 6 | `queueSize_delegatesToRepo` | `size()` → 3 | 반환값 == 3 |
| 7 | `completeCurrentJob_callsDequeue` | `front()` → job, `isEmpty()` → false | `dequeue()` 1회 호출 |
| 8 | `completeCurrentJob_addsStock` | `front()` → job(actualQty=206) | `updateStock("S-001", +206)` |
| 9 | `completeCurrentJob_setsConfirmed` | `front()` → job(orderId="ORD-001") | `updateStatus("ORD-001", CONFIRMED)` |
| 10 | `completeCurrentJob_orderOfCalls` | - | dequeue → updateStock → updateStatus 순서 (InOrder) |
| 11 | `completeCurrentJob_emptyQueue_throws` | `isEmpty()` → true | `std::logic_error` |

### InOrder 검증 예시 (테스트 #10)

```cpp
TEST_F(ProductionServiceTest, completeCurrentJob_orderOfCalls) {
    auto job = makeJob();
    EXPECT_CALL(mockProduction, isEmpty()).WillOnce(Return(false));
    EXPECT_CALL(mockProduction, front()).WillOnce(Return(job));

    ::testing::InSequence seq;
    EXPECT_CALL(mockProduction, dequeue());
    EXPECT_CALL(mockSamples, updateStock("S-001", 206));
    EXPECT_CALL(mockOrders, updateStatus("ORD-001", OrderStatus::CONFIRMED));

    service.completeCurrentJob();
}
```

---

## 완료 기준

- [ ] `ProductionServiceTest` 전체 11개 케이스 GREEN
- [ ] `completeCurrentJob` 호출 순서 InOrder 검증 통과
- [ ] `ProductionService`가 3개 인터페이스에만 의존
- [ ] SRP 확인: `ProductionService`가 생산 큐 관리·완료 처리만 보유 (주문 접수·시료 등록 로직 없음)
- [ ] Coverage: `OpenCppCoverage.exe --sources C:*.cpp --export_type=html:coverage -- .\x64\Debug\SampleOrderSystem_Test.exe` → `ProductionService.cpp` 커버리지 확인 (빈 큐 예외 경로 포함)
- [ ] `git commit`: "Phase 5: Implement ProductionService with TDD — FIFO queue and completion flow"
