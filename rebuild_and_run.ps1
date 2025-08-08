Write-Host "Building project with updated capsule dimensions..." -ForegroundColor Green
& "C:\Program Files\UE_5.6\Engine\Build\BatchFiles\Build.bat" CharacterCreationCppEditor Win64 Development -Project="C:\Users\ahmed\Documents\Unreal Projects\CharacterCreationCpp\CharacterCreationCpp.uproject" -WaitMutex

Write-Host "`nBuild complete. Running Character Creation Commandlet..." -ForegroundColor Green
& "C:\Program Files\UE_5.6\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Users\ahmed\Documents\Unreal Projects\CharacterCreationCpp\CharacterCreationCpp.uproject" -run=CharacterCreation -batch -createcharacter -nosplash -stdout -unattended

Write-Host "`nRunning Level Creation Commandlet..." -ForegroundColor Green
& "C:\Program Files\UE_5.6\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Users\ahmed\Documents\Unreal Projects\CharacterCreationCpp\CharacterCreationCpp.uproject" -run=LevelCreation -nosplash -stdout -unattended

Write-Host "`nAll tasks completed!" -ForegroundColor Yellow