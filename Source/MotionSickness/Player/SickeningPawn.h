// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "SickeningPawn.generated.h"

UCLASS()
class MOTIONSICKNESS_API ASickeningPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASickeningPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Window;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidgetComponent* TrialResponseWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTrialResponseWidget* TrialResponseWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UTrialResponseWidget> TrialResponseWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidgetComponent* SessionFinishedWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UUserWidget* SessionFinishedWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UUserWidget> SessionFinishedWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsSickening;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector CurrentSickeningDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SickeningSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bSickenX = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bSickenY = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bSickenZ = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SickeningRotatorChangeSeconds = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SickeningTimer = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsInTrial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TrialTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TrialTimer = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<float> SpeedMultiplierTestPool = { 1 / 8.f, 1 / 4.f, 1 / 2.f, 1, 2, 4, 8, 16, 32 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<float> DirectionChangeFrequencyTestPool = { 1 / 32.f, 1 / 16.f, 1 / 8.f, 1 / 4.f, 1, 2, 4, 8, 16, 32 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int> TimesIndexTested;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bTestingOnlySpeedMultiplier = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bTestingOnlyDirectionChangeRate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bShowTrialResponseWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bNeedsToUpdateUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int NUM_TIMES_TO_TEST = 5;

	void ToggleSickening();

	void ToggleWindow();

	void IncreaseSickenSpeed();

	void DecreaseSickenSpeed();

	void IncreaseTimerSpeed();

	void DecreaseTimerSpeed();

	void EnableX();

	void EnableY();

	void EnableZ();

	void IncreaseFOV();

	void DecreaseFOV();

	void ResetCamera();

	void OnPreTrial();

	void OnPostTrial();

	void UIModeOn();

	void UIModeOff();

	void IncrementSicknessRating();

	void DecrementSicknessRating();

	void AcceptTrialInput();

	void SetupTrialResultsFile();

};
