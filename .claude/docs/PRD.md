# PRD - 반도체 시료 생산주문관리 시스템

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

### Sample (시료)

| 필드 | 타입 | 설명 |
|---|---|---|
| id | string | 고유 ID (S-NNN) |
| name | string | 시료명 |
| avgProductionTime | float | 평균 생산시간 (min/ea) |
| yield | float | 수율 (0 < yield ≤ 1.0) |
| stock | int | 현재 재고 수량 |

### Order (주문)

| 필드 | 타입 | 설명 |
|---|---|---|
| orderId | string | 주문번호 (ORD-YYYYMMDD-NNNN) |
| sampleId | string | 시료 ID (FK → Sample) |
| customerName | string | 고객명 |
| quantity | int | 주문 수량 |
| status | enum | RESERVED / REJECTED / PRODUCING / CONFIRMED / RELEASE |
| createdAt | datetime | 주문 생성 일시 |

### ProductionJob (생산 작업)

| 필드 | 타입 | 설명 |
|---|---|---|
| orderId | string | 연결 주문 ID |
| sampleId | string | 시료 ID |
| shortage | int | 부족분 |
| actualQty | int | 실 생산량 = ceil(shortage / (yield × 0.9)) |
| totalTime | float | 총 생산 시간 = avgProductionTime × actualQty |
| status | enum | WAITING / RUNNING / DONE |

---

## TDD 구현 지침

- 각 기능은 테스트를 먼저 작성하고 구현한다 (Red → Green → Refactor).
- 테스트는 `test/` 디렉토리에 기능 단위로 분리한다.
- Mock 대상: 저장소(Repository) 레이어, 생산라인 스케줄러.
- 핵심 테스트 케이스:
  - 주문 승인 시 재고 충분 → `CONFIRMED` 전환 검증
  - 주문 승인 시 재고 부족 → `PRODUCING` 전환 및 생산라인 큐 등록 검증
  - 실 생산량 공식: `ceil(shortage / (yield × 0.9))` 계산 검증
  - 생산 완료 시 `PRODUCING` → `CONFIRMED` 전환 검증
  - 출고 처리 시 `CONFIRMED` → `RELEASE` 전환 검증
  - 재고 상태(여유/부족/고갈) 판정 로직 검증
  - FIFO 생산 큐 순서 보장 검증

---

## 아키텍처 가이드

PoC(ConsoleMVC)에서 검증한 `controller/` / `model/` / `view/` 3계층 구조를 그대로 적용한다.

```
src/
├── model/          # 도메인 엔티티, Repository 인터페이스
├── controller/     # 비즈니스 로직, 상태 전환 규칙
├── view/           # 콘솔 입출력, 메뉴 렌더링
└── main.cpp
test/               # Google Test / Mock 기반 단위 테스트
```

- **영속성**: DataPersistence PoC에서 선택한 저장 방식(파일/JSON/DB)을 Repository 구현체로 연결한다.
- **모니터링**: DataMonitor PoC의 실시간 조회 패턴을 모니터링 메뉴에 적용한다.
- **더미 데이터**: DummyDataGenerator PoC를 활용하여 개발·테스트 환경에 초기 데이터를 주입한다.
