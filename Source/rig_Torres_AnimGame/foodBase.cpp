// Fill out your copyright notice in the Description page of Project Settings.


#include "foodBase.h"
#include "PlayerBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AfoodBase::AfoodBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	grabBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GrabBox"));
	RootComponent = grabBox;
	grabBox->SetBoxExtent(FVector(32,32, 32));

	pickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	pickupMesh->SetupAttachment(GetRootComponent());
	pickupMesh->SetSimulatePhysics(false);
	pickupMesh->SetWorldScale3D(FVector(5.5f, 5.5f, 5.5f));

}

// Called when the game starts or when spawned
void AfoodBase::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AfoodBase::OnOverlapBegin);
	startingLocation = GetActorLocation();
	startingRotation = GetActorRotation();
	
	if (bSpawnWithPhysics)
	{
		enablePhysics();
	}
}

// Called every frame
void AfoodBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWorldIs2D)
	{
		FVector lerp = FMath::Lerp(GetActorLocation(), FVector(0, GetActorLocation().Y, GetActorLocation().Z), 5 * DeltaTime);
		SetActorLocation(lerp);
	}
	else
	{
		FRotator spinTheBanana = GetActorRotation();
		spinTheBanana.Yaw += 1;
		SetActorRotation(spinTheBanana);
	}
}

void AfoodBase::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	APlayerBase* player = Cast<APlayerBase>(otherActor);
	if (!player) return;

	player->addPoints(pointsToGive);
	Destroy();
}

void AfoodBase::enablePhysics()
{
	pickupMesh->SetSimulatePhysics(true);
}

void AfoodBase::switchWorld()
{
	bWorldIs2D = !bWorldIs2D;

	if (bWorldIs2D == false)
	{
		SetActorLocation(startingLocation);
		SetActorRotation(startingRotation);
	}
	else
	{
		SetActorRotation(FRotator(90.0f, 0, 0.0f));
	}
}
