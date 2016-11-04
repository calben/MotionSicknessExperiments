// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "TrialResponseWidget.generated.h"

/**
 *
 */
UCLASS()
class MOTIONSICKNESS_API UTrialResponseWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int SicknessRating = 5;

};
