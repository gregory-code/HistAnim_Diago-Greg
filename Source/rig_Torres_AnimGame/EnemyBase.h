// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "EnemyBase.generated.h"

UCLASS()
class RIG_TORRES_ANIMGAME_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:

	AEnemyBase();



protected:

	virtual void BeginPlay() override;

	//Private components

	UBoxComponent* hitBox;

public:	

	virtual void Tick(float DeltaTime) override;


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
