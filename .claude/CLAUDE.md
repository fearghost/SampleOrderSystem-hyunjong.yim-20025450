# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

**GitHub Repository**: https://github.com/fearghost/SampleOrderSystem-hyunjong.yim-20025450

## Project Overview

C++ Console Application targeting C++20, built with Visual Studio 2022 (MSBuild, toolset v145). Uses Google Mock/Test (gmock 1.11.0) for testing.

## Build Commands

```powershell
# Debug x64 (default development build)
msbuild SampleOrderSystem.vcxproj /p:Configuration=Debug /p:Platform=x64

# Release x64
msbuild SampleOrderSystem.vcxproj /p:Configuration=Release /p:Platform=x64

# Run after building
.\x64\Debug\SampleOrderSystem.exe
```

Or open `SampleOrderSystem.slnx` in Visual Studio 2022 and use the IDE build/run controls.

## Testing

Google Test (gtest) and Google Mock (gmock) are available via the `packages/gmock.1.11.0/` NuGet package.

```powershell
# 테스트 프로젝트 빌드
$msbuild = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"
& $msbuild SampleOrderSystem_Test.vcxproj /p:Configuration=Debug /p:Platform=x64

# 테스트 실행 (gtest 자동 탐색)
.\x64\Debug\SampleOrderSystem_Test.exe

# Coverage 확인 (OpenCppCoverage 설치 필요)
OpenCppCoverage.exe --sources C:*.cpp --export_type=html:coverage -- .\x64\Debug\SampleOrderSystem_Test.exe
# 결과: coverage\index.html
```

TDD 각 Phase에서 테스트 GREEN 확인 후 반드시 Coverage를 측정하여 신규 구현 코드가 테스트로 커버되는지 확인한다.

## Coding Standards (Harness)

구현 및 리팩토링 시 아래 규칙을 준수한다. Claude Code가 코드를 생성하거나 수정할 때 이 규칙이 자동으로 적용된다.

### SRP (Single Responsibility Principle)

클래스는 단 하나의 책임만 가진다.

- `model/`: 데이터 구조 보유만 (로직 없음)
- `repository/`: 저장소 CRUD만 (비즈니스 판단 없음)
- `service/`: 비즈니스 로직만 (입출력 없음)
- `controller/`: 입력 라우팅만 (비즈니스 로직 없음, Repository 직접 접근 금지)
- `view/`: 콘솔 입출력만 (상태 보유 금지)

하나의 클래스가 두 가지 이유로 변경되어야 한다면 분리 대상이다.  
리팩토링 시 기존 테스트가 모두 GREEN을 유지해야 한다.

### Commit Message 형식

코드 구현·수정 커밋에는 반드시 테스트 결과와 Coverage 결과를 포함한다.

```
<제목 (Phase N: 설명 또는 fix/refactor: 설명)>

Tests   : NN/NN PASSED
Coverage: Line XX.X% (src/ 전체)
  - 신규/변경 파일.cpp : XX.X%
  - 기타 주요 파일.cpp : XX.X%

Co-Authored-By: Claude Sonnet 4.6 (1M context) <noreply@anthropic.com>
```

- `Tests` 줄: 전체 누적 테스트 수와 통과 수를 기록한다.
- `Coverage` 줄: `src/` 전체 Line Coverage와 해당 커밋에서 신규·변경된 파일의 개별 커버리지를 기록한다.
- 테스트가 없는 순수 문서·설정 커밋은 Tests/Coverage 줄을 생략한다.

## Project Configuration

- **Language standard**: C++20 (`/std:c++20`)
- **Configurations**: Debug/Release × Win32/x64
- **SDL checks**: enabled (security warnings as errors)
- **Conformance mode**: enabled (strict standard compliance)
- **Warning level**: W3

## Architecture

4-layer structure combining ConsoleMVC and DataPersistence PoC patterns:

```
src/
├── model/       # Domain entity headers only (Sample, Order, ProductionJob, OrderStatus enum)
├── repository/  # ISampleRepository / IOrderRepository / IProductionRepository (pure virtual)
│                # + JSON file-based concrete implementations
├── service/     # Business logic: SampleService, OrderService, ProductionService
│                # Depend on interfaces only — fully mockable
├── controller/  # Input routing: one controller per menu feature
├── view/        # Console I/O with virtual methods for Google Mock
└── main.cpp     # Wires concrete repos → services → controllers via constructor injection
test/
├── MockRepositories.h  # Mock I*Repository interfaces
├── MockViews.h         # Mock View virtual methods
└── service/ controller/ repository/  # Test files per layer
```

Dependency direction: `controller → service → I*Repository ← JsonRepository`

See [PRD.md](docs/PRD.md) for full architecture rationale, dependency diagram, TDD mock strategy, and data models.

## Documentation

- **[PRD.md](docs/PRD.md)** — 기능 명세서. 전체 기능, 주문 상태 흐름, 데이터 모델, TDD 구현 지침, 아키텍처 가이드.
- **[phase1.md](docs/phase1.md)** — 프로젝트 구조 & 도메인 엔티티
- **[phase2.md](docs/phase2.md)** — Repository 인터페이스 & 인메모리 구현 & Mock
- **[phase3.md](docs/phase3.md)** — SampleService TDD
- **[phase4.md](docs/phase4.md)** — OrderService TDD (핵심 상태 전환)
- **[phase5.md](docs/phase5.md)** — ProductionService TDD (FIFO 큐)
- **[phase6.md](docs/phase6.md)** — View & Controller TDD
- **[phase7.md](docs/phase7.md)** — JSON 영속성 & 시스템 통합
