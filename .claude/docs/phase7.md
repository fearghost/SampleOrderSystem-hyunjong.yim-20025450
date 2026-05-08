# Phase 7: JSON 영속성 & 시스템 통합

## 목표

JSON 파일 기반 Repository 구현체를 통합하고,  
`main.cpp`에서 전체 계층을 DI로 조립하여 실행 가능 시스템을 완성한다.  
DummyDataGenerator로 개발·테스트 환경 초기 데이터를 주입한다.

---

## 구현 파일 목록

```
src/repository/
├── JsonSampleRepository.h/cpp
├── JsonOrderRepository.h/cpp
└── JsonProductionRepository.h/cpp

src/view/                    # ConsoleSampleView 등 실제 콘솔 구현체
├── ConsoleSampleView.h/cpp
├── ConsoleOrderView.h/cpp
├── ConsoleMonitorView.h/cpp
├── ConsoleProductionView.h/cpp
└── ConsoleReleaseView.h/cpp

main.cpp

test/repository/             # JSON 파일 기반 통합 테스트
├── JsonSampleRepositoryTest.cpp
├── JsonOrderRepositoryTest.cpp
└── JsonProductionRepositoryTest.cpp
```

---

## JSON Repository 설계

DataPersistence PoC(`src/json/` + nlohmann 또는 자체 JSON 파서)를 그대로 이식한다.

### 저장 파일 경로 (기본값)

```
data/
├── samples.json
├── orders.json
└── production.json
```

### `JsonSampleRepository` 핵심 메서드

```cpp
class JsonSampleRepository : public ISampleRepository {
public:
    explicit JsonSampleRepository(const std::string& filePath = "data/samples.json");

    void        save(const Sample& sample) override;    // upsert (id 기준)
    std::optional<Sample> findById(const std::string& id) override;
    std::vector<Sample>   findAll() override;
    std::vector<Sample>   findByName(const std::string& keyword) override;
    bool        existsById(const std::string& id) override;
    void        updateStock(const std::string& id, int delta) override;
    int         count() override;

private:
    std::string filePath_;
    std::vector<Sample> load() const;  // 파일 → 벡터
    void flush(const std::vector<Sample>& data) const;  // 벡터 → 파일
};
```

### JSON 직렬화 포맷

```json
// samples.json
[
  {
    "id": "S-001",
    "name": "실리콘 웨이퍼-8인치",
    "avgProductionTime": 0.5,
    "yieldRate": 0.92,
    "stock": 480
  }
]

// orders.json
[
  {
    "orderId": "ORD-20260508-0001",
    "sampleId": "S-001",
    "customerName": "삼성전자 파운드리",
    "quantity": 200,
    "status": "RESERVED",
    "createdAt": "2026-05-08T09:32:15"
  }
]

// production.json  (queue 순서 보존)
[
  {
    "orderId": "ORD-20260508-0043",
    "sampleId": "S-003",
    "shortage": 170,
    "actualQty": 206,
    "totalTime": 164.8
  }
]
```

---

## Repository 통합 테스트

> 구체 파일 I/O가 개입하므로 Mock 없이 임시 파일 경로로 실제 동작 검증.

### `JsonSampleRepositoryTest.cpp`

```cpp
class JsonSampleRepositoryTest : public ::testing::Test {
protected:
    std::string testFile = "test_samples_tmp.json";
    JsonSampleRepository repo{testFile};

    void TearDown() override {
        std::remove(testFile.c_str());  // 테스트 후 파일 삭제
    }
};
```

| # | 테스트명 | 검증 |
|---|---|---|
| 1 | `save_persistsAcrossReload` | save 후 새 인스턴스로 findById → 동일 객체 |
| 2 | `updateStock_persistsDelta` | updateStock 후 새 인스턴스로 findById → stock 반영 |
| 3 | `save_upsert_updatesExisting` | 동일 ID로 2회 save → findAll() 1개만 존재 |
| 4 | `findByName_partialMatch` | "GaN" 저장 후 "gaN" 검색 (대소문자 무관) |
| 5 | `emptyFile_findAll_returnsEmpty` | 빈 파일 → findAll() == [] |

### `JsonOrderRepositoryTest.cpp`

| # | 테스트명 | 검증 |
|---|---|---|
| 1 | `save_and_reload` | 저장 후 재로드 → status 포함 동일 |
| 2 | `updateStatus_persistsChange` | RESERVED → CONFIRMED 변경 후 재로드 |
| 3 | `findByStatus_afterReload` | PRODUCING 1건 저장 → 재로드 후 findByStatus(PRODUCING)==1건 |

### `JsonProductionRepositoryTest.cpp`

| # | 테스트명 | 검증 |
|---|---|---|
| 1 | `enqueue_persistsOrder` | A, B 추가 후 재로드 → front()==A (FIFO 보존) |
| 2 | `dequeue_removesFromFile` | dequeue 후 재로드 → size 감소 |

---

## `main.cpp` — DI 조립

```cpp
#include <windows.h>
#include "src/repository/JsonSampleRepository.h"
#include "src/repository/JsonOrderRepository.h"
#include "src/repository/JsonProductionRepository.h"
#include "src/service/SampleService.h"
#include "src/service/OrderService.h"
#include "src/service/ProductionService.h"
#include "src/controller/SampleController.h"
#include "src/controller/OrderController.h"
#include "src/controller/MonitorController.h"
#include "src/controller/ProductionController.h"
#include "src/controller/ReleaseController.h"
#include "src/view/MainView.h"
#include "src/view/ConsoleSampleView.h"
#include "src/view/ConsoleOrderView.h"
#include "src/view/ConsoleMonitorView.h"
#include "src/view/ConsoleProductionView.h"
#include "src/view/ConsoleReleaseView.h"

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    // Repository 계층 (JSON 파일 기반)
    JsonSampleRepository     sampleRepo;
    JsonOrderRepository      orderRepo;
    JsonProductionRepository productionRepo;

    // Service 계층
    SampleService     sampleSvc(sampleRepo);
    OrderService      orderSvc(sampleRepo, orderRepo, productionRepo);
    ProductionService productionSvc(productionRepo, orderRepo, sampleRepo);

    // View 계층 (콘솔 구현체)
    MainView              mainView;
    ConsoleSampleView     sampleView;
    ConsoleOrderView      orderView;
    ConsoleMonitorView    monitorView;
    ConsoleProductionView productionView;
    ConsoleReleaseView    releaseView;

    // Controller 계층
    SampleController     sampleCtrl(sampleSvc, sampleView);
    OrderController      orderCtrl(orderSvc, orderView);
    MonitorController    monitorCtrl(sampleSvc, orderSvc, monitorView);
    ProductionController productionCtrl(productionSvc, productionView);
    ReleaseController    releaseCtrl(orderSvc, releaseView);

    // 메인 루프
    while (true) {
        mainView.showMenu(sampleSvc.totalSampleCount(), orderSvc.totalOrderCount());
        switch (mainView.getMenuChoice()) {
            case 1: sampleCtrl.run();     break;
            case 2: orderCtrl.run();      break;
            case 3: orderCtrl.runApproval(); break;
            case 4: monitorCtrl.run();    break;
            case 5: productionCtrl.run(); break;
            case 6: releaseCtrl.run();    break;
            case 0: return 0;
            default: mainView.showError("잘못된 입력입니다.");
        }
    }
}
```

---

## DummyDataGenerator 연동

DummyDataGenerator PoC의 `DummyDataFacade`를 개발 모드에서 활용한다.

```
// 빌드 타겟 분리 또는 조건부 컴파일
#ifdef DUMMY_MODE
    DummyDataFacade dummy(sampleRepo, orderRepo, productionRepo);
    dummy.generateAll(12, 36);  // 시료 12종, 주문 36건
#endif
```

---

## 완료 기준

- [ ] `JsonSampleRepositoryTest` 전체 GREEN
- [ ] `JsonOrderRepositoryTest` 전체 GREEN
- [ ] `JsonProductionRepositoryTest` 전체 GREEN
- [ ] `data/` 디렉토리 자동 생성 (파일 없을 시 빈 JSON `[]` 초기화)
- [ ] `SampleOrderSystem.exe` 실행: 메인 메뉴 표시 확인
- [ ] 시료 등록 → 재실행 후 데이터 유지 확인 (영속성)
- [ ] 주문 접수 → 승인(재고 충분) → 출고 처리 전체 흐름 콘솔 수동 확인
- [ ] 주문 접수 → 승인(재고 부족) → 생산라인 등록 → 생산 완료 → 출고 흐름 확인
- [ ] SRP 최종 확인: `JsonSampleRepository`가 JSON 직렬화/역직렬화만 담당 (비즈니스 로직 없음)
- [ ] Coverage (전체 누적):
  ```powershell
  OpenCppCoverage.exe --sources C:*.cpp --export_type=html:coverage -- .\x64\Debug\SampleOrderSystem_Test.exe
  ```
  → `coverage\index.html`에서 전체 프로젝트 라인/분기 커버리지 최종 확인
- [ ] `git commit`: "Phase 7: Add JSON repositories and wire main.cpp — system complete"
