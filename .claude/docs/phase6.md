# Phase 6: View & Controller TDD

## 목표

콘솔 입출력을 담당하는 View 클래스(가상 메서드)와,  
입력 라우팅을 담당하는 Controller 클래스를 구현하고 TDD로 검증한다.  
Controller 테스트는 `MockView` + `MockService`(또는 직접 Mock Service)로 입력→서비스 호출 경로를 검증한다.

---

## 구현 파일 목록

```
src/view/                        # 추상 헤더 (순수 가상) + Console 구현체 분리
├── MainView.h                   # 추상 인터페이스
├── SampleView.h / OrderView.h / MonitorView.h
├── ProductionView.h / ReleaseView.h
├── ConsoleMainView.h/cpp        # 실제 콘솔 구현체 (Phase 7에서 추가)
├── ConsoleSampleView.h/cpp
├── ConsoleOrderView.h/cpp
├── ConsoleMonitorView.h/cpp
├── ConsoleProductionView.h/cpp
└── ConsoleReleaseView.h/cpp

src/controller/
├── SampleController.h/cpp
├── OrderController.h/cpp        # run(): 주문접수 / runApproval(): 승인·거절
├── MonitorController.h/cpp
├── ProductionController.h/cpp
└── ReleaseController.h/cpp

test/
├── MockViews.h
├── MockServices.h
└── controller/
    ├── SampleControllerTest.cpp
    ├── OrderControllerTest.cpp
    ├── MonitorControllerTest.cpp
    ├── ProductionControllerTest.cpp
    └── ReleaseControllerTest.cpp
```

---

## View 설계 원칙

ConsoleMVC PoC의 패턴을 따른다.

- 모든 입력/출력 메서드를 `virtual`로 선언 → Google Mock 가능
- View는 데이터 수신·표시만 담당, 비즈니스 판단 없음
- 입력 구조체(`*Input`)를 통해 여러 필드를 묶어 반환

### 공통 입력 구조체 (헤더)

```cpp
// SampleView.h
struct SampleInput {
    std::string id;
    std::string name;
    double avgProductionTime;
    double yieldRate;
    int initialStock;
};

// OrderView.h
struct OrderInput {
    std::string sampleId;
    std::string customerName;
    int quantity;
};
```

### 예시: `SampleView.h`

```cpp
#pragma once
#include "../model/Sample.h"
#include <vector>
#include <string>

struct SampleInput { ... };

class SampleView {
public:
    virtual ~SampleView() = default;
    virtual void showSubMenu() = 0;
    virtual int  getMenuChoice() = 0;
    virtual SampleInput getRegisterInput() = 0;
    virtual std::string getSearchKeyword() = 0;
    virtual void showSampleList(const std::vector<Sample>&, const std::string& header) = 0;
    virtual void showSuccess(const std::string& msg) = 0;
    virtual void showError(const std::string& msg) = 0;
};
```

---

## Controller 설계 원칙

ConsoleMVC PoC의 패턴을 따른다.

- Controller는 View 입력 → Service 호출 → View 출력의 흐름만 담당
- Service 의존 (인터페이스 또는 구체 클래스 주입)
- `run()` 메서드: 서브메뉴 루프 또는 단일 액션
- **ID 생성 금지**: `OrderController::run()`은 `placeOrder(sampleId, customerName, qty)` 3인자만 전달. `orderId`·`createdAt`은 `OrderService` 내부에서 생성하며 Controller는 관여하지 않는다.

### 예시: `SampleController.h`

```cpp
#pragma once
#include "../service/SampleService.h"
#include "../view/SampleView.h"

class SampleController {
public:
    SampleController(SampleService& service, SampleView& view);
    void run();

private:
    SampleService& service_;
    SampleView&    view_;

    void handleRegister();
    void handleList();
    void handleSearch();
};
```

---

## Mock 클래스

### `test/MockViews.h`

```cpp
#pragma once
#include <gmock/gmock.h>
#include "../src/view/SampleView.h"
#include "../src/view/OrderView.h"
#include "../src/view/MonitorView.h"
#include "../src/view/ProductionView.h"
#include "../src/view/ReleaseView.h"

class MockSampleView : public SampleView {
public:
    MOCK_METHOD(void, showSubMenu, (), (override));
    MOCK_METHOD(int,  getMenuChoice, (), (override));
    MOCK_METHOD(SampleInput, getRegisterInput, (), (override));
    MOCK_METHOD(std::string, getSearchKeyword, (), (override));
    MOCK_METHOD(void, showSampleList, (const std::vector<Sample>&, const std::string&), (override));
    MOCK_METHOD(void, showSuccess, (const std::string&), (override));
    MOCK_METHOD(void, showError, (const std::string&), (override));
};

class MockOrderView : public OrderView {
public:
    MOCK_METHOD(void, showSubMenu, (), (override));
    MOCK_METHOD(int,  getMenuChoice, (), (override));
    MOCK_METHOD(OrderInput, getOrderInput, (), (override));
    MOCK_METHOD(std::string, getOrderId, (const std::string&), (override));
    MOCK_METHOD(void, showOrderList, (const std::vector<Order>&, const std::string&), (override));
    MOCK_METHOD(void, showSuccess, (const std::string&), (override));
    MOCK_METHOD(void, showError, (const std::string&), (override));
};

// MockMonitorView, MockProductionView, MockReleaseView 동일 패턴
```

### `test/MockServices.h` — MockOrderService

```cpp
class MockOrderService : public OrderService {
    // ... 더미 레포지토리로 베이스 생성자 호출 ...
public:
    // placeOrder는 3인자 (orderId·createdAt 파라미터 없음)
    MOCK_METHOD(void, placeOrder,
        (const std::string&, const std::string&, int), (override));
    MOCK_METHOD(void, approveOrder,    (const std::string&), (override));
    MOCK_METHOD(void, rejectOrder,     (const std::string&), (override));
    MOCK_METHOD(void, processShipment, (const std::string&), (override));
    // ...
};
```

---

## TDD 테스트 케이스

### `SampleControllerTest.cpp`

| # | 테스트명 | 시나리오 | 검증 |
|---|---|---|---|
| 1 | `run_choice1_callsRegister` | getMenuChoice → 1 → 0 | `getRegisterInput()` 1회, `service.registerSample()` 1회 |
| 2 | `run_choice2_callsList` | getMenuChoice → 2 → 0 | `service.listAll()` 1회, `showSampleList()` 1회 |
| 3 | `run_choice3_callsSearch` | getMenuChoice → 3 → 0 | `getSearchKeyword()` 1회, `service.searchByName()` 1회 |
| 4 | `register_duplicateId_showsError` | `service.registerSample()` throws | `showError()` 1회 호출 |
| 5 | `run_choice0_exits` | getMenuChoice → 0 | `run()` 반환, loop 종료 |

### `OrderControllerTest.cpp`

| # | 테스트명 | 시나리오 | 검증 |
|---|---|---|---|
| 1 | `placeOrder_callsService` | `run()` 호출 | `service.placeOrder("S-001", "고객A", 200)` 3인자로 호출 (orderId·createdAt 없음) |
| 2 | `approveOrder_callsService` | `runApproval()` → 메뉴 1 | `service.approveOrder(orderId)` 1회 |
| 3 | `rejectOrder_callsService` | `runApproval()` → 메뉴 2 | `service.rejectOrder(orderId)` 1회 |
| 4 | `approveOrder_serviceThrows_showsError` | service throws | `showError()` 1회 |

### `MonitorControllerTest.cpp`

| # | 테스트명 | 검증 |
|---|---|---|
| 1 | `run_callsListAll_onSampleService` | `sampleService.listAll()` 1회 |
| 2 | `run_callsListAll_onOrderService` | `orderService.listAll()` 1회 |
| 3 | `run_passesDataToView` | `monitorView.showMonitor()` 1회, 인자 정확성 |

### `ProductionControllerTest.cpp`

| # | 테스트명 | 검증 |
|---|---|---|
| 1 | `run_showsCurrentAndWaiting` | `getCurrentJob()` + `getWaitingJobs()` 호출 |
| 2 | `completeJob_callsService` | 완료 메뉴 선택 → `service.completeCurrentJob()` |

### `ReleaseControllerTest.cpp`

| # | 테스트명 | 검증 |
|---|---|---|
| 1 | `run_showsConfirmedOrders` | `service.listByStatus(CONFIRMED)` → view 전달 |
| 2 | `release_callsProcessShipment` | 주문 선택 → `service.processShipment(orderId)` |
| 3 | `release_serviceThrows_showsError` | service throws → `showError()` |

---

## 완료 기준

- [ ] 모든 View 클래스 컴파일 (순수 가상 메서드 포함)
- [ ] `MockViews.h` 컴파일 오류 없음
- [ ] 5개 Controller 테스트 파일 전체 GREEN (총 ~20개 케이스)
- [ ] Controller가 Service와 View에만 의존 (Repository 직접 참조 없음)
- [ ] SRP 확인:
  - View: 콘솔 입출력만 (비즈니스 판단 없음)
  - Controller: 입력→서비스 라우팅만 (계산·상태 전환 로직 없음)
- [ ] Coverage: `OpenCppCoverage.exe --sources C:*.cpp --export_type=html:coverage -- .\x64\Debug\SampleOrderSystem_Test.exe` → 각 Controller의 메뉴 분기 경로 커버리지 확인
- [ ] `git commit`: "Phase 6: Implement Views and Controllers with TDD"
