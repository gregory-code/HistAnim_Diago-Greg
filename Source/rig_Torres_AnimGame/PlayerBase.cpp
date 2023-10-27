
#include "PlayerBase.h"
//
#include "GameFramework/CharacterMovementComponent.h" // For GetCharacterMovement
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "rig_Torres_AnimGameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/SlateBrush.h"
#include "Engine/Texture2D.h"
#include "playerUI.h"
#include "Engine/World.h"


// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false; // Player can't rotate, camera does
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; //
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f; // jump velocity
	GetCharacterMovement()->AirControl = 0.2;

	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	cameraBoom->SetupAttachment(RootComponent);

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	followCamera->bUsePawnControlRotation = false; //gives this control of the cameraBoom
	followCamera->OrthoWidth = 2000;
	followCamera->OrthoNearClipPlane = 300;

}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	
	PrimaryActorTick.bTickEvenWhenPaused = true;
	menuAction->bTriggerWhenPaused = true;

	time = 200;
	points = 0;

	if (!playerUI) return;
	playerUI->AddToViewport();

	GetWorld()->GetTimerManager().SetTimer(TimerTimerHandle, this, &APlayerBase::timerTick, 1, false);

	playerUI->coinText->SetText(FText::FromString("Points: " + FString::FromInt(points)));

	pointsOverlaySlot = Cast<UCanvasPanelSlot>(playerUI->pointsOverlay->Slot);
	respawnOverlaySlot = Cast<UCanvasPanelSlot>(playerUI->respawnOverlay->Slot);
	timerOverlaySlot = Cast<UCanvasPanelSlot>(playerUI->timerOverlay->Slot);
	menuPauseOverlaySlot = Cast<UCanvasPanelSlot>(playerUI->menuPauseOverlay->Slot);

	spawnPoint = GetActorLocation();

	changeCameraState();
	
	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(mappingContext, 0);
		}
	}
}

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (pointsOverlaySlot == nullptr) return;

	int respawnLerp = (bRespawnTransition) ? FMath::Lerp(respawnOverlaySlot->GetPosition().X, 0, 16 * DeltaTime) : FMath::Lerp(respawnOverlaySlot->GetPosition().X, -2000, 16 * DeltaTime);
	if (bExitScreenRight) respawnLerp = FMath::Lerp(respawnOverlaySlot->GetPosition().X, 2000, 12 * DeltaTime);
	respawnOverlaySlot->SetPosition(FVector2D(respawnLerp, 0));
	
	if (bDying) return;

	FRotator CurrentRotation = GetActorRotation();
	CurrentRotation.Pitch = 0;
	CurrentRotation.Roll = 0;
	SetActorRotation(CurrentRotation);

	if (bWorldIs2D)
	{
		FVector lerp = FMath::Lerp(GetActorLocation(), FVector(0, GetActorLocation().Y, GetActorLocation().Z), 5 * DeltaTime);
		SetActorLocation(lerp);
	}

	int pointsLerp = (bGainedPoints) ? FMath::Lerp(pointsOverlaySlot->GetPosition().Y, 180, 12 * DeltaTime) : FMath::Lerp(pointsOverlaySlot->GetPosition().Y, 200, 12 * DeltaTime);
	pointsOverlaySlot->SetPosition(FVector2D(150, pointsLerp));

	int timeLerp = (bTick) ? FMath::Lerp(timerOverlaySlot->GetPosition().X, -335, 12 * DeltaTime) : FMath::Lerp(timerOverlaySlot->GetPosition().X, -350, 12 * DeltaTime);
	timerOverlaySlot->SetPosition(FVector2D(timeLerp, 50));

	int menuLerp = (bOpenMenu) ? FMath::Lerp(menuPauseOverlaySlot->GetPosition().Y, 0, 16 * DeltaTime) : FMath::Lerp(menuPauseOverlaySlot->GetPosition().Y, 1110, 16 * DeltaTime);
	menuPauseOverlaySlot->SetPosition(FVector2D(0, menuLerp));
}


void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &APlayerBase::MoveCharacter);
		enhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);
		enhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Started, this, &APlayerBase::Jump);
		enhancedInputComponent->BindAction(switchAction, ETriggerEvent::Started, this, &APlayerBase::switchWorld);
		enhancedInputComponent->BindAction(menuAction, ETriggerEvent::Started, this, &APlayerBase::menu);
	}
}

void APlayerBase::MoveCharacter(const FInputActionValue& Value)
{
	if (bDying) return;

	const FVector2D moveVector = Value.Get<FVector2D>();

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("X: %f"), moveVector.X));

	MakeNoise(0.5f, Controller->GetPawn(), GetActorLocation());

	if (bWorldIs2D)
	{
		SetActorRotation((moveVector.X > 0) ? FRotator(0, 90, 0) : FRotator(0, -90, 0));
		Controller->SetControlRotation(FRotator(0, 0, 0));
	}

	const FRotator rotation = Controller->GetControlRotation();
	const FRotator yawRotation(0.0f, rotation.Yaw, 0.0f);

	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	if (!bWorldIs2D) AddMovementInput(forwardDirection, moveVector.Y);
	AddMovementInput(rightDirection, moveVector.X);
}

void APlayerBase::Look(const FInputActionValue& Value)
{
	const FVector2D lookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(-lookAxisVector.Y);
	AddControllerYawInput(lookAxisVector.X);
}

void APlayerBase::switchWorld()
{
	if (bDying) return;
	if (GetCharacterMovement()->IsFalling()) return;

	Arig_Torres_AnimGameGameModeBase* customGameMode = Cast<Arig_Torres_AnimGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if(customGameMode)
	{
		customGameMode->switchWorld();
		bWorldIs2D = !bWorldIs2D;
		changeCameraState();
		changeRotationState();
	}
}

void APlayerBase::menu()
{
	bOpenMenu = !bOpenMenu;
	UGameplayStatics::SetGamePaused(GetWorld(), bOpenMenu);
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(bOpenMenu);
	
	FInputModeGameOnly game;
	GetWorld()->GetFirstPlayerController()->SetInputMode(game);
}

void APlayerBase::changeCameraState()
{
	if (bWorldIs2D)
	{
		if(livesRemaining >= 1) setImage(lifeIconBlackandWhite, playerUI->livesImage1);
		if (livesRemaining >= 2) setImage(lifeIconBlackandWhite, playerUI->livesImage2);
		if (livesRemaining >= 3) setImage(lifeIconBlackandWhite, playerUI->livesImage3);

		setImage(respawnBlackandWhiteTransition, playerUI->respawnImage);

		setImage(foodIconBlackandWhite, playerUI->coinImage);
	}
	else 
	{
		if (livesRemaining >= 1)setImage(lifeIcon, playerUI->livesImage1);
		if (livesRemaining >= 2) setImage(lifeIcon, playerUI->livesImage2);
		if (livesRemaining >= 3) setImage(lifeIcon, playerUI->livesImage3);

		setImage(respawnTransition, playerUI->respawnImage);

		setImage(foodIcon, playerUI->coinImage);
	}

	cameraBoom->bUsePawnControlRotation = !bWorldIs2D; // Let's this control the camera rotation //  can also set this booleon in the blueprint editor
	cameraBoom->bInheritPitch = !bWorldIs2D;
	cameraBoom->bInheritRoll = !bWorldIs2D;
	cameraBoom->bInheritYaw = !bWorldIs2D;

	cameraBoom->TargetArmLength = (bWorldIs2D) ? 800.0f : 300.0f ;

	(bWorldIs2D) ? followCamera->SetProjectionMode(ECameraProjectionMode::Orthographic) : followCamera->SetProjectionMode(ECameraProjectionMode::Perspective);
}

void APlayerBase::changeRotationState()
{
	if (bWorldIs2D)
	{
		FRotator CurrentRotation = GetActorRotation();
		float CurrentZRotation = CurrentRotation.Yaw;

		float differenceFoward = FMath::Abs(90 - CurrentZRotation);
		float differenceBack = FMath::Abs(-90 - CurrentZRotation);

		float rotate = (differenceFoward < differenceBack) ? 90 : - 90;
		SetActorRotation(FRotator(0.0f, rotate, 0.0f));
	}
}

void APlayerBase::bounceOffEnemy()
{
	addPoints(1);

	PlayAnimMontage(M_jump, 1, NAME_None);

	FVector jumpDirection = FVector(0, 0, 1);
	float jumpVelocity = 950;

	LaunchCharacter(jumpDirection * jumpVelocity, false, false);
}

void APlayerBase::addPoints(int pointsToAdd)
{
	if (playerUI == nullptr) return;

	points += pointsToAdd;
	playerUI->coinText->SetText(FText::FromString("Points: " + FString::FromInt(points)));

	bGainedPoints = true;

	GetWorld()->GetTimerManager().SetTimer(PointsTimerHandle, this, &APlayerBase::finishedAddingPoints, 0.2f, false);
}

void APlayerBase::finishedAddingPoints()
{
	bGainedPoints = false;
}

void APlayerBase::death()
{
	if (bDying) return;

	bDying = true;

	PlayAnimMontage(M_death, 1, NAME_None);

	USkeletalMeshComponent* skeletalMeshComponent = this->FindComponentByClass<USkeletalMeshComponent>();
	UAnimInstance* animInstance = skeletalMeshComponent->GetAnimInstance();
	UAnimMontage* currentMontage = animInstance->GetCurrentActiveMontage();
	float montageLength = currentMontage->GetPlayLength();
	montageLength -= 0.4f;

	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &APlayerBase::respawn, montageLength, false);
}

void APlayerBase::respawn()
{
	if (playerUI == nullptr) return;

	respawnOverlaySlot->SetPosition(FVector2D(-2000, 0));
	bRespawnTransition = true;
	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &APlayerBase::hideScreen, 1, false);
}

void APlayerBase::hideScreen()
{
	loseLife();
	bExitScreenRight = true;
	SetActorLocation(spawnPoint);
	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &APlayerBase::finishRespawn, 0.5f, false);
}

void APlayerBase::finishRespawn()
{
	bDying = false;
	respawnOverlaySlot->SetPosition(FVector2D(-2000, 0));
	bRespawnTransition = false;
	bExitScreenRight = false;
}

void APlayerBase::loseLife()
{
	switch (livesRemaining)
	{
	case 3:
		setImage(transparent, playerUI->livesImage3);
		break;

	case 2:
		setImage(transparent, playerUI->livesImage2);
		break;

	case 1:
		setImage(transparent, playerUI->livesImage1);
		break;

	case 0:
		//Game Over
		break;
	}

	--livesRemaining;
}

void APlayerBase::timerTick()
{
	bTick = !bTick;
	if (playerUI == nullptr) return;
	--time;
	playerUI->timerText->SetText(FText::FromString("Time: " + FString::FromInt(time)));
	GetWorld()->GetTimerManager().SetTimer(TimerTimerHandle, this, &APlayerBase::timerTick, 1.2f, false);
}

void APlayerBase::setImage(UTexture2D* desiredTexture, UImage* ImageToSet)
{
	if (playerUI == nullptr) return;
	FSlateBrush brush;
	brush.SetResourceObject(desiredTexture);
	brush.SetImageSize(FVector2D(130, 130));
	if(ImageToSet == playerUI->respawnImage) brush.SetImageSize(FVector2D(2000, 1300));
	ImageToSet->SetBrush(brush);
}
