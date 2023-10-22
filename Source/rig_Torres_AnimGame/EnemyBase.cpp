// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "enemyControllerAI.h"
#include "PlayerBase.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	hitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	hitBox->SetBoxExtent(FVector(40, 40, 40));
	hitBox->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AEnemyBase::OnOverlapBegin);

	navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	
	aiController = Cast<AenemyControllerAI>(GetController());

	GetCharacterMovement()->MaxWalkSpeed = 0;

	startingLocation = GetActorLocation();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyBase::delayedTick, 5, true);
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (worldIs2D)
	{
		FVector lerp = FMath::Lerp(GetActorLocation(), FVector(0, GetActorLocation().Y, GetActorLocation().Z), 5 * DeltaTime);
		SetActorLocation(lerp);
	}
	
	if (bFaceLocation) 
	{
		if (worldIs2D)
		{
			FVector target = (bSeesPlayer) ? sensedActor->GetActorLocation() : targetLocation.Location;
			FRotator faceDirection = (target - GetActorLocation()).Rotation();
			faceDirection.Pitch = 0;
			SetActorRotation(faceDirection);
		}
		else
		{
			FVector target = (bSeesPlayer) ? sensedActor->GetActorLocation() : targetLocation.Location;
			FRotator faceDirection = (target - GetActorLocation()).Rotation();
			FRotator lerp = FMath::RInterpTo(GetActorRotation(), faceDirection, DeltaTime, 3);
			lerp.Pitch = 0;
			SetActorRotation(lerp);
		}
	}

	if (!aiController) return;

	if (bRoaming)
	{
		if (FVector::Dist(GetActorLocation(), targetLocation) < 200)
		{
			changeSpeed(0, DeltaTime);
		}
		else
		{
			changeSpeed(50, DeltaTime);
		}
		return;
	}

	if (bSeesPlayer && bFreezeMovement == false)
	{
		aiController->MoveToActor(sensedActor);
		changeSpeed(400, DeltaTime);
	}
	else
	{
		changeSpeed(0, DeltaTime);
	}

}

void AEnemyBase::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	if (bHasDied == true) return;

	APlayerBase* player = Cast<APlayerBase>(otherActor);
	if (!player) return;

	if (player->GetCharacterMovement()->IsFalling())
	{
		player->bounceOffEnemy();

		bHasDied = true;
		bFreezeMovement = true;
		aiController->StopMovement();

		PlayAnimMontage(M_death, 1, NAME_None);

		USkeletalMeshComponent* skeletalMeshComponent = this->FindComponentByClass<USkeletalMeshComponent>();
		UAnimInstance* animInstance = skeletalMeshComponent->GetAnimInstance();
		UAnimMontage* currentMontage = animInstance->GetCurrentActiveMontage();
		float montageLength = currentMontage->GetPlayLength();
		montageLength -= 0.4f;

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyBase::deathPoof, montageLength, false);
	}
	else 
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Player has died"));
	}
}



// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::findStimulus(FVector stimLocation, bool bSeeing, AActor* sensed)
{
	if (sensed->Tags[0] != "Player") return; // it will crash if there's no tags

	bRoaming = false;

	stimulusLocation = stimLocation;
	bSeesPlayer = bSeeing;
	sensedActor = sensed;

	bFaceLocation = true;

	if (bFreezeMovement == true) return;

	if (bFirstTimeSeeingPlayer == false) noticesPlayer();

	navPath = navSystem->FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), stimLocation);
	moveOnPath(stimLocation);
}

void AEnemyBase::noticesPlayer()
{
	bFirstTimeSeeingPlayer = true;

	bFreezeMovement = true;

	PlayAnimMontage(M_notice, 1, NAME_None);

	aiController->StopMovement();

	USkeletalMeshComponent* skeletalMeshComponent = this->FindComponentByClass<USkeletalMeshComponent>();
	UAnimInstance* animInstance = skeletalMeshComponent->GetAnimInstance();
	UAnimMontage* currentMontage = animInstance->GetCurrentActiveMontage();
	float montageLength = currentMontage->GetPlayLength();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyBase::unfreeze, montageLength, false);
}

void AEnemyBase::moveOnPath(FVector location)
{
	navPath = navSystem->FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), location);

	if (navPath && navPath->PathPoints.Num() > 1)
	{
		if (!aiController || bFreezeMovement) return;

		aiController->MoveToLocation(navPath->PathPoints.Last());
	}
}

void AEnemyBase::deathPoof()
{
	float randomNumOfFruits = FMath::FRandRange(0.0f, 5);

	for (int i = 0; i < randomNumOfFruits; ++i)
	{
		float ranFruits = FMath::FRandRange(0.0f, food_BluePrints.Num());

		if (food_BluePrints.IsValidIndex(ranFruits))
		{
			AActor* spawnedFood = GetWorld()->SpawnActor<AActor>(food_BluePrints[ranFruits]->GetClass(), GetActorLocation(), GetActorRotation());

			AfoodBase* foodBase = Cast<AfoodBase>(spawnedFood);
			if (foodBase)
			{
				if(worldIs2D) foodBase->switchWorld();
				foodBase->enablePhysics();
			}
			
			UPrimitiveComponent* spawnedFoodComponent = Cast<UPrimitiveComponent>(spawnedFood);
			if (spawnedFoodComponent)
			{
				spawnedFoodComponent->AddImpulse(FMath::VRand() * 50, NAME_None, true);
			}
		}
	}

	Destroy();
}

void AEnemyBase::unfreeze()
{
	bFreezeMovement = false;
}

void AEnemyBase::switchWorld()
{
	worldIs2D = !worldIs2D;

	delayedTick();

	GetController()->GetPawn()->bUseControllerRotationYaw = worldIs2D;

	if (worldIs2D)
	{
		FRotator CurrentRotation = GetActorRotation();
		float CurrentZRotation = CurrentRotation.Yaw;

		float differenceFoward = FMath::Abs(90 - CurrentZRotation);
		float differenceBack = FMath::Abs(-90 - CurrentZRotation);

		float rotate = (differenceFoward < differenceBack) ? 90 : -90;
		SetActorRotation(FRotator(0.0f, rotate, 0.0f));
	}
}

void AEnemyBase::delayedTick()
{
	if (bSeesPlayer == false)
	{
		bFaceLocation = true;
		bRoaming = true;

		if (worldIs2D == true)
		{

			navSystem->GetRandomReachablePointInRadius(startingLocation, getRadius(), targetLocation);
			targetLocation.Location.X = 0;
			moveOnPath(targetLocation.Location);
		}
		else if (worldIs2D == false)
		{
			navSystem->GetRandomReachablePointInRadius(startingLocation, getRadius(), targetLocation);
			moveOnPath(targetLocation.Location);
		}
	}

}

float AEnemyBase::getRadius()
{
	ANavMeshBoundsVolume* navMeshBoundsVolume = nullptr;
	for (TActorIterator<ANavMeshBoundsVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		navMeshBoundsVolume = *ActorItr;
		break;
	}
	FBoxSphereBounds bounds = navMeshBoundsVolume->GetBounds();

	return bounds.SphereRadius;
}

void AEnemyBase::changeSpeed(int speed, float deltaTime)
{
	float currentSpeed = GetCharacterMovement()->MaxWalkSpeed;
	float lerpSpeed = FMath::Lerp(currentSpeed, speed, deltaTime * 2.5f);

	GetCharacterMovement()->MaxWalkSpeed = lerpSpeed;
}
