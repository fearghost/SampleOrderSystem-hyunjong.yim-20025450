# PRD - 반도체 시료 생산주문관리 시스템

## TDD 구현 Phase 문서

| Phase | 문서 | 내용 |
|---|---|---|
| 1 | [phase1.md](phase1.md) | 프로젝트 구조 & 도메인 엔티티 (`Sample`, `Order`, `ProductionJob`) |
| 2 | [phase2.md](phase2.md) | Repository 인터페이스 & 인메모리 구현 & Mock 클래스 |
| 3 | [phase3.md](phase3.md) | `SampleService` TDD (등록, 검색, 재고 관리) |
| 4 | [phase4.md](phase4.md) | `OrderService` TDD (상태 전환, 재고 분기, 생산량 계산) |
| 5 | [phase5.md](phase5.md) | `ProductionService` TDD (FIFO 큐, 생산 완료) |
| 6 | [phase6.md](phase6.md) | View & Controller TDD (가상 메서드, 입력 라우팅) |
| 7 | [phase7.md](phase7.md) | JSON 영속성 & `main.cpp` 시스템 통합 |

---

## 배경

가상의 반도체 회사 "S-Semi"는 연구소, 팹리스(Fabless) 업체, 대학 연구실 등에 반도체 시료(Sample)를 납품한다. 주문량 급증으로 엑셀/메모장 기반 관리의 한계(주문 처리 누락, 재고·공정 현황 불투명)가 드러났고, 이를 해소하기 위해 콘솔 기반 생산주문관리 시스템을 개발한다.

## PoC 참고 레포지토리

본 프로젝트는 아래 4개의 PoC(Proof of Concept) 레포지토리를 기반으로 설계되었다. 각 PoC에서 검증한 패턴과 구조를 본 시스템에 통합·적용한다.

| PoC 항목 | 레포지토리 |
|---|---|
| MVC 스켈레톤 코드 | https://github.com/fearghost/ConsoleMVC-hyunjong.yim-20025450 |
| 데이터 영속성 처리 | https://github.com/fearghost/DataPersistence-hyunjong.yim-20025450 |
| 데이터 모니터링 Tool | https://github.com/fearghost/DataMonitor-hyunjong.yim-20025450 |
| Dummy 데이터 생성 Tool | https://github.com/fearghost/DummyDataGenerator-hyunjong.yim-20025450 |

- **ConsoleMVC**: `controller/` / `model/` / `view/` 계층 분리 구조와 Google Mock 기반 TDD 패턴 적용
- **DataPersistence**: 애플리케이션 재시작 후에도 데이터가 유지되는 영속성 레이어(파일/JSON/DB 선택) 및 CRUD 구조
- **DataMonitor**: 콘솔에서 저장 데이터를 실시간 조회하는 관리자 모니터링 도구
- **DummyDataGenerator**: 테스트용 더미 데이터를 생성하여 연결된 저장소에 주입하는 도구

---

## 시스템 개요

- **형태**: C++20 콘솔 애플리케이션
- **아키텍처**: MVC (Model / Controller / View) 계층 분리
- **테스트**: TDD 기반, Google Test / Google Mock (gmock 1.11.0) 사용
- **빌드**: MSBuild / Visual Studio 2022

### 역할 구조

```
고객 → (시료 요청) → 주문 담당자 → (주문서 전달) → 생산 담당자
                          ↑                              |
                          └──────── (승인 / 거절) ────────┘
```

- **주문 담당자**: 고객 요청을 주문서로 작성, 주문 목록 관리
- **생산 담당자**: 시료 등록, 주문 수신 후 승인 또는 거절

---

## 주문 상태 흐름

```
RESERVED ──→ (승인, 재고 충분) ──→ CONFIRMED ──→ RELEASE
     │
     ├──→ (승인, 재고 부족) ──→ PRODUCING ──→ CONFIRMED ──→ RELEASE
     │
     └──→ (거절) ──→ REJECTED
```

| 상태 | 의미 |
|---|---|
| `RESERVED` | 주문 접수 |
| `REJECTED` | 주문 거절 (모니터링 제외) |
| `PRODUCING` | 승인 완료, 재고 부족으로 생산 중 |
| `CONFIRMED` | 승인 완료, 출고 대기 중 |
| `RELEASE` | 출고 완료 |

---

## 기능 명세

### 1. 메인 메뉴

시작 시 전체 시료 요약 정보(등록 시료 수, 총 재고, 전체 주문 수, 생산라인 대기 수)를 표시하고 아래 6개 메뉴를 제공한다.

| 번호 | 메뉴 | 설명 |
|---|---|---|
| 1 | 시료 관리 | 시료 등록 / 목록 조회 / 이름 검색 |
| 2 | 시료 주문 | 고객 주문 접수 (RESERVED 생성) |
| 3 | 주문 승인/거절 | RESERVED 목록 조회 후 승인 또는 거절 |
| 4 | 모니터링 | 상태별 주문 수 및 시료별 재고 현황 |
| 5 | 생산라인 조회 | 현재 생산 중인 시료 및 대기 큐 확인 |
| 6 | 출고 처리 | CONFIRMED 주문에 대해 출고 실행 |
| 0 | 종료 | 애플리케이션 종료 |

---

### 2. 시료 관리 (Sample Management)

시료(Sample)는 시스템의 기본 단위로, 등록된 시료만 주문 가능하다.

#### 2-1. 시료 등록

입력 값:

| 필드 | 타입 | 설명 |
|---|---|---|
| 시료 ID | string | 고유 식별자 (예: S-001) |
| 이름 | string | 시료명 (예: 실리콘 웨이퍼-8인치) |
| 평균 생산시간 | float | 개당 생산 소요 시간 (min/ea) |
| 수율 | float | 정상 시료 / 총 생산 시료 (0 < yield ≤ 1.0) |

#### 2-2. 시료 목록 조회

등록된 모든 시료를 ID / 이름 / 평균 생산시간 / 수율 / 현재 재고 컬럼으로 페이지네이션하여 표시한다.

#### 2-3. 시료 검색

이름 등 속성 키워드로 시료를 검색하여 결과를 목록으로 표시한다.

---

### 3. 시료 주문 (Order Reservation)

주문 담당자가 고객 요청을 시스템에 입력한다.

입력 값:

| 필드 | 타입 |
|---|---|
| 시료 ID | string |
| 고객명 | string |
| 주문 수량 | int |

- 입력 확인 후 [Y] 예약 접수 / [N] 취소를 선택한다.
- 접수 완료 시 주문번호(예: `ORD-YYYYMMDD-NNNN`) 및 상태 `RESERVED`를 출력한다.
- 재고 확인은 이 단계에서 하지 않는다. 재고 처리는 승인 단계에서 이루어진다.

---

### 4. 주문 승인/거절 (Order Approval)

`RESERVED` 상태의 주문 목록을 표시하고, 생산 담당자가 개별 주문을 처리한다.

#### 4-1. 주문 승인

승인 시 재고 상황에 따라 자동 분기:

- **재고 충분** (현재 재고 ≥ 주문 수량): 즉시 `CONFIRMED`로 전환, 재고에서 주문 수량을 차감한다.
- **재고 부족** (현재 재고 < 주문 수량): 생산라인에 자동 등록, 상태를 `PRODUCING`으로 전환한다.

생산 등록 시 계산:

```
부족분 = 주문 수량 - 현재 재고
실 생산량 = ceil(부족분 / (수율 × 0.9))
총 생산 시간 = 평균 생산시간 × 실 생산량
```

#### 4-2. 주문 거절

즉시 `REJECTED`로 전환한다.

---

### 5. 모니터링 (Monitoring)

담당자가 시스템 전체 상태를 한눈에 파악할 수 있도록 두 가지 뷰를 제공한다.

#### 5-1. 주문량 확인

`RESERVED` / `CONFIRMED` / `PRODUCING` / `RELEASE` 상태별 주문 건수를 표시한다. `REJECTED`는 제외한다.

#### 5-2. 재고량 확인

각 시료별 현재 재고 수량과 재고 상태를 표시한다.

| 상태 | 조건 |
|---|---|
| 여유 | 주문 대비 재고 충분 |
| 부족 | 주문 대비 재고 수량 부족 (재고 > 0) |
| 고갈 | 재고 수량 = 0 |

---

### 6. 생산라인 (Production Line)

단일 생산라인이며, 주문이 들어온 시료에 대해서만 생산한다.

#### 6-1. 생산 현황 표기

현재 생산 중인 주문 정보(주문번호, 시료명, 주문량, 재고, 부족분, 실 생산량, 수율, 총 생산 시간, 진행률)를 표시한다.

#### 6-2. 대기 주문 확인

생산 큐(FIFO)에 대기 중인 주문 목록과 순서별 예상 완료 시간을 표시한다.

#### 6-3. 생산 완료 처리

생산 완료 시 주문 상태를 `PRODUCING` → `CONFIRMED`로 변경하고, 재고에 생산된 수량을 반영한다.

---

### 7. 출고 처리 (Release)

`CONFIRMED` 상태의 주문 목록을 표시하고, 선택한 주문을 출고 처리한다.

- 출고 실행 시 주문 상태가 `CONFIRMED` → `RELEASE`로 전환된다.
- 출고 결과(주문번호, 출고 수량, 처리 일시, 상태 변경)를 출력한다.

---

## 데이터 모델

ConsoleMVC PoC의 실제 엔티티 구조를 기반으로 한다.

### Sample (시료)

```cpp
struct Sample {
    std::string id;               // 고유 ID (S-NNN)
    std::string name;             // 시료명
    double      avgProductionTime;// 평균 생산시간 (min/ea) — PDF 예시 0.5/0.3 등 소수점 포함
    double      yieldRate;        // 수율 (0 < yieldRate ≤ 1.0)
    int         stock;            // 현재 재고 수량
};
```

### Order (주문)

```cpp
enum class OrderStatus { RESERVED, PRODUCING, CONFIRMED, RELEASE, REJECTED };

struct Order {
    std::string orderId;      // 주문번호 (ORD-YYYYMMDD-NNNN)
    std::string sampleId;     // 시료 ID → Sample
    std::string customerName; // 고객명
    int         quantity;     // 주문 수량
    OrderStatus status;
    std::string createdAt;    // 생성 일시 (문자열, ISO 8601)
};
```

### ProductionJob (생산 작업)

```cpp
struct ProductionJob {
    std::string orderId;    // 연결 주문 ID
    std::string sampleId;   // 시료 ID
    int         shortage;   // 부족분 = quantity - stock_at_approval
    int         actualQty;  // 실 생산량 = ceil(shortage / (yieldRate × 0.9))
    double      totalTime;  // 총 생산 시간 = avgProductionTime × actualQty
};
```

`ProductionLine`은 `std::queue<ProductionJob>`으로 FIFO 관리한다 (ConsoleMVC PoC 검증).

---

## 아키텍처 가이드

### Review 결과: 두 PoC의 역할 분담

| PoC | 검증한 패턴 | 본 시스템 적용 |
|---|---|---|
| ConsoleMVC | Controller/View 명칭·분리, View 가상화(MockViews), ProductionLine(queue), DI | Controller·View 계층 네이밍과 구조 그대로 채택 |
| DataPersistence | `I*Repository` 순수 가상 인터페이스, Service 레이어, JSON 영속성 | Repository 인터페이스 + Service 계층 도입 |
| DataMonitor | Service를 통한 실시간 조회 | MonitorController → Service 의존 구조 |
| DummyDataGenerator | `DummyDataFacade` + generator/, Repository 주입 | 개발·테스트 환경 초기 데이터 주입 도구로 활용 |

### 4계층 디렉토리 구조

ConsoleMVC PoC의 단일 `model/`은 엔티티와 레포지토리가 혼재하고 인터페이스가 없어 Repository Mock이 불가능하다. DataPersistence PoC에서 검증된 인터페이스 + 서비스 레이어를 추가하여 아래 4계층으로 분리한다.

```
src/
├── model/                      # 도메인 엔티티 헤더만 (외부 의존 없음)
│   ├── Sample.h
│   ├── Order.h                 # enum class OrderStatus 포함
│   └── ProductionJob.h
│
├── repository/                 # DataPersistence PoC 패턴
│   ├── ISampleRepository.h     # 순수 가상 인터페이스 (Mock 진입점)
│   ├── IOrderRepository.h
│   ├── IProductionRepository.h
│   ├── SampleRepository.h/cpp  # JSON 파일 기반 구현체
│   ├── OrderRepository.h/cpp
│   └── ProductionRepository.h/cpp
│
├── service/                    # DataPersistence PoC 패턴 — 비즈니스 로직
│   ├── SampleService.h/cpp     # 시료 등록·검색·재고 관리
│   ├── OrderService.h/cpp      # 주문 접수·승인·거절·출고·생산완료
│   └── ProductionService.h/cpp # FIFO 큐 관리, 생산 완료 처리
│
├── controller/                 # ConsoleMVC PoC 패턴 — 입력 라우팅만 담당
│   ├── SampleController.h/cpp
│   ├── OrderController.h/cpp
│   ├── MonitorController.h/cpp
│   ├── ProductionController.h/cpp
│   └── ReleaseController.h/cpp
│
├── view/                       # ConsoleMVC PoC 패턴 — 가상 메서드로 Mock 가능
│   ├── MainView.h/cpp
│   ├── SampleView.h/cpp
│   ├── OrderView.h/cpp
│   ├── MonitorView.h/cpp
│   ├── ProductionView.h/cpp
│   └── ReleaseView.h/cpp
│
└── main.cpp                    # 구체 타입 생성 및 DI 조립

test/
├── MockRepositories.h          # MOCK_METHOD on I*Repository (DataPersistence 패턴)
├── MockViews.h                 # MOCK_METHOD on View virtual methods (ConsoleMVC 패턴)
├── service/
│   ├── OrderServiceTest.cpp    # Mock I*Repository → OrderService 로직 검증
│   ├── SampleServiceTest.cpp
│   └── ProductionServiceTest.cpp
├── controller/
│   ├── OrderControllerTest.cpp # Mock Service + MockView → 라우팅 검증
│   ├── SampleControllerTest.cpp
│   └── ...
└── repository/
    ├── SampleRepositoryTest.cpp # 임시 JSON 파일로 영속성 통합 테스트
    └── ...
```

### 의존성 방향 (단방향 준수)

```
main.cpp
  └─ 생성: JsonSampleRepository, JsonOrderRepository, JsonProductionRepository
  └─ 생성: SampleService(ISampleRepo&)
           OrderService(ISampleRepo&, IOrderRepo&, IProductionRepo&)
           ProductionService(IProductionRepo&, IOrderRepo&)
  └─ 생성: SampleController(SampleService&, SampleView&)
           OrderController(OrderService&, OrderView&)
           MonitorController(SampleService&, OrderService&, MonitorView&)
           ProductionController(ProductionService&, ProductionView&)
           ReleaseController(OrderService&, ReleaseView&)

의존 방향: controller → service → repository(interface) ← repository(impl)
           controller → view(interface) ← view(impl)
```

### TDD Mock 전략

| 테스트 대상 | Mock 대상 | 검증 내용 |
|---|---|---|
| `OrderService` | `MockISampleRepo`, `MockIOrderRepo`, `MockIProductionRepo` | 재고 분기 로직, 상태 전환 |
| `SampleService` | `MockISampleRepo` | 중복 ID 거부, 재고 차감 |
| `ProductionService` | `MockIProductionRepo`, `MockIOrderRepo` | FIFO 순서, 완료 처리 |
| `*Controller` | Mock Service + `MockView` | 입력→서비스 호출 라우팅 |
| `*Repository` | 없음 (임시 JSON 파일) | 영속성 CRUD 통합 테스트 |

### 핵심 TDD 테스트 케이스

- 주문 승인 시 재고 충분(stock ≥ qty) → `CONFIRMED` 전환, `updateStock` 호출 검증
- 주문 승인 시 재고 부족(stock < qty) → `PRODUCING` 전환, `IProductionRepository::enqueue` 호출 검증
- 실 생산량 공식: `ceil(shortage / (yieldRate × 0.9))` — 경계값 포함 검증
- 생산 완료 시 `PRODUCING` → `CONFIRMED` 전환 및 재고 반영
- 출고 처리 시 `CONFIRMED` → `RELEASE` 전환
- 재고 상태 판정: 여유(stock ≥ pending_qty) / 부족(0 < stock < pending_qty) / 고갈(stock == 0)
- FIFO 생산 큐 순서 보장: 먼저 등록된 Job이 먼저 처리됨

### C++20 적용 지침

- `enum class OrderStatus` — 타입 안전 상태 (ConsoleMVC PoC 확인됨)
- `std::queue<ProductionJob>` — FIFO 생산 큐 (ConsoleMVC PoC 확인됨)
- `[[nodiscard]]`, `const` 정확성, `noexcept` — 순수 조회 메서드에 적용
- `std::optional<Sample>` — 검색 결과 없음을 null 대신 표현
- 스마트 포인터: 소유권이 명확한 경우 `std::unique_ptr`, 공유는 `std::shared_ptr`
