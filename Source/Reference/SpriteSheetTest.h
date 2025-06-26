#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpriteSheetProcessor.h"
#include "SpriteSheetTest.generated.h"

UCLASS()
class PAPERFLIPBOOKPROJECT_API ASpriteSheetTest : public AActor
{
	GENERATED_BODY()

public:
	ASpriteSheetTest();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpriteSheetProcessor* Processor;

public:
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Testing")
	void RunSpriteSheetTest();
};