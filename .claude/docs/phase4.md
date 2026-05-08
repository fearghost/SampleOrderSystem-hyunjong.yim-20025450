# Phase 4: OrderService TDD

## 목표

주문 접수, 승인(재고 분기), 거절, 출고, 생산 완료 처리를 담당하는 `OrderService`를 구현한다.  
시스템에서 가장 복잡한 상태 전환 로직이며, 3개 Repository 인터페이스를 Mock하여 순수하게 검증한다.

---

## 구현 파일 목록

```
src/service/
├── OrderService.h
└── OrderService.cpp
test/service/
└── OrderServiceTest.cpp
```

---

## 인터페이스 설계

### `src/service/OrderService.h`

```cpp
#pragma once
#include "../repository/ISampleRepository.h"
#include "../repository/IOrderRepository.h"
#include "../repository/IProductionRepository.h"
#include <string>
#include <vector>
#include <cmath>

class OrderService {
public:
    OrderService(ISampleRepository& samples,
                 IOrderRepository&  orders,
                 IProductionRepository& production);

    // 주문 접수 → RESERVED 상태로 저장
    void placeOrder(const std::string& sampleId,
                    const std::string& customerName,
                    int quantity,
                    const std::string& orderId,
                    const std::string& createdAt);

    // 승인: 재고 충분 → CONFIRMED + 재고 차감
    //       재고 부족 → PRODUCING + 생산라인 등록
    void approveOrder(const std::string& orderId);

    // 거절 → REJECTED
    void rejectOrder(const std::string& orderId);

    // 출고: CONFIRMED → RELEASE
    void processShipment(const std::string& orderId);

    // 생산 완료: PRODUCING → CONFIRMED (생산라인에서 dequeue 후 호출)
    void completeProduction(const std::string& orderId);

    [[nodiscard]] std::vector<Order> listByStatus(OrderStatus status) const;
    [[nodiscard]] std::vector<Order> listAll() const;
    [[nodiscard]] int totalOrderCount() const;

private:
    ISampleRepository&     samples_;
    IOrderRepository&      orders_;
    IProductionRepository& production_;

    // 실 생산량 계산: ceil(shortage / (yieldRate * 0.9))
    static int calcActualQty(int shortage, double yieldRate);
    static std::string generateTimestamp();
};
```

---

## 핵심 비즈니스 로직 — `approveOrder` 상세

```
1. orders_.findById(orderId) → Order 조회 (없으면 예외)
2. order.status != RESERVED → 예외
3. samples_.findById(order.sampleId) → Sample 조회
4. if (sample.stock >= order.quantity):
       orders_.updateStatus(orderId, CONFIRMED)
       samples_.updateStock(sampleId, -order.quantity)
   else:
       shortage = order.quantity - sample.stock
       actualQty = ceil(shortage / (sample.yieldRate * 0.9))
       totalTime = sample.avgProductionTime * actualQty
       production_.enqueue(ProductionJob{orderId, sampleId, shortage, actualQty, totalTime})
       orders_.updateStatus(orderId, PRODUCING)
       samples_.updateStock(sampleId, -sample.stock)  // 잔여 재고 전부 소진
```

---

## 생산량 계산 공식

```
부족분(shortage) = 주문 수량 - 현재 재고
실 생산량(actualQty) = ceil(shortage / (yieldRate × 0.9))
총 생산 시간(totalTime) = avgProductionTime × actualQty
```

예시 (SiC 파워기판-6인치, 수율 0.92):
- 주문 200ea, 재고 30ea → shortage = 170
- actualQty = ceil(170 / (0.92 × 0.9)) = ceil(170 / 0.828) = ceil(205.3) = **206ea**
- totalTime = 0.8 × 206 = **164.8 min**

---

## TDD 구현 순서 (Red → Green → Refactor)

### Step 1: `placeOrder` — RESERVED 저장

```
[RED]   placeOrder 호출 → orders_.save() 1회, status == RESERVED 검증
[GREEN] Order 객체 생성 후 orders_.save()
[REFACTOR] 필드 생성 로직 정리
```

### Step 2: `placeOrder` — 존재하지 않는 시료 ID

```
[RED]   samples_.existsById() == false → std::invalid_argument
[GREEN] save 전 시료 존재 확인
```

### Step 3: `approveOrder` — 재고 충분 경로

```
[RED]   stock(500) >= qty(200) → updateStatus(CONFIRMED) + updateStock(-200) 호출
[GREEN] 분기 로직 구현
```

### Step 4: `approveOrder` — 재고 부족 경로

```
[RED]   stock(30) < qty(200) → updateStatus(PRODUCING) + production_.enqueue() 호출
        enqueue 인자: shortage==170, actualQty==206
[GREEN] else 분기 구현 + calcActualQty()
[REFACTOR] calcActualQty static 분리
```

### Step 5: `approveOrder` — RESERVED 아닌 주문에 재승인 시도

```
[RED]   status == CONFIRMED 상태에서 approveOrder → std::logic_error
[GREEN] status 검증 추가
```

### Step 6: `rejectOrder` → REJECTED

```
[RED]   rejectOrder → updateStatus(REJECTED)
[GREEN] 단순 상태 변경
```

### Step 7: `processShipment` — CONFIRMED → RELEASE

```
[RED]   CONFIRMED 상태에서 processShipment → updateStatus(RELEASE)
        RESERVED 상태에서 processShipment → 예외
[GREEN] 상태 검증 + 변경
```

### Step 8: `completeProduction` — PRODUCING → CONFIRMED

```
[RED]   completeProduction → updateStatus(CONFIRMED)
[GREEN] 상태 변경
```

### Step 9: `calcActualQty` 계산 검증

```
[RED]   shortage=170, yieldRate=0.92 → 206
        shortage=150, yieldRate=0.78 → ceil(150/0.702) = 214
        shortage=1,   yieldRate=1.0  → ceil(1/0.9) = 2  (경계값)
[GREEN] static 함수 구현
```

---

## 테스트 케이스 상세

### `test/service/OrderServiceTest.cpp`

```cpp
class OrderServiceTest : public ::testing::Test {
protected:
    MockSampleRepository     mockSamples;
    MockOrderRepository      mockOrders;
    MockProductionRepository mockProduction;
    OrderService service{mockSamples, mockOrders, mockProduction};

    Sample makeSample(int stock, double yieldRate = 0.92) {
        return {"S-001", "SiC", 0.8, yieldRate, stock};
    }
    Order makeOrder(OrderStatus s = OrderStatus::RESERVED, int qty = 200) {
        return {"ORD-001", "S-001", "고객A", qty, s, "2026-05-08"};
    }
};
```

| # | 테스트명 | 시나리오 | 검증 |
|---|---|---|---|
| 1 | `placeOrder_savesReservedOrder` | 정상 입력 | `orders_.save()` 호출, status==RESERVED |
| 2 | `placeOrder_unknownSampleId_throws` | `existsById`→false | `std::invalid_argument` |
| 3 | `approveOrder_stockSufficient_confirmed` | stock=500, qty=200 | `updateStatus(CONFIRMED)` + `updateStock(-200)` |
| 4 | `approveOrder_stockInsufficient_producing` | stock=30, qty=200 | `updateStatus(PRODUCING)` + `enqueue(shortage=170, actualQty=206)` |
| 5 | `approveOrder_stockZero_producing` | stock=0, qty=100 | `updateStatus(PRODUCING)` + `enqueue(shortage=100)` |
| 6 | `approveOrder_notReserved_throws` | status=CONFIRMED | `std::logic_error` |
| 7 | `approveOrder_stockInsufficient_consumesRemainingStock` | stock=30, qty=200 | `updateStock(-30)` (잔여 재고 소진) |
| 8 | `rejectOrder_setsRejected` | status=RESERVED | `updateStatus(REJECTED)` |
| 9 | `processShipment_confirmedToRelease` | status=CONFIRMED | `updateStatus(RELEASE)` |
| 10 | `processShipment_notConfirmed_throws` | status=RESERVED | `std::logic_error` |
| 11 | `completeProduction_producingToConfirmed` | status=PRODUCING | `updateStatus(CONFIRMED)` |
| 12 | `calcActualQty_normalCase` | shortage=170, yield=0.92 | 206 |
| 13 | `calcActualQty_boundaryOne` | shortage=1, yield=1.0 | 2 |
| 14 | `calcActualQty_exactDivision` | shortage=90, yield=1.0 | 100 (ceil(90/0.9)==100) |
| 15 | `listByStatus_delegatesToRepo` | `findByStatus(RESERVED)` → 3건 | 반환 크기 == 3 |

---

## 완료 기준

- [ ] `OrderServiceTest` 전체 15개 케이스 GREEN
- [ ] `calcActualQty` 경계값 포함 3개 케이스 GREEN
- [ ] `OrderService`가 3개 인터페이스에만 의존 (구체 클래스 import 없음)
- [ ] SRP 확인: `OrderService`가 주문 상태 전환 로직만 보유 (콘솔 출력 없음, View 참조 없음)
- [ ] Coverage: `OpenCppCoverage.exe --sources C:*.cpp --export_type=html:coverage -- .\x64\Debug\SampleOrderSystem_Test.exe` → `OrderService.cpp` 분기 커버리지 확인 (재고 충분/부족 양쪽 경로 모두 커버)
- [ ] `git commit`: "Phase 4: Implement OrderService with TDD — state machine and production branching"
