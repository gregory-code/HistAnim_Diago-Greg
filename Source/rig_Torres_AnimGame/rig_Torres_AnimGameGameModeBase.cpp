// Copyright Epic Games, Inc. All Rights Reserved.


#include "rig_Torres_AnimGameGameModeBase.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/World.h"

Arig_Torres_AnimGameGameModeBase::Arig_Torres_AnimGameGameModeBase()
{

}

void Arig_Torres_AnimGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	bWorldIs2D = false;

	TArray<AActor*> levelPostProcesses;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "PostProcess", levelPostProcesses);
	if (levelPostProcesses.Num() > 0)
	{
		levelPostProcess = Cast<APostProcessVolume>(levelPostProcesses[0]);
		if (levelPostProcess) levelPostProcess->bEnabled = false;
	}

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "object", levelObjects);
	for (AActor* Object : levelObjects)
	{
		levelObjectsSpawn.Add(Object->GetActorLocation());
	}

}

void Arig_Torres_AnimGameGameModeBase::switchWorld()
{
	bWorldIs2D = !bWorldIs2D;
	if(levelPostProcess) levelPostProcess->bEnabled = bWorldIs2D;
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Switched"));


	for (int i = 0; i < levelObjects.Num(); ++i)
	{
		FVector lerp;

		lerp = (bWorldIs2D) ? FVector(0, levelObjectsSpawn[i].Y, levelObjectsSpawn[i].Z) : levelObjectsSpawn[i] ;

		levelObjects[i]->SetActorLocation(lerp);
	}
}
