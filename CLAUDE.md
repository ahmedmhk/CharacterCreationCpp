# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an Unreal Engine 5.5 C++ project called "CharacterCreationCpp" designed for character creation functionality. It's a basic Unreal project with minimal custom code currently implemented.

## Environment Paths (Windows)

- **Unreal Engine Installation**: `C:\Epic Games\UE_5.5\`
- **UnrealEditor Executable**: `C:\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe`
- **UnrealEditor-Cmd Executable**: `C:\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor-Cmd.exe`
- **Build Batch File**: `C:\Epic Games\UE_5.5\Engine\Build\BatchFiles\Build.bat`
- **Project Path**: `C:\Users\ahmed\Documents\Unreal Projects\CharacterCreationCpp\CharacterCreationCpp.uproject`

## Build System and Commands

### Building the Project (Windows)
- **Build Editor**: `"C:\Epic Games\UE_5.5\Engine\Build\BatchFiles\Build.bat" CharacterCreationCppEditor Win64 Development "C:\Users\ahmed\Documents\Unreal Projects\CharacterCreationCpp\CharacterCreationCpp.uproject" -waitmutex`
- **Rebuild Editor**: Add `-clean` before `-waitmutex` to clean and rebuild

### Running Commandlets (Windows)
**IMPORTANT**: Always use these exact commands with all parameters:

```powershell
# Run CharacterCreationCommandlet (processes sprite sheets)
& "C:\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "C:\Users\ahmed\Documents\Unreal Projects\CharacterCreationCpp\CharacterCreationCpp.uproject" -run=CharacterCreationCommandlet -batch -nullrhi -nosplash -nopause

# Run LevelCreationCommandlet (creates Map1)
& "C:\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "C:\Users\ahmed\Documents\Unreal Projects\CharacterCreationCpp\CharacterCreationCpp.uproject" -run=LevelCreationCommandlet -nullrhi -nosplash -nopause

# Run GameModeCreationCommandlet (creates MyGameMode)
& "C:\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "C:\Users\ahmed\Documents\Unreal Projects\CharacterCreationCpp\CharacterCreationCpp.uproject" -run=GameModeCreationCommandlet -nullrhi -nosplash -nopause
```

### Launching the Editor
```powershell
& "C:\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Users\ahmed\Documents\Unreal Projects\CharacterCreationCpp\CharacterCreationCpp.uproject"
```

## Project Structure

### Core Module Configuration
- **Primary Module**: `CharacterCreationCpp` (Runtime module)
- **Dependencies**: Core, CoreUObject, Engine, InputCore, EnhancedInput, Paper2D, GameplayTags
- **Build System**: Uses Unreal Build Tool (UBT) with C++20 and Unreal 5.5 include order
- **Active Plugins**: ModelingToolsEditorMode (Editor-only), Paper2D, GameplayAbilities, RawInput

### Source Code Layout
```
Source/
├── CharacterCreationCpp.Target.cs          # Game target configuration
├── CharacterCreationCppEditor.Target.cs    # Editor target configuration
└── CharacterCreationCpp/
    ├── CharacterCreationCpp.Build.cs       # Module build configuration
    ├── CharacterCreationCpp.h              # Main module header
    ├── CharacterCreationCpp.cpp             # Main module implementation
    ├── CharacterCreationCommandlet/        # Sprite processing commandlet
    ├── LevelCreationCommandlet/            # Level creation commandlet
    ├── GameModeCreationCommandlet/         # Game mode creation commandlet
    ├── MyGameMode.cpp/h                    # Custom game mode
    └── Warrior*Character.cpp/h             # Generated character classes
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