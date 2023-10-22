
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputAction.h"
#include "Components/InputComponent.h"
//
#include "PlayerBase.generated.h"

UCLASS()
class RIG_TORRES_ANIMGAME_API APlayerBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values
	APlayerBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_jump;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		USpringArmComponent* cameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* followCamera;

	//Inputs
	UPROPERTY(EditAnywhere, Category = "Input")
		UInputMappingContext* mappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* moveAction;
	void MoveCharacter(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* lookAction;
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* jumpAction;
	//This one is handled automatically using the character controller jump

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* switchAction;
	void switchWorld();
	void changeCameraState();
	void changeRotationState();
	void bounceOffEnemy();
	bool bWorldIs2D	{false};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
