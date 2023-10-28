// Fill out your copyright notice in the Description page of Project Settings.


#include "checkPoint.h"
#include "PlayerBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AcheckPoint::AcheckPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	grabBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GrabBox"));
	grabBox->SetBoxExtent(FVector(600, 600, 600));
}

// Called when the game starts or when spawned
void AcheckPoint::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &AcheckPoint::OnOverlapBegin);
}

// Called every frame
void AcheckPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AcheckPoint::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	APlayerBase* player = Cast<APlayerBase>(otherActor);
	if (!player) return;
	player->setCheckPoint(GetActorLocation());
}

