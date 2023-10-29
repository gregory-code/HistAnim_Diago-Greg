
#include "PlayerBase.h"
//
#include "GameFramework/CharacterMovementComponent.h" // For GetCharacterMovement
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "rig_Torres_AnimGameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/SlateBrush.h"
#include "Components/AudioComponent.h"
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
	
	defaultCameraOrthoNearClipPlane = followCamera->OrthoNearClipPlane;
	defaultCameraOrthoWidth = followCamera->OrthoWidth;

	PrimaryActorTick.bTickEvenWhenPaused = true;
	menuAction->bTriggerWhenPaused = true;
	

	FInputModeGameOnly input;
	GetWorld()->GetFirstPlayerController()->SetInputMode(input);

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
	controlsOverlaySlot = Cast<UCanvasPanelSlot>(playerUI->controlsOverlay->Slot);
	gameOverOverlaySlot = Cast<UCanvasPanelSlot>(playerUI->gameOverOverlay->Slot);
	winOverlaySlot = Cast<UCanvasPanelSlot>(playerUI->winOverlay->Slot);

	//UButton* button

	spawnPoint = GetActorLocation();

	pirateSong = UGameplayStatics::SpawnSound2D(GetWorld(), pirateBop, 1.0f, 1.0f, 0.0f);
	pirateBackground = UGameplayStatics::SpawnSound2D(GetWorld(), pirateWaves, 1.0f, 1.0f, 0.0f);
	cartoonSong = UGameplayStatics::SpawnSound2D(GetWorld(), cartoonBop, 1.0f, 1.0f, 0.0f);
	//cartoonBackground = UGameplayStatics::SpawnSound2D(GetWorld(), cartoonStatic, 1.0f, 1.0f, 0.0f);

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

	int menuLerp = (bOpenMenu) ? FMath::Lerp(menuPauseOverlaySlot->GetPosition().Y, 0, 16 * DeltaTime) : FMath::Lerp(menuPauseOverlaySlot->GetPosition().Y, 1110, 16 * DeltaTime);
	menuPauseOverlaySlot->SetPosition(FVector2D(0, menuLerp));

	int controlsLerp = (bHideControls) ? FMath::Lerp(controlsOverlaySlot->GetPosition().Y, 700, 16 * DeltaTime) : FMath::Lerp(controlsOverlaySlot->GetPosition().Y, 460, 16 * DeltaTime);
	controlsOverlaySlot->SetPosition(FVector2D(20, controlsLerp));

	int gameOverLerp = (bGameOver) ? FMath::Lerp(gameOverOverlaySlot->GetPosition().Y, 0, 16 * DeltaTime) : FMath::Lerp(gameOverOverlaySlot->GetPosition().Y, -1100, 16 * DeltaTime);
	gameOverOverlaySlot->SetPosition(FVector2D(0, gameOverLerp));

	int winLerp = (bWon) ? FMath::Lerp(winOverlaySlot->GetPosition().Y, 0, 16 * DeltaTime) : FMath::Lerp(winOverlaySlot->GetPosition().Y, -1100, 16 * DeltaTime);
	winOverlaySlot->SetPosition(FVector2D(0, winLerp));
	
	if (bDying) return;

	FRotator CurrentRotation = GetActorRotation();
	CurrentRotation.Pitch = 0;
	CurrentRotation.Roll = 0;
	SetActorRotation(CurrentRotation);

	if (bWorldIs2D)
	{
		FVector lerp = FMath::Lerp(GetActorLocation(), FVector(0, GetActorLocation().Y, GetActorLocation().Z), 5 * DeltaTime);
		SetActorLocation(lerp);

		followCamera->OrthoNearClipPlane = FMath::Lerp(followCamera->OrthoNearClipPlane, defaultCameraOrthoNearClipPlane, 10 * DeltaTime);
		followCamera->OrthoWidth = FMath::Lerp(followCamera->OrthoWidth, defaultCameraOrthoWidth, 10 * DeltaTime);
		followCamera->FieldOfView = FMath::Lerp(followCamera->FieldOfView, 125, 10 * DeltaTime);
		cameraBoom->TargetArmLength = FMath::Lerp(cameraBoom->TargetArmLength, 800.0f, 10 * DeltaTime);

		cartoonLerp = FMath::Lerp(cartoonLerp, 1.2f, 8 * DeltaTime);
		cartoonSong->SetVolumeMultiplier(cartoonLerp);
		//cartoonBackground->SetVolumeMultiplier(cartoonLerp);

		pirateLerp = FMath::Lerp(pirateLerp, 0.1f, 8 * DeltaTime);
		pirateSong->SetVolumeMultiplier(pirateLerp);
		pirateBackground->SetVolumeMultiplier(pirateLerp);
	}
	else
	{
		followCamera->OrthoNearClipPlane = FMath::Lerp(followCamera->OrthoNearClipPlane, 0, 10 * DeltaTime);
		followCamera->OrthoWidth = FMath::Lerp(followCamera->OrthoWidth, 500, 10 * DeltaTime);
		followCamera->FieldOfView = FMath::Lerp(followCamera->FieldOfView, 90, 10 * DeltaTime);
		cameraBoom->TargetArmLength = FMath::Lerp(cameraBoom->TargetArmLength, 300.0f, 10 * DeltaTime);

		cartoonLerp = FMath::Lerp(cartoonLerp, 0.1f, 8 * DeltaTime);
		cartoonSong->SetVolumeMultiplier(cartoonLerp);
		//cartoonBackground->SetVolumeMultiplier(cartoonLerp);

		pirateLerp = FMath::Lerp(pirateLerp, 1, 8 * DeltaTime);
		pirateSong->SetVolumeMultiplier(pirateLerp);
		pirateBackground->SetVolumeMultiplier(pirateLerp);
	}

	int pointsLerp = (bGainedPoints) ? FMath::Lerp(pointsOverlaySlot->GetPosition().Y, 180, 12 * DeltaTime) : FMath::Lerp(pointsOverlaySlot->GetPosition().Y, 200, 12 * DeltaTime);
	pointsOverlaySlot->SetPosition(FVector2D(150, pointsLerp));

	int timeLerp = (bTick) ? FMath::Lerp(timerOverlaySlot->GetPosition().X, -335, 12 * DeltaTime) : FMath::Lerp(timerOverlaySlot->GetPosition().X, -350, 12 * DeltaTime);
	timerOverlaySlot->SetPosition(FVector2D(timeLerp, 50));

}


void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &APlayerBase::MoveCharacter);
		enhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);
		enhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Started, this, &APlayerBase::Jump);
		enhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Started, this, &APlayerBase::fakeJump);
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

	bDying = true;

	if (bWorldIs2D)
	{
		PlayAnimMontage(M_Tran2D, 1, NAME_None);
	}
	else
	{
		PlayAnimMontage(M_Tran3D, 1, NAME_None);
	}

	Arig_Torres_AnimGameGameModeBase* customGameMode = Cast<Arig_Torres_AnimGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if(customGameMode)
	{
		customGameMode->switchWorld();
		bWorldIs2D = !bWorldIs2D;
		changeCameraState();
		changeRotationState();
		switchWorldAnim();
	}
}

void APlayerBase::fakeJump()
{
	USkeletalMeshComponent* skeletalMeshComponent = this->FindComponentByClass<USkeletalMeshComponent>();
	UAnimInstance* animInstance = skeletalMeshComponent->GetAnimInstance();
	UplayerAnimInstance* playerAnim = Cast<UplayerAnimInstance>(animInstance);
	if (playerAnim) playerAnim->setJumpActionBool(true);

	if (bWorldIs2D)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), blackwhiteJump, 1.0f, 1.0f, 0.0f);
	}
	else
	{
		UGameplayStatics::PlaySound2D(GetWorld(), colorJump, 1.0f, 1.0f, 0.0f);
	}

	GetWorld()->GetTimerManager().SetTimer(ChangeTimerHandle, this, &APlayerBase::finishFakeJump, 0.1f, false);
}

void APlayerBase::finishFakeJump()
{
	USkeletalMeshComponent* skeletalMeshComponent = this->FindComponentByClass<USkeletalMeshComponent>();
	UAnimInstance* animInstance = skeletalMeshComponent->GetAnimInstance();
	UplayerAnimInstance* playerAnim = Cast<UplayerAnimInstance>(animInstance);
	if (playerAnim) playerAnim->setJumpActionBool(false);
}

void APlayerBase::menu()
{
	if (bDying) return;


	bOpenMenu = !bOpenMenu;
	UGameplayStatics::SetGamePaused(GetWorld(), bOpenMenu);
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(bOpenMenu);

	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
	
	FInputModeGameOnly game;
	GetWorld()->GetFirstPlayerController()->SetInputMode(game);
}

void APlayerBase::changeCameraState()
{
	GetWorld()->GetTimerManager().SetTimer(ChangeTimerHandle, this, &APlayerBase::changeToOrthographic, 0.5f, false);
	if (bWorldIs2D)
	{
		if (livesRemaining >= 1) setImage(lifeIconBlackandWhite, playerUI->livesImage1);
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
}

void APlayerBase::changeToOrthographic()
{
	bDying = false;
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
	if (bWorldIs2D)
	{
		PlayAnimMontage(M_jump2D, 1, NAME_None);
	}
	else
	{
		PlayAnimMontage(M_jump3D, 1, NAME_None);
	}

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

	if (bWorldIs2D)
	{
		PlayAnimMontage(M_death2D, 1, NAME_None);
	}
	else
	{
		PlayAnimMontage(M_death3D, 1, NAME_None);
	}

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
	if (bWorldIs2D) 
	{
		UGameplayStatics::PlaySound2D(GetWorld(), blackwhiteVoidOut, 1.0f, 1.0f, 0.0f);
	}
	else 
	{
		UGameplayStatics::PlaySound2D(GetWorld(), colorVoidOut, 1.0f, 1.0f, 0.0f);
	}
	
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
		bDying = true;
		bGameOver = true;
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
		GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
		FInputModeGameOnly game;
		GetWorld()->GetFirstPlayerController()->SetInputMode(game);
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
	if (time <= 0) time = 0;
	playerUI->timerText->SetText(FText::FromString("Time: " + FString::FromInt(time)));
	GetWorld()->GetTimerManager().SetTimer(TimerTimerHandle, this, &APlayerBase::timerTick, 1.2f, false);
}

void APlayerBase::switchWorldAnim()
{
	USkeletalMeshComponent* skeletalMeshComponent = this->FindComponentByClass<USkeletalMeshComponent>();
	UAnimInstance* animInstance = skeletalMeshComponent->GetAnimInstance();
	UplayerAnimInstance* playerAnim = Cast<UplayerAnimInstance>(animInstance);
	if (playerAnim) playerAnim->setWorldis2DBool(bWorldIs2D);
}

void APlayerBase::wonGame()
{
	bDying = true;
	bWon = true;

	UGameplayStatics::SetGamePaused(GetWorld(), true);
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
	FInputModeGameOnly game;
	GetWorld()->GetFirstPlayerController()->SetInputMode(game);

	if (playerUI == nullptr) return;

	playerUI->pointsBonusText->SetText(FText::FromString("Points: " + FString::FromInt(points)));
	float timeBonusFloat = time / 2;
	int timeBonus = FMath::RoundToInt(timeBonusFloat);
	playerUI->timeBonusText->SetText(FText::FromString("Time Bonus: " + FString::FromInt(timeBonus)));
	playerUI->totalPoinstText->SetText(FText::FromString("Total Points: " + FString::FromInt(points + timeBonus)));
}

void APlayerBase::setCheckPoint(FVector newSpawnPoint)
{
	spawnPoint = newSpawnPoint;
}

void APlayerBase::respawnAtCheckpoint()
{
	menu();
	death();
}

void APlayerBase::restartLevel()
{
	menu();
	UGameplayStatics::OpenLevel(GetWorld(), "mainLevel");
}

void APlayerBase::toMainMenu()
{
	menu();
	UGameplayStatics::OpenLevel(GetWorld(), "mainMenu");
}

void APlayerBase::hideControls()
{
	bHideControls = !bHideControls;
	if (playerUI == nullptr) return;
	if (bHideControls) playerUI->hideControlsText->SetText(FText::FromString("Show Controls"));
	if (!bHideControls) playerUI->hideControlsText->SetText(FText::FromString("Hide Controls"));
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
