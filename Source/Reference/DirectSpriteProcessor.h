#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpriteSheetProcessor.h"
#include "DirectSpriteProcessor.generated.h"

UCLASS()
class PAPERFLIPBOOKPROJECT_API ADirectSpriteProcessor : public AActor
{
	GENERATED_BODY()

public:
	ADirectSpriteProcessor();

protected:
	virtual void BeginPlay() override;

private:
	void ProcessSpriteSheetDirectly();
};