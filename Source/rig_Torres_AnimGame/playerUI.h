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
		UOverlay* UIOverlay;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UImage* coinImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "Widget Component")
		UTextBlock* coinText;

};
