// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "playerUI.generated.h"

/**
 * 
 */
UCLASS()
class RIG_TORRES_ANIMGAME_API UplayerUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UOverlay* pointsOverlay;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UImage* coinImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UTextBlock* coinText;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UOverlay* livesOverlay;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UOverlay* gameOverOverlay;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UImage* livesImage1;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UImage* livesImage2;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UImage* livesImage3;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UOverlay* respawnOverlay;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UImage* respawnImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UOverlay* timerOverlay;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UTextBlock* timerText;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UOverlay* controlsOverlay;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UOverlay* menuPauseOverlay;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UOverlay* winOverlay;

		UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UTextBlock* pointsBonusText;

		UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UTextBlock* timeBonusText;

		UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UTextBlock* totalPoinstText;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UTextBlock* hideControlsText;
};
