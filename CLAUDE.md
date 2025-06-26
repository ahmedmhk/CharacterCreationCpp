# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an Unreal Engine 5.6 C++ project called "CharacterCreationCpp" designed for character creation functionality. It's a basic Unreal project with minimal custom code currently implemented.

## Build System and Commands

### Building the Project
- **Main Editor Build**: `make CharacterCreationCppEditor` or `make CharacterCreationCppEditor-Linux-Development`
- **Game Build**: `make CharacterCreationCpp` or `make CharacterCreationCpp-Linux-Development`
- **Complete Standard Build**: `make StandardSet` (builds required tools + editor + insights)
- **Generate Project Files**: `make configure`

### Development Builds
- **Debug Editor**: `make CharacterCreationCppEditor-Linux-DebugGame`
- **Development Editor**: `make CharacterCreationCppEditor-Linux-Development`
- **Debug Game**: `make CharacterCreationCpp-Linux-DebugGame`
- **Shipping Game**: `make CharacterCreationCpp-Linux-Shipping`

### Platform-Specific Builds
- **Android**: Use targets like `CharacterCreationCpp-Android-Development`
- **Linux ARM64**: Use targets like `CharacterCreationCpp-LinuxArm64-Development`

### Launching the Project
```bash
"/home/ahmedmhk/UnrealEngine/Linux_Unreal_Engine_5.6.0/Engine/Binaries/Linux/UnrealEditor" "/home/ahmedmhk/Documents/Unreal Projects/CharacterCreationCpp/CharacterCreationCpp.uproject" -nullrhi
```

## Project Structure

### Core Module Configuration
- **Primary Module**: `CharacterCreationCpp` (Runtime module)
- **Dependencies**: Core, CoreUObject, Engine, InputCore, EnhancedInput
- **Build System**: Uses Unreal Build Tool (UBT) with C++20 and Unreal 5.6 include order
- **Active Plugin**: ModelingToolsEditorMode (Editor-only)

### Source Code Layout
```
Source/
├── CharacterCreationCpp.Target.cs          # Game target configuration
├── CharacterCreationCppEditor.Target.cs    # Editor target configuration
└── CharacterCreationCpp/
    ├── CharacterCreationCpp.Build.cs       # Module build configuration
    ├── CharacterCreationCpp.h              # Main module header (minimal)
    └── CharacterCreationCpp.cpp             # Main module implementation
```

### Key Directories
- **Source/**: All C++ source code and build configuration
- **Content/**: Unreal assets (.uasset, .umap files) - currently minimal
- **Config/**: Engine and game configuration files
- **Binaries/**: Compiled binaries and shared libraries
- **Intermediate/**: Build artifacts and generated code

## Development Notes

### Current Implementation Status
- Basic Unreal module structure is in place
- Main module files contain only standard Unreal boilerplate
- No custom gameplay classes, characters, or UI components yet implemented
- Project is ready for character creation system development

### Module Dependencies
The project currently uses standard Unreal modules:
- **Core/CoreUObject**: Essential Unreal framework
- **Engine**: Core engine functionality  
- **InputCore/EnhancedInput**: Input handling system

### Build Configuration
- Uses PCH (Pre-Compiled Headers) with explicit/shared mode
- Configured for BuildSettingsVersion.V5
- Targets both Game and Editor configurations