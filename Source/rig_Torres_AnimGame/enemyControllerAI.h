// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyBase.h"
#include "enemyControllerAI.generated.h"

/**
 * 
 */
UCLASS()
class RIG_TORRES_ANIMGAME_API AenemyControllerAI : public AAIController
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;

public:

	AenemyControllerAI();

	class UAISenseConfig_Sight* aiSight;
	class UAISenseConfig_Hearing* aiHear;

	UPROPERTY()
		AEnemyBase* myEnemy;

	UPROPERTY(EditAnywhere)
		UAIPerceptionComponent* aiPerception;

	UFUNCTION()
		void OnTargetPerceptionUpdated(AActor* otherActor, FAIStimulus stim);

	virtual void PostInitializeComponents() override;
};
