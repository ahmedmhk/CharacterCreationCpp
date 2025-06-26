# CharacterCreationCpp - 2.5D Sprite Character System

## Overview

This Unreal Engine 5.6 C++ project provides a comprehensive system for processing sprite sheets and creating 2.5D characters with animations. Based on the reference architecture, it implements a clean, efficient pipeline for converting 8x6 sprite grids into fully animated characters.

## System Architecture

### Core Components

1. **SpriteSheetProcessor** - Handles sprite sheet import, texture configuration, sprite extraction, and flipbook creation
2. **WarriorCharacter** - 2.5D character class with movement, animations, and input handling
3. **SpriteProcessorCommand** - Console command interface for automation
4. **Custom Logging** - Project-specific log category (`LogCharacterCreation`) for debugging

### Key Features

- **Automated Sprite Processing**: Converts 8x6 sprite sheets into individual sprites and flipbook animations
- **Paper2D Integration**: Applies proper texture settings for pixel-perfect 2D sprites
- **8 Animation Types**: Supports Idle, Move, and 6 directional attack animations
- **2.5D Character System**: Movement constrained to XY plane with proper collision
- **Console Command Workflow**: Scriptable pipeline for automation
- **Enhanced Input Support**: Modern input system with WASD movement and attack controls

## Usage

### Prerequisites

- Unreal Engine 5.6
- Paper2D plugin enabled
- Warrior_Blue.png sprite sheet in RawAssets folder (8 rows x 6 columns)

### Console Commands

The system provides a single console command for processing sprite sheets:

```
ProcessSpriteSheet
```

This command:
1. Imports Warrior_Blue.png from RawAssets folder
2. Applies Paper2D texture settings (nearest filtering, no compression)
3. Extracts 48 individual sprites from the 8x6 grid
4. Creates 8 flipbook animations (one for each row)
5. Saves all assets to appropriate Content directories

### Generated Assets

After running the command, the following assets are created:

- **Textures**: `/Game/Textures/` (49 textures: 1 main + 48 individual sprites)
- **Sprites**: `/Game/Sprites/` (48 individual Paper2D sprites)
- **Animations**: `/Game/Animations/` (8 flipbook animations)

### Animation Mapping

The 8 rows in the sprite sheet correspond to:

1. **Row 0**: Idle
2. **Row 1**: Move
3. **Row 2**: AttackSideways
4. **Row 3**: AttackSideways2
5. **Row 4**: AttackDownwards
6. **Row 5**: AttackDownwards2
7. **Row 6**: AttackUpwards
8. **Row 7**: AttackUpwards2

### Character Features

The WarriorCharacter class provides:

- **2.5D Movement**: XY-plane constrained movement with gravity
- **Sprite Flipping**: Automatic sprite direction based on movement
- **Animation States**: Idle, Move, and 8 attack animation variants
- **Input Handling**: Traditional WASD movement + directional attacks
- **Attack System**: Timed attacks with automatic return to idle/move states

## Development Notes

### Custom Log Category

Use the custom log category for debugging:

```bash
# Enable verbose logging
log LogCharacterCreation Warning

# Filter logs with grep
grep "LogCharacterCreation" Saved/Logs/CharacterCreationCpp.log
```

### Building

```bash
# Build editor
make CharacterCreationCppEditor

# Build game
make CharacterCreationCpp
```

### Testing

1. Build the project
2. Launch Unreal Editor (can use -nullrhi for headless mode)
3. Run `ProcessSpriteSheet` in the console
4. Check Content Browser for generated assets
5. Create a character blueprint from WarriorCharacter class
6. Test in-game with WASD movement and attack inputs

## Technical Implementation

### Sprite Extraction Process

The system uses advanced reflection techniques to access protected UPaperSprite properties, allowing for:

- Individual texture creation for each sprite
- Proper UV coordinate setup
- Centered pivot points
- Optimized Paper2D texture settings

### Memory Management

- Uses Unreal's UObject system for automatic garbage collection
- Proper package creation and asset registration
- Thread-safe texture data handling

### Error Handling

Comprehensive error handling with detailed logging for:

- File existence validation
- Image format verification
- Memory allocation checks
- Asset creation validation

## Project Status

✅ **Core System**: Fully implemented and tested  
✅ **Build System**: Successful compilation  
✅ **Console Commands**: Registered and functional  
✅ **Character System**: Complete 2.5D character with animations  
✅ **Documentation**: Comprehensive usage guide  

The system is ready for production use and can be extended for additional sprite formats or character types.