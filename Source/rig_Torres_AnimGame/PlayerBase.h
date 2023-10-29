
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputAction.h"
#include "playerUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/StaticMeshComponent.h"
#include "playerAnimInstance.h"
#include "Components/Widget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/Texture2D.h"
#include "Components/Overlayslot.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		UplayerUI* playerUI;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
		UCanvasPanelSlot* pointsOverlaySlot;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
		UCanvasPanelSlot* timerOverlaySlot;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
		UCanvasPanelSlot* respawnOverlaySlot;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
		UCanvasPanelSlot* menuPauseOverlaySlot;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
		UCanvasPanelSlot* controlsOverlaySlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_jump2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_jump3D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_death2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* M_death3D;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		USpringArmComponent* cameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* followCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		USceneCaptureComponent2D* sceneCaptureComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UStaticMeshComponent* planeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
		FVector spawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PNGs")
		UTexture2D* lifeIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PNGs")
		UTexture2D* lifeIconBlackandWhite;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PNGs")
		UTexture2D* transparent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PNGs")
		UTexture2D* foodIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PNGs")
		UTexture2D* foodIconBlackandWhite;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PNGs")
		UTexture2D* respawnTransition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PNGs")
		UTexture2D* respawnBlackandWhiteTransition;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* menuAction;
	//This one is handled automatically using the character controller jump

		UButton* respawnCheckpointButton;
		UButton* restartLevelButton;
		UButton* hideControlsButton;
		UButton* toMainMenuButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* switchAction;

	void switchWorld();
	void menu();
	void changeCameraState();
	void changeRotationState();
	void bounceOffEnemy();
	void addPoints(int pointsToAdd);
	void finishedAddingPoints();
	void death();
	void respawn();
	void hideScreen();
	void finishRespawn();
	void changeToOrthographic();
	void loseLife();
	void timerTick();
	void switchWorldAnim();
	void setCheckPoint(FVector newSpawnPoint);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void respawnAtCheckpoint();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void restartLevel();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void toMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void hideControls();


	void setImage(UTexture2D* desiredTexture, UImage* ImageToSet);

	bool bWorldIs2D	{false};
	bool bRespawnTransition{ false };
	bool bExitScreenRight{ false };
	bool bDying{ false };
	bool bTick{ false };
	bool bOpenMenu{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerControls")
	bool bHideControls;

	int livesRemaining{ 3 };

	int time{ 200 };
	int points;

	int defaultCameraOrthoWidth;
	int defaultCameraOrthoNearClipPlane;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle PointsTimerHandle;
	FTimerHandle TimerTimerHandle;
	FTimerHandle DeathTimerHandle;
	FTimerHandle ChangeTimerHandle;
	bool bGainedPoints {false};

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
