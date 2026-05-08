# Phase 1: 프로젝트 구조 & 도메인 엔티티

## 목표

빌드 가능한 프로젝트 골격을 구성하고, 시스템 전체에서 사용되는 도메인 엔티티 헤더를 정의한다.  
이 단계에서는 비즈니스 로직이 없으므로 별도 테스트 케이스는 없으나, 빌드 통과가 완료 기준이다.

---

## 구현 파일 목록

```
src/
├── model/
│   ├── Sample.h
│   ├── Order.h
│   └── ProductionJob.h
test/
└── test_main.cpp
```

---

## 구현 상세

### `src/model/Sample.h`

```cpp
#pragma once
#include <string>

struct Sample {
    std::string id;
    std::string name;
    double      avgProductionTime; // min/ea
    double      yieldRate;         // 0 < yieldRate <= 1.0
    int         stock;
};
```

### `src/model/Order.h`

```cpp
#pragma once
#include <string>

enum class OrderStatus {
    RESERVED,
    PRODUCING,
    CONFIRMED,
    RELEASE,
    REJECTED
};

std::string statusToString(OrderStatus s);

struct Order {
    std::string orderId;       // ORD-YYYYMMDD-NNNN
    std::string sampleId;
    std::string customerName;
    int         quantity;
    OrderStatus status;
    std::string createdAt;     // ISO 8601 문자열
};
```

### `src/model/Order.cpp`

```cpp
#include "Order.h"

std::string statusToString(OrderStatus s) {
    switch (s) {
        case OrderStatus::RESERVED:  return "RESERVED";
        case OrderStatus::PRODUCING: return "PRODUCING";
        case OrderStatus::CONFIRMED: return "CONFIRMED";
        case OrderStatus::RELEASE:   return "RELEASE";
        case OrderStatus::REJECTED:  return "REJECTED";
        default:                     return "UNKNOWN";
    }
}
```

### `src/model/ProductionJob.h`

```cpp
#pragma once
#include <string>

struct ProductionJob {
    std::string orderId;
    std::string sampleId;
    int         shortage;    // 부족분 = quantity - stock_at_approval
    int         actualQty;   // ceil(shortage / (yieldRate * 0.9))
    double      totalTime;   // avgProductionTime * actualQty
};
```

### `test/test_main.cpp`

```cpp
#include <gtest/gtest.h>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

---

## .vcxproj 파일 구조 수정

기존 `SampleOrderSystem.vcxproj`의 필터 그룹 외에, 실제 소스 파일을 프로젝트에 추가한다.  
`src/` 하위 디렉토리를 Visual Studio 필터로 구성한다:

| 필터 이름 | 경로 |
|---|---|
| Source Files/model | `src/model/*.cpp` |
| Header Files/model | `src/model/*.h` |
| Source Files/repository | `src/repository/*.cpp` |
| Header Files/repository | `src/repository/*.h` |
| Source Files/service | `src/service/*.cpp` |
| Header Files/service | `src/service/*.h` |
| Source Files/controller | `src/controller/*.cpp` |
| Header Files/controller | `src/controller/*.h` |
| Source Files/view | `src/view/*.cpp` |
| Header Files/view | `src/view/*.h` |

테스트 프로젝트(`SampleOrderSystem_Test.vcxproj`)는 별도 프로젝트로 분리하여 `test/` 를 참조한다.

---

## 완료 기준

- [ ] `src/model/` 세 파일이 오류 없이 컴파일됨
- [ ] `statusToString()` 5개 케이스 모두 매핑 확인
- [ ] 테스트 프로젝트 빌드 후 `[0 tests passed]` 출력 (빈 테스트 스위트)
- [ ] Coverage: Phase 1은 비즈니스 로직 없으므로 Coverage 대상 없음 — Phase 2부터 측정 시작
- [ ] `git commit`: "Phase 1: Add domain entities and project structure"
