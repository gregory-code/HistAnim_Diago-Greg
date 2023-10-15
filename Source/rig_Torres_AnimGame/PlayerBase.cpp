
#include "PlayerBase.h"
//
#include "GameFramework/CharacterMovementComponent.h" // For GetCharacterMovement
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "rig_Torres_AnimGameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
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
	cameraBoom->TargetArmLength = 300.0f;
	cameraBoom->bUsePawnControlRotation = true; // Let's this control the camera rotation //  can also set this booleon in the blueprint editor

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	followCamera->bUsePawnControlRotation = false; //gives this control of the cameraBoom
}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	
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

	const FRotator rotation = Controller->GetControlRotation();
	const FRotator yawRotation(0.0f, rotation.Yaw, 0.0f);

	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(forwardDirection, moveVector.Y);
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
	}
}