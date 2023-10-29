// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "playerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RIG_TORRES_ANIMGAME_API UplayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Custom Animation")
	void setWorldis2DBool(bool state);

	UFUNCTION(BlueprintCallable, Category = "Custom Animation")
	void setJumpActionBool(bool state);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Animation")
		bool bWorldIs2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Animation")
		bool bJumpAction;
};
