# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

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

Google Test (gtest) and Google Mock (gmock) are available via the `packages/gmock.1.11.0/` NuGet package. Tests are run by executing the compiled binary — gtest auto-discovers and runs all registered test cases.

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

- **[PRD.md](.claude/docs/PRD.md)** — 기능 명세서. 반도체 시료 생산주문관리 시스템의 전체 기능, 주문 상태 흐름, 데이터 모델, TDD 구현 지침, 아키텍처 가이드를 포함한다.
