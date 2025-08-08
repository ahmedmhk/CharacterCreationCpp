@echo off
echo Building project with updated capsule dimensions...
call "C:\Program Files\UE_5.6\Engine\Build\BatchFiles\Build.bat" CharacterCreationCppEditor Win64 Development -Project="C:\Users\ahmed\Documents\Unreal Projects\CharacterCreationCpp\CharacterCreationCpp.uproject" -WaitMutex

echo.
echo Build complete. Running Character Creation Commandlet...
"C:\Program Files\UE_5.6\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Users\ahmed\Documents\Unreal Projects\CharacterCreationCpp\CharacterCreationCpp.uproject" -run=CharacterCreation -batch -createcharacter -nosplash -stdout -unattended

echo.
echo Running Level Creation Commandlet...
"C:\Program Files\UE_5.6\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Users\ahmed\Documents\Unreal Projects\CharacterCreationCpp\CharacterCreationCpp.uproject" -run=LevelCreation -nosplash -stdout -unattended

echo.
echo All tasks completed!
pause