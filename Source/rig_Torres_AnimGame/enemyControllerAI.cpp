// Fill out your copyright notice in the Description page of Project Settings.


#include "enemyControllerAI.h"
#include "EngineUtils.h"
#include "Math/Vector.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionComponent.h"

void AenemyControllerAI::BeginPlay()
{
	Super::BeginPlay();

	myEnemy = Cast<AEnemyBase>(GetPawn());
	if (!myEnemy && GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("AI Controller can't find enemy pawn"));

}

AenemyControllerAI::AenemyControllerAI()
{
	aiPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	aiSight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISight"));
	aiSight->SightRadius = 530;
	aiSight->LoseSightRadius = 700;
	aiSight->PeripheralVisionAngleDegrees = 90;
	aiSight->DetectionByAffiliation.bDetectEnemies = true;
	aiSight->DetectionByAffiliation.bDetectFriendlies = true;
	aiSight->DetectionByAffiliation.bDetectNeutrals = true;
	aiSight->SetMaxAge(5);
	aiPerception->ConfigureSense(*aiSight);

	aiHear = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("AIHearing"));
	aiHear->HearingRange = 500;
	aiHear->DetectionByAffiliation.bDetectEnemies = true;
	aiHear->DetectionByAffiliation.bDetectFriendlies = true;
	aiHear->DetectionByAffiliation.bDetectNeutrals = true;
	aiPerception->ConfigureSense(*aiHear);
}

void AenemyControllerAI::OnTargetPerceptionUpdated(AActor* otherActor, FAIStimulus stim)
{
	myEnemy->findStimulus(stim.StimulusLocation, stim.WasSuccessfullySensed(), otherActor);
}

void AenemyControllerAI::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	aiPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AenemyControllerAI::OnTargetPerceptionUpdated);
}
