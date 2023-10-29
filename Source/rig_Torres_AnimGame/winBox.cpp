// Fill out your copyright notice in the Description page of Project Settings.


#include "winBox.h"
#include "PlayerBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AwinBox::AwinBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	grabBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GrabBox"));
	grabBox->SetBoxExtent(FVector(500, 500, 500));
}

// Called when the game starts or when spawned
void AwinBox::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &AwinBox::OnOverlapBegin);
}

// Called every frame
void AwinBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AwinBox::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	APlayerBase* player = Cast<APlayerBase>(otherActor);
	if (!player) return;

	player->wonGame();
}

