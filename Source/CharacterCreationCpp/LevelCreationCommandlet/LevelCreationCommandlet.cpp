#include "LevelCreationCommandlet.h"
#include "LevelCreationLog.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Editor/EditorEngine.h"
#include "Editor.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/GarbageCollection.h"
#include "../CharacterCreationCommandlet/WarriorPurpleCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

// Include headers for atmosphere actors
#include "Atmosphere/AtmosphericFog.h"

// Forward declarations for actors not in standard headers
class ASkyAtmosphere;
class AVolumetricCloud;

ULevelCreationCommandlet::ULevelCreationCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
	ShowErrorCount = true;
	HelpDescription = TEXT("Create new levels with environmental actors");
	HelpUsage = TEXT("LevelCreationCommandlet [-mapname=<MapName>] [-outputpath=<OutputPath>] [-dryrun]");
}

int32 ULevelCreationCommandlet::Main(const FString& Params)
{
	UE_LOG(LogLevelCreation, Warning, TEXT("=== Level Creation Commandlet Started ==="));
	UE_LOG(LogLevelCreation, Warning, TEXT("Parameters: %s"), *Params);

	// Parse parameters
	FString MapName = TEXT("Map1");
	FString OutputPath = TEXT("/Game/Maps/");
	bool bDryRun = FParse::Param(*Params, TEXT("dryrun"));

	FParse::Value(*Params, TEXT("mapname="), MapName);
	FParse::Value(*Params, TEXT("m="), MapName);
	FParse::Value(*Params, TEXT("outputpath="), OutputPath);
	FParse::Value(*Params, TEXT("o="), OutputPath);

	// Validate output path
	if (!ValidateAndSanitizePath(OutputPath))
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Invalid output path: %s"), *OutputPath);
		PrintUsage();
		return 1;
	}

	// Validate map name
	if (MapName.IsEmpty())
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Map name cannot be empty"));
		PrintUsage();
		return 1;
	}

	UE_LOG(LogLevelCreation, Warning, TEXT("Creating level: %s"), *MapName);
	UE_LOG(LogLevelCreation, Warning, TEXT("Output path: %s"), *OutputPath);
	UE_LOG(LogLevelCreation, Warning, TEXT("Dry run: %s"), bDryRun ? TEXT("YES") : TEXT("NO"));

	// Handle dry run
	if (bDryRun)
	{
		PrintDryRunSummary(MapName, OutputPath);
		UE_LOG(LogLevelCreation, Warning, TEXT("=== Level Creation Commandlet Dry Run Completed ==="));
		return 0;
	}

	// Create the level
	bool bSuccess = CreateLevel(MapName, OutputPath, bDryRun);

	if (bSuccess)
	{
		PrintSuccess(MapName, OutputPath + MapName);
		
		// Clean up memory before exiting
		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
		
		UE_LOG(LogLevelCreation, Warning, TEXT("=== Level Creation Commandlet Completed Successfully ==="));
		return 0;
	}
	else
	{
		PrintFailure(MapName);
		UE_LOG(LogLevelCreation, Error, TEXT("=== Level Creation Commandlet Failed ==="));
		return 1;
	}
}

bool ULevelCreationCommandlet::CreateLevel(const FString& MapName, const FString& OutputPath, bool bDryRun)
{
	UE_LOG(LogLevelCreation, Warning, TEXT("Creating new world..."));

	// Create a new world
	UWorld* NewWorld = UWorld::CreateWorld(EWorldType::Editor, false);
	if (!NewWorld)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to create new world"));
		return false;
	}

	// Initialize the world if not already initialized
	if (!NewWorld->bIsWorldInitialized)
	{
		NewWorld->InitWorld(UWorld::InitializationValues().AllowAudioPlayback(false));
	}
	
	// Get or create world settings
	AWorldSettings* WorldSettings = NewWorld->GetWorldSettings();
	if (!WorldSettings)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to get world settings"));
		return false;
	}

	UE_LOG(LogLevelCreation, Warning, TEXT("✓ World created successfully"));

	// Spawn environmental actors
	bool bActorsSpawned = SpawnEnvironmentalActors(NewWorld);
	if (!bActorsSpawned)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to spawn environmental actors"));
		return false;
	}

	// Save the level
	FString PackageName = OutputPath + MapName;
	bool bSaved = SaveLevel(NewWorld, PackageName);
	
	return bSaved;
}

bool ULevelCreationCommandlet::SpawnEnvironmentalActors(UWorld* World)
{
	if (!World)
	{
		return false;
	}

	UE_LOG(LogLevelCreation, Warning, TEXT("Spawning environmental actors..."));

	// 1. Spawn Directional Light (Sun)
	ADirectionalLight* DirectionalLight = SpawnDirectionalLight(World);
	if (!DirectionalLight)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to spawn Directional Light"));
		return false;
	}
	UE_LOG(LogLevelCreation, Warning, TEXT("✓ Directional Light spawned"));

	// 2. Spawn Sky Light
	ASkyLight* SkyLight = SpawnSkyLight(World);
	if (!SkyLight)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to spawn Sky Light"));
		return false;
	}
	UE_LOG(LogLevelCreation, Warning, TEXT("✓ Sky Light spawned"));

	// 3. Spawn Sky Atmosphere
	ASkyAtmosphere* SkyAtmosphere = SpawnSkyAtmosphere(World);
	if (!SkyAtmosphere)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to spawn Sky Atmosphere"));
		return false;
	}
	UE_LOG(LogLevelCreation, Warning, TEXT("✓ Sky Atmosphere spawned"));

	// 4. Spawn Volumetric Cloud
	AVolumetricCloud* VolumetricCloud = SpawnVolumetricCloud(World);
	if (!VolumetricCloud)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to spawn Volumetric Cloud"));
		return false;
	}
	UE_LOG(LogLevelCreation, Warning, TEXT("✓ Volumetric Cloud spawned"));

	// 5. Spawn Exponential Height Fog
	AExponentialHeightFog* HeightFog = SpawnExponentialHeightFog(World);
	if (!HeightFog)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to spawn Exponential Height Fog"));
		return false;
	}
	UE_LOG(LogLevelCreation, Warning, TEXT("✓ Exponential Height Fog spawned"));

	// 6. Spawn Plane Actor (Ground)
	AStaticMeshActor* PlaneActor = SpawnPlaneActor(World);
	if (!PlaneActor)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to spawn Plane Actor"));
		return false;
	}
	UE_LOG(LogLevelCreation, Warning, TEXT("✓ Plane Actor spawned"));

	// 7. Spawn Warrior Purple Character on top of the plane
	// Calculate spawn location: plane is at (0,0,0) with scale 8x8x8
	// The plane mesh is 100x100 units by default, so scaled it's 800x800 units
	// Spawn character slightly above the plane surface
	FVector CharacterSpawnLocation(0.0f, 0.0f, 50.0f); // 50 units above plane center
	
	AWarriorPurpleCharacter* WarriorCharacter = SpawnWarriorPurpleCharacter(World, CharacterSpawnLocation);
	if (!WarriorCharacter)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to spawn Warrior Purple Character"));
		return false;
	}
	UE_LOG(LogLevelCreation, Warning, TEXT("✓ Warrior Purple Character spawned at location: %s"), *CharacterSpawnLocation.ToString());

	return true;
}

ADirectionalLight* ULevelCreationCommandlet::SpawnDirectionalLight(UWorld* World)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn at a specific rotation to simulate sun angle
	FRotator SunRotation(-45.0f, 30.0f, 0.0f);
	ADirectionalLight* DirectionalLight = World->SpawnActor<ADirectionalLight>(
		ADirectionalLight::StaticClass(),
		FVector::ZeroVector,
		SunRotation,
		SpawnParams
	);

	if (DirectionalLight)
	{
		UDirectionalLightComponent* LightComponent = DirectionalLight->GetComponent();
		if (LightComponent)
		{
			// Configure sun properties
			LightComponent->SetIntensity(10.0f);
			LightComponent->SetLightColor(FLinearColor(1.0f, 0.95f, 0.8f));
			LightComponent->bAtmosphereSunLight = true;
			LightComponent->SetMobility(EComponentMobility::Movable);
			
			// Enable shadows
			LightComponent->SetCastShadows(true);
			LightComponent->bCastCloudShadows = true;
		}
	}

	return DirectionalLight;
}

ASkyLight* ULevelCreationCommandlet::SpawnSkyLight(UWorld* World)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASkyLight* SkyLight = World->SpawnActor<ASkyLight>(
		ASkyLight::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (SkyLight)
	{
		USkyLightComponent* SkyLightComponent = SkyLight->GetLightComponent();
		if (SkyLightComponent)
		{
			// Set to movable for dynamic updates
			SkyLightComponent->SetMobility(EComponentMobility::Movable);
			
			// Enable real-time capture for dynamic sky
			SkyLightComponent->bRealTimeCapture = true;
			
			// Set intensity
			SkyLightComponent->SetIntensity(1.0f);
			
			// Capture the scene
			SkyLightComponent->RecaptureSky();
		}
	}

	return SkyLight;
}

ASkyAtmosphere* ULevelCreationCommandlet::SpawnSkyAtmosphere(UWorld* World)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Find the SkyAtmosphere class
	UClass* SkyAtmosphereClass = FindObject<UClass>(nullptr, TEXT("SkyAtmosphere"));
	if (!SkyAtmosphereClass)
	{
		// Try to load it
		SkyAtmosphereClass = LoadObject<UClass>(nullptr, TEXT("/Script/Engine.SkyAtmosphere"));
	}

	if (!SkyAtmosphereClass)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Could not find SkyAtmosphere class"));
		return nullptr;
	}

	AActor* SkyAtmosphere = World->SpawnActor<AActor>(
		SkyAtmosphereClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	return Cast<ASkyAtmosphere>(SkyAtmosphere);
}

AVolumetricCloud* ULevelCreationCommandlet::SpawnVolumetricCloud(UWorld* World)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Find the VolumetricCloud class
	UClass* VolumetricCloudClass = FindObject<UClass>(nullptr, TEXT("VolumetricCloud"));
	if (!VolumetricCloudClass)
	{
		// Try to load it
		VolumetricCloudClass = LoadObject<UClass>(nullptr, TEXT("/Script/Engine.VolumetricCloud"));
	}

	if (!VolumetricCloudClass)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Could not find VolumetricCloud class"));
		return nullptr;
	}

	AActor* VolumetricCloud = World->SpawnActor<AActor>(
		VolumetricCloudClass,
		FVector(0.0f, 0.0f, 5000.0f), // Spawn at height
		FRotator::ZeroRotator,
		SpawnParams
	);

	return Cast<AVolumetricCloud>(VolumetricCloud);
}

AExponentialHeightFog* ULevelCreationCommandlet::SpawnExponentialHeightFog(UWorld* World)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AExponentialHeightFog* HeightFog = World->SpawnActor<AExponentialHeightFog>(
		AExponentialHeightFog::StaticClass(),
		FVector(0.0f, 0.0f, 200.0f), // Spawn at slight height
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (HeightFog)
	{
		UExponentialHeightFogComponent* FogComponent = HeightFog->GetComponent();
		if (FogComponent)
		{
			// Configure fog properties
			FogComponent->SetFogDensity(0.02f);
			FogComponent->SetFogInscatteringColor(FLinearColor(0.5f, 0.6f, 0.7f));
			FogComponent->SetFogHeightFalloff(0.2f);
			FogComponent->SetVolumetricFog(true);
		}
	}

	return HeightFog;
}

AStaticMeshActor* ULevelCreationCommandlet::SpawnPlaneActor(UWorld* World)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn the static mesh actor at origin
	AStaticMeshActor* PlaneActor = World->SpawnActor<AStaticMeshActor>(
		AStaticMeshActor::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (PlaneActor)
	{
		UStaticMeshComponent* MeshComponent = PlaneActor->GetStaticMeshComponent();
		if (MeshComponent)
		{
			// Load the built-in plane mesh
			UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
			if (PlaneMesh)
			{
				MeshComponent->SetStaticMesh(PlaneMesh);
				
				// Set the scale to 8x8x8
				PlaneActor->SetActorScale3D(FVector(8.0f, 8.0f, 8.0f));
				
				// Ensure proper collision and physics settings
				MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
			}
			else
			{
				UE_LOG(LogLevelCreation, Error, TEXT("Failed to load plane mesh"));
				World->DestroyActor(PlaneActor);
				return nullptr;
			}
		}
	}

	return PlaneActor;
}

bool ULevelCreationCommandlet::SaveLevel(UWorld* World, const FString& PackageName)
{
	if (!World)
	{
		return false;
	}

	UE_LOG(LogLevelCreation, Warning, TEXT("Saving level to: %s"), *PackageName);

	// Create the package
	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to create package: %s"), *PackageName);
		return false;
	}

	// Rename the world into the package
	World->Rename(*FPackageName::GetLongPackageAssetName(PackageName), Package);
	
	// Set required flags for saving
	World->SetFlags(RF_Public | RF_Standalone);

	// Mark the package as dirty
	Package->MarkPackageDirty();

	// Get the file path
	FString PackageFilename = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetMapPackageExtension());
	
	// Ensure the directory exists
	FString Directory = FPaths::GetPath(PackageFilename);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*Directory))
	{
		PlatformFile.CreateDirectoryTree(*Directory);
	}

	// Save the package
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.Error = GError;
	SaveArgs.bForceByteSwapping = false;
	SaveArgs.bWarnOfLongFilename = true;
	SaveArgs.SaveFlags = SAVE_None;

	bool bSaved = UPackage::SavePackage(Package, World, *PackageFilename, SaveArgs);

	if (bSaved)
	{
		UE_LOG(LogLevelCreation, Warning, TEXT("✓ Level saved successfully to: %s"), *PackageFilename);
	}
	else
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to save level to: %s"), *PackageFilename);
	}

	return bSaved;
}

void ULevelCreationCommandlet::PrintUsage() const
{
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
	UE_LOG(LogLevelCreation, Warning, TEXT("Usage: LevelCreationCommandlet [options]"));
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
	UE_LOG(LogLevelCreation, Warning, TEXT("Options:"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  -mapname=<name>      | -m=<name>    Name of the map to create (default: Map1)"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  -outputpath=<path>   | -o=<path>    Output path for the map (default: /Game/Maps/)"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  -dryrun                              Preview operations without creating files"));
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
	UE_LOG(LogLevelCreation, Warning, TEXT("Examples:"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  LevelCreationCommandlet"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  LevelCreationCommandlet -mapname=MyLevel"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  LevelCreationCommandlet -m=TestMap -o=/Game/MyMaps/ -dryrun"));
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
}

void ULevelCreationCommandlet::PrintSuccess(const FString& MapName, const FString& PackagePath) const
{
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
	UE_LOG(LogLevelCreation, Warning, TEXT("✓ SUCCESS! Level created successfully"));
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
	UE_LOG(LogLevelCreation, Warning, TEXT("Level Details:"));
	UE_LOG(LogLevelCreation, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
	UE_LOG(LogLevelCreation, Warning, TEXT("📍 Map Name: %s"), *MapName);
	UE_LOG(LogLevelCreation, Warning, TEXT("📁 Package Path: %s"), *PackagePath);
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
	UE_LOG(LogLevelCreation, Warning, TEXT("Environmental Actors Added:"));
	UE_LOG(LogLevelCreation, Warning, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
	UE_LOG(LogLevelCreation, Warning, TEXT("☀️  Directional Light (Sun)"));
	UE_LOG(LogLevelCreation, Warning, TEXT("🌤️  Sky Light"));
	UE_LOG(LogLevelCreation, Warning, TEXT("🌌 Sky Atmosphere"));
	UE_LOG(LogLevelCreation, Warning, TEXT("☁️  Volumetric Cloud"));
	UE_LOG(LogLevelCreation, Warning, TEXT("🌫️  Exponential Height Fog"));
	UE_LOG(LogLevelCreation, Warning, TEXT("🏗️  Plane Static Mesh (Ground) - Scale: 8x8x8"));
	UE_LOG(LogLevelCreation, Warning, TEXT("🎮 Warrior Purple Character"));
	UE_LOG(LogLevelCreation, Warning, TEXT("🚩 Player Start"));
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
	UE_LOG(LogLevelCreation, Warning, TEXT("📋 Open the level in the Unreal Editor to see your new environment!"));
}

void ULevelCreationCommandlet::PrintFailure(const FString& MapName) const
{
	UE_LOG(LogLevelCreation, Error, TEXT(""));
	UE_LOG(LogLevelCreation, Error, TEXT("✗ FAILED! Level creation unsuccessful"));
	UE_LOG(LogLevelCreation, Error, TEXT(""));
	UE_LOG(LogLevelCreation, Error, TEXT("Troubleshooting:"));
	UE_LOG(LogLevelCreation, Error, TEXT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
	UE_LOG(LogLevelCreation, Error, TEXT("1. Check that the output path is valid"));
	UE_LOG(LogLevelCreation, Error, TEXT("2. Ensure you have write permissions"));
	UE_LOG(LogLevelCreation, Error, TEXT("3. Verify the map name doesn't contain invalid characters"));
	UE_LOG(LogLevelCreation, Error, TEXT("4. Check the log for specific error messages"));
}

void ULevelCreationCommandlet::PrintDryRunSummary(const FString& MapName, const FString& OutputPath) const
{
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
	UE_LOG(LogLevelCreation, Warning, TEXT("════════════════════════════════════════════════════"));
	UE_LOG(LogLevelCreation, Warning, TEXT("                  DRY RUN SUMMARY                    "));
	UE_LOG(LogLevelCreation, Warning, TEXT("════════════════════════════════════════════════════"));
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
	UE_LOG(LogLevelCreation, Warning, TEXT("The following operations would be performed:"));
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
	UE_LOG(LogLevelCreation, Warning, TEXT("Level Creation:"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  • Create new empty level: %s"), *MapName);
	UE_LOG(LogLevelCreation, Warning, TEXT("  • Save to: %s%s.umap"), *OutputPath, *MapName);
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
	UE_LOG(LogLevelCreation, Warning, TEXT("Environmental Actors to Spawn:"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  • Directional Light (Sun)"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  • Sky Light"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  • Sky Atmosphere"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  • Volumetric Cloud"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  • Exponential Height Fog"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  • Plane Static Mesh (Ground) - Scale: 8x8x8"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  • Warrior Purple Character"));
	UE_LOG(LogLevelCreation, Warning, TEXT("  • Player Start"));
	UE_LOG(LogLevelCreation, Warning, TEXT(""));
	UE_LOG(LogLevelCreation, Warning, TEXT("No files have been created or modified."));
	UE_LOG(LogLevelCreation, Warning, TEXT("Remove -dryrun flag to execute these operations."));
	UE_LOG(LogLevelCreation, Warning, TEXT("════════════════════════════════════════════════════"));
}

bool ULevelCreationCommandlet::ValidateAndSanitizePath(FString& Path) const
{
	// Ensure path starts with /Game/
	if (!Path.StartsWith(TEXT("/Game/")))
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Path must start with /Game/: %s"), *Path);
		return false;
	}
	
	// Check for directory traversal attempts
	if (Path.Contains(TEXT("..")) || Path.Contains(TEXT("//")))
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Invalid path characters detected: %s"), *Path);
		return false;
	}
	
	// Ensure path ends with slash
	if (!Path.EndsWith(TEXT("/")))
	{
		Path += TEXT("/");
	}
	
	// Validate it's a valid package path
	if (!FPackageName::IsValidLongPackageName(Path.LeftChop(1))) // Remove trailing slash for validation
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Invalid package path: %s"), *Path);
		return false;
	}
	
	return true;
}

AWarriorPurpleCharacter* ULevelCreationCommandlet::SpawnWarriorPurpleCharacter(UWorld* World, const FVector& Location)
{
	if (!World)
	{
		return nullptr;
	}

	UE_LOG(LogLevelCreation, Warning, TEXT("Spawning Warrior Purple Character..."));

	// First, spawn a PlayerStart at the location for proper player spawning
	FActorSpawnParameters PlayerStartParams;
	PlayerStartParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	APlayerStart* PlayerStart = World->SpawnActor<APlayerStart>(
		APlayerStart::StaticClass(),
		Location,
		FRotator::ZeroRotator,
		PlayerStartParams
	);

	if (PlayerStart)
	{
		PlayerStart->SetActorLabel(TEXT("PlayerStart_WarriorPurple"));
		UE_LOG(LogLevelCreation, Warning, TEXT("✓ PlayerStart spawned for character"));
	}

	// Now spawn the Warrior Purple Character
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	// Use the C++ class directly
	UClass* WarriorPurpleClass = AWarriorPurpleCharacter::StaticClass();
	UE_LOG(LogLevelCreation, Warning, TEXT("Using C++ class AWarriorPurpleCharacter"));

	AWarriorPurpleCharacter* WarriorCharacter = World->SpawnActor<AWarriorPurpleCharacter>(
		WarriorPurpleClass,
		Location,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (WarriorCharacter)
	{
		// Set some initial properties
		WarriorCharacter->SetActorLabel(TEXT("WarriorPurpleCharacter"));
		
		// Make sure the character is set to possess
		WarriorCharacter->AutoPossessPlayer = EAutoReceiveInput::Player0;
		
		// Set up a default visible sprite
		UPaperFlipbookComponent* SpriteComponent = WarriorCharacter->GetSprite();
		if (SpriteComponent)
		{
			// Load the idle animation flipbook as default
			UPaperFlipbook* IdleFlipbook = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Game/Animations/Idle_Warrior_Purple"));
			if (IdleFlipbook)
			{
				SpriteComponent->SetFlipbook(IdleFlipbook);
				SpriteComponent->SetVisibility(true);
				SpriteComponent->SetHiddenInGame(false);
				UE_LOG(LogLevelCreation, Warning, TEXT("✓ Set default Idle animation on character sprite"));
			}
			else
			{
				UE_LOG(LogLevelCreation, Warning, TEXT("✗ Could not load Idle_Warrior_Purple animation"));
			}
		}
		
		UE_LOG(LogLevelCreation, Warning, TEXT("✓ Warrior Purple Character spawned successfully"));
		UE_LOG(LogLevelCreation, Warning, TEXT("  - Location: %s"), *WarriorCharacter->GetActorLocation().ToString());
		UE_LOG(LogLevelCreation, Warning, TEXT("  - Class: %s"), *WarriorCharacter->GetClass()->GetName());
	}
	else
	{
		UE_LOG(LogLevelCreation, Error, TEXT("Failed to spawn Warrior Purple Character"));
	}

	return WarriorCharacter;
}