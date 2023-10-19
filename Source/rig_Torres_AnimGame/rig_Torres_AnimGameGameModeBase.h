// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/PostProcessVolume.h"
#include "rig_Torres_AnimGameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RIG_TORRES_ANIMGAME_API Arig_Torres_AnimGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	public:
		Arig_Torres_AnimGameGameModeBase();
	
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Objects")
			TArray<AActor*> levelObjects;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Objects")
			APostProcessVolume* levelPostProcess;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Objects")
			TArray<FVector> levelObjectsSpawn;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Objects")
			bool bWorldIs2D;

		void switchWorld();

	protected:
		virtual void BeginPlay() override;


};
