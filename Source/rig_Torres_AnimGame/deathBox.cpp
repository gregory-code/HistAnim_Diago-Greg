// Fill out your copyright notice in the Description page of Project Settings.


#include "deathBox.h"
#include "PlayerBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AdeathBox::AdeathBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	grabBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GrabBox"));
	grabBox->SetBoxExtent(FVector(10000, 10000, 5));
}

// Called when the game starts or when spawned
void AdeathBox::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &AdeathBox::OnOverlapBegin);
}

// Called every frame
void AdeathBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AdeathBox::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	APlayerBase* player = Cast<APlayerBase>(otherActor);
	if (!player) return;
	
	player->respawn();
}

