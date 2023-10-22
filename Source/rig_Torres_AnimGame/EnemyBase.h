// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "EnemyBase.generated.h"

UCLASS()
class RIG_TORRES_ANIMGAME_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:

	AEnemyBase();

	void findStimulus(FVector stimLocation, bool bSeeing, AActor* sensed);
	void noticesPlayer();
	void moveOnPath(FVector location);
	void deathPoof();
	void unfreeze();
	void switchWorld();
	void delayedTick();
	float getRadius();
	void changeSpeed(int speed, float deltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_notice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_death;



protected:

	virtual void BeginPlay() override;

	//Private components
	AAIController* aiController;
	UNavigationSystemV1* navSystem;
	UNavigationPath* navPath;
	UBoxComponent* hitBox;
	FTimerHandle TimerHandle;
	bool bFreezeMovement;
	bool bFaceLocation;
	bool bHasDied;
	bool worldIs2D;
	bool bRoaming;

	//Private stimulus components
	AActor* sensedActor;
	bool bSeesPlayer;
	FVector stimulusLocation;
	FVector startingLocation;
	FNavLocation targetLocation;

	bool bFirstTimeSeeingPlayer;


public:	

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Hitbox")
		void OnOverlapBegin(AActor* overlappedActor, AActor* otherActor);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
