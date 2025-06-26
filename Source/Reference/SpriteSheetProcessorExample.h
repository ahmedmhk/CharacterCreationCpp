#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpriteSheetProcessor.h"
#include "SpriteSheetProcessorExample.generated.h"

UCLASS()
class PAPERFLIPBOOKPROJECT_API ASpriteSheetProcessorExample : public AActor
{
	GENERATED_BODY()

public:
	ASpriteSheetProcessorExample();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Processing")
	FString TextureFileName = TEXT("Warrior_Blue");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite Processing")
	FSpriteSheetInfo SpriteSheetSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite Processing")
	USpriteSheetProcessor* SpriteProcessor;

	UFUNCTION(BlueprintCallable, Category = "Sprite Processing")
	void ProcessWarriorSpriteSheet();

public:
	virtual void Tick(float DeltaTime) override;
};