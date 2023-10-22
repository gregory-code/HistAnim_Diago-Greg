// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "foodBase.generated.h"

UCLASS()
class RIG_TORRES_ANIMGAME_API AfoodBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AfoodBase();

	void enablePhysics();
	void switchWorld();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Food Details")
		UBoxComponent* grabBox;

	UPROPERTY(VisibleAnywhere, Category = "Food Details")
		UStaticMeshComponent* pickupMesh;

	UPROPERTY(EditAnywhere, Category = "Food Details")
		bool bSpawnWithPhysics;

	UPROPERTY(EditAnywhere, Category = "Food Details")
		int pointsToGive{1};

	FVector startingLocation;
	FRotator startingRotation;
	bool bWorldIs2D;
	bool isActive;


public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void OnOverlapBegin(AActor* overlappedActor, AActor* otherActor);
};
