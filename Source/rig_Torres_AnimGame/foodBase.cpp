// Fill out your copyright notice in the Description page of Project Settings.


#include "foodBase.h"
#include "PlayerBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AfoodBase::AfoodBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	pickupMesh->SetupAttachment(GetRootComponent());
	pickupMesh->SetSimulatePhysics(false);
	pickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pickupMesh->SetWorldScale3D(FVector(5.5f, 5.5f, 5.5f));

	RootComponent = pickupMesh;
	grabBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GrabBox"));
	grabBox->SetBoxExtent(FVector(32,32, 32));
	grabBox->AttachToComponent(pickupMesh, FAttachmentTransformRules::KeepRelativeTransform);


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
		if (isActive) return;
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
	//grabBox->SetSimulatePhysics(true);
	grabBox->AttachToComponent(pickupMesh, FAttachmentTransformRules::KeepRelativeTransform);
	grabBox->SetBoxExtent(FVector(12, 12, 12));
	isActive = true;
	pickupMesh->SetSimulatePhysics(true);
	pickupMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AfoodBase::switchWorld()
{
	bWorldIs2D = !bWorldIs2D;

	SetActorRotation(startingRotation);

	if (bWorldIs2D == false)
	{
		SetActorLocation(startingLocation);
	}
}
