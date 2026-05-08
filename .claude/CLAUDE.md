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

The project is in early/skeleton state. Source files go in the root alongside the `.vcxproj`; Visual Studio filters them into three logical groups: Source Files (`.cpp`), Header Files (`.h`), and Resource Files. Add new files to `SampleOrderSystem.vcxproj` under the appropriate `<ClCompile>` or `<ClInclude>` item group.
