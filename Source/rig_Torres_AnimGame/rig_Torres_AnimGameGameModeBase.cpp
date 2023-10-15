// Copyright Epic Games, Inc. All Rights Reserved.


#include "rig_Torres_AnimGameGameModeBase.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"

Arig_Torres_AnimGameGameModeBase::Arig_Torres_AnimGameGameModeBase()
{

}

void Arig_Torres_AnimGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	bWorldIs2D = false;

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "object", levelObjects);
	for (AActor* Object : levelObjects)
	{
		levelObjectsSpawn.Add(Object->GetActorLocation());
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Found an object"));
	}

}

void Arig_Torres_AnimGameGameModeBase::switchWorld()
{
	bWorldIs2D = !bWorldIs2D;
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Switched"));

	for (int i = 0; i < levelObjects.Num(); ++i)
	{
		FVector lerp;

		if (bWorldIs2D)
		{
			//lerp = FMath::Lerp(levelObjects[i]->GetActorLocation(), FVector(0, levelObjectsSpawn[i].Y, levelObjectsSpawn[i].Z), 5 * DeltaTime);
			lerp = FVector(0, levelObjectsSpawn[i].Y, levelObjectsSpawn[i].Z);
		}
		else
		{
			//lerp = FMath::Lerp(levelObjects[i]->GetActorLocation(), levelObjectsSpawn[i], 5 * DeltaTime);
			lerp = levelObjectsSpawn[i];
		}

		levelObjects[i]->SetActorLocation(lerp);
	}
}
