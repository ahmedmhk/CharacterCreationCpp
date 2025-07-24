# Archive Directory

This directory contains source files that are no longer actively used in the project but are kept for historical reference.

## Archived Files

### Console Command Implementation (Replaced by Commandlet)
- **SpriteProcessorCommand.h** - Header file for console command implementation
- **SpriteProcessorCommand.cpp** - Implementation of ProcessSpriteSheet console command

**Reason for archival:** These files implemented the console command functionality which has been replaced by the commandlet-based approach (`CharacterCreationCommandlet`). The commandlet provides better command-line integration and parameter handling for automated execution.

**Date archived:** 2025-07-03
**Replaced by:** CharacterCreationCommandlet.h/cpp