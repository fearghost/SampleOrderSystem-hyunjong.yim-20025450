# Phase 3: SampleService TDD

## 목표

시료 등록, 목록 조회, 이름 검색, 재고 관리 비즈니스 로직을 `SampleService`로 구현한다.  
`ISampleRepository` Mock을 통해 Repository 없이 서비스 로직만 순수하게 검증한다.

---

## 구현 파일 목록

```
src/service/
├── SampleService.h
└── SampleService.cpp
test/service/
└── SampleServiceTest.cpp
```

---

## 인터페이스 설계

### `src/service/SampleService.h`

```cpp
#pragma once
#include "../repository/ISampleRepository.h"
#include <string>
#include <vector>
#include <optional>

class SampleService {
public:
    explicit SampleService(ISampleRepository& repo);

    // 시료 등록 — 중복 ID면 예외
    void registerSample(const std::string& id, const std::string& name,
                        double avgProductionTime, double yieldRate, int initialStock);

    [[nodiscard]] std::vector<Sample> listAll() const;
    [[nodiscard]] std::vector<Sample> searchByName(const std::string& keyword) const;
    [[nodiscard]] std::optional<Sample> findById(const std::string& id) const;
    [[nodiscard]] int totalSampleCount() const;

    // 재고 차감 — 주문 승인 시 호출
    void deductStock(const std::string& sampleId, int quantity);
    // 재고 추가 — 생산 완료 시 호출
    void addStock(const std::string& sampleId, int quantity);

private:
    ISampleRepository& repo_;
};
```

---

## TDD 구현 순서 (Red → Green → Refactor)

### Step 1: `registerSample` — 정상 등록

```
[RED]   test: registerSample 호출 시 repo.save() 1회 호출 검증
[GREEN] SampleService::registerSample 구현 → existsById 확인 후 save() 호출
[REFACTOR] Sample 생성 로직 분리
```

### Step 2: `registerSample` — 중복 ID 거부

```
[RED]   test: 동일 ID로 2회 호출 시 두 번째에서 std::invalid_argument 발생
[GREEN] existsById() == true 이면 throw
[REFACTOR] 예외 메시지 상수화
```

### Step 3: `listAll` 위임

```
[RED]   test: listAll() 호출 시 repo.findAll() 결과 그대로 반환
[GREEN] return repo_.findAll()
```

### Step 4: `searchByName` 위임

```
[RED]   test: searchByName("웨이퍼") 호출 시 repo.findByName("웨이퍼") 결과 반환
[GREEN] return repo_.findByName(keyword)
```

### Step 5: `deductStock` — 수량 차감

```
[RED]   test: deductStock("S-001", 50) 호출 시 repo.updateStock("S-001", -50) 호출
[GREEN] repo_.updateStock(sampleId, -quantity)
```

### Step 6: `addStock` — 수량 추가

```
[RED]   test: addStock("S-001", 100) 호출 시 repo.updateStock("S-001", +100) 호출
[GREEN] repo_.updateStock(sampleId, quantity)
```

---

## 테스트 케이스 상세

### `test/service/SampleServiceTest.cpp`

```cpp
class SampleServiceTest : public ::testing::Test {
protected:
    MockSampleRepository mockRepo;
    SampleService        service{mockRepo};
};
```

| # | 테스트명 | Mock 설정 | 검증 |
|---|---|---|---|
| 1 | `registerSample_callsSave` | `existsById` → false | `save()` 1회 호출, 인자 ID 일치 |
| 2 | `registerSample_duplicateId_throws` | `existsById` → true | `std::invalid_argument` 발생 |
| 3 | `registerSample_setsFieldsCorrectly` | `existsById` → false | `save()` 인자의 name, yieldRate 검증 |
| 4 | `listAll_delegatesToRepo` | `findAll()` → 3개 반환 | 반환 벡터 크기 == 3 |
| 5 | `searchByName_delegatesToRepo` | `findByName("GaN")` → 1개 반환 | 반환 벡터 크기 == 1 |
| 6 | `findById_found` | `findById("S-001")` → `Sample{...}` | optional에 값 있음 |
| 7 | `findById_notFound` | `findById("X-999")` → nullopt | `std::nullopt` 반환 |
| 8 | `deductStock_callsUpdateStockNegative` | - | `updateStock("S-001", -50)` 호출 |
| 9 | `addStock_callsUpdateStockPositive` | - | `updateStock("S-001", +100)` 호출 |
| 10 | `totalSampleCount_delegatesToRepo` | `count()` → 5 | 반환값 == 5 |

---

## 완료 기준

- [ ] `SampleServiceTest` 전체 10개 케이스 GREEN
- [ ] `SampleService`가 `ISampleRepository`에만 의존 (구체 클래스 import 없음)
- [ ] SRP 확인: `SampleService`가 시료 도메인 로직만 보유 (콘솔 출력·파일 I/O 없음)
- [ ] Coverage: `OpenCppCoverage.exe --sources C:*.cpp --export_type=html:coverage -- .\x64\Debug\SampleOrderSystem_Test.exe` → `SampleService.cpp` 라인 커버리지 확인
- [ ] `git commit`: "Phase 3: Implement SampleService with TDD"
