
#include "PlayerBase.h"
//
#include "GameFramework/CharacterMovementComponent.h" // For GetCharacterMovement
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "rig_Torres_AnimGameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
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

	points = 0;

	changeCameraState();
	
	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(mappingContext, 0);
		}
	}
	
	if (!playerUI) return;
	playerUI->AddToViewport();

	playerUI->coinText->SetText(FText::FromString("Points: " + FString::FromInt(points)));

	menuOverlaySlot = Cast<UCanvasPanelSlot>(playerUI->menuOverlay->Slot);
	pointsOverlaySlot = Cast<UCanvasPanelSlot>(playerUI->pointsOverlay->Slot);
}

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWorldIs2D)
	{
		FVector lerp = FMath::Lerp(GetActorLocation(), FVector(0, GetActorLocation().Y, GetActorLocation().Z), 5 * DeltaTime);
		SetActorLocation(lerp);
	}

	if (pointsOverlaySlot == nullptr) return;

	int pointsLerp = (bGainedPoints) ? FMath::Lerp(pointsOverlaySlot->GetPosition().Y, 60, 12 * DeltaTime) : FMath::Lerp(pointsOverlaySlot->GetPosition().Y, 100, 12 * DeltaTime);
	pointsOverlaySlot->SetPosition(FVector2D(100, pointsLerp));
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
	}
}

void APlayerBase::MoveCharacter(const FInputActionValue& Value)
{
	const FVector2D moveVector = Value.Get<FVector2D>();

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("X: %f"), moveVector.X));

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
	Arig_Torres_AnimGameGameModeBase* customGameMode = Cast<Arig_Torres_AnimGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if(customGameMode)
	{
		customGameMode->switchWorld();
		bWorldIs2D = !bWorldIs2D;
		changeCameraState();
		changeRotationState();
	}
}

void APlayerBase::changeCameraState()
{
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
	float jumpVelocity = 1200;

	LaunchCharacter(jumpDirection * jumpVelocity, false, false);
}

void APlayerBase::addPoints(int pointsToAdd)
{
	points += pointsToAdd;
	playerUI->coinText->SetText(FText::FromString("Points: " + FString::FromInt(points)));

	bGainedPoints = true;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APlayerBase::finishedAddingPoints, 0.2f, false);
}

void APlayerBase::finishedAddingPoints()
{
	bGainedPoints = false;
}
