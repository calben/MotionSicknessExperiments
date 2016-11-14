// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "SickeningPawn.generated.h"

UENUM(BlueprintType)
enum class ETrialState : uint8
{
	PreTrial,
	InTrial,
	PostTrial,
	OutOfTrial,
	FinishedAllTrials
};

UENUM(BlueprintType)
enum class ETrialMode : uint8
{
	SpeedMultiplier,
	DirectionChangeRate
};

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

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	class UWidgetComponent* SessionFinishedWidgetComponent;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	class UUserWidget* SessionFinishedWidget;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	TSubclassOf<class UUserWidget> SessionFinishedWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsSickening;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETrialState TrialState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETrialMode TrialMode = ETrialMode::SpeedMultiplier;

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
		float SickeningRotatorChangeSeconds = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SickeningTimer = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TrialTime = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TrialTimer = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<float> SpeedMultiplierTestPool = { 1 / 4.f, 1 / 2.f, 1, 2, 4, 8, 16, 32 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<float> DirectionChangeFrequencyTestPool = { 1 / 32.f, 1 / 16.f, 1 / 8.f, 1 / 4.f, 1, 2, 4, 8, 16, 32 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int> TimesIndexTested;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<float> InTrialFPSPerTick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bShowTrialResponseWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int NumberTimesToTestParameter = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bUseWindow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bUseGeneratedVectorList = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int GeneratedVectorListCurrentIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString OutputFilePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Results;

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

	void OnInTrial();

	void OnPostTrial();

	void OnFinishedAllTrials();

	void UIModeOn();

	void UIModeOff();

	void IncrementSicknessRating();

	void DecrementSicknessRating();

	void AcceptTrialInput();

	void SetupTrialResultsFile();

	bool CheckFinishedTesting();

	void ChooseNewSickeningDirection();

	TArray<FVector> GeneratedVectorList = {
		FVector(-0.058, 0.878, -0.475),
		FVector(-0.859, 0.498, -0.122),
		FVector(0.905, 0.027, -0.425),
		FVector(0.784, 0.616, 0.076),
		FVector(0.337, -0.940, 0.061),
		FVector(-0.833, 0.268, 0.485),
		FVector(-0.443, 0.530, -0.723),
		FVector(0.951, -0.017, 0.307),
		FVector(0.230, 0.851, -0.473),
		FVector(0.591, -0.495, 0.637),
		FVector(0.715, -0.655, 0.244),
		FVector(0.395, 0.008, -0.919),
		FVector(-0.289, 0.870, 0.400),
		FVector(0.552, 0.794, 0.255),
		FVector(-0.974, 0.210, -0.084),
		FVector(-0.369, 0.816, 0.444),
		FVector(0.649, 0.389, -0.653),
		FVector(-0.521, 0.449, -0.726),
		FVector(-0.866, 0.496, 0.057),
		FVector(0.640, -0.553, -0.533),
		FVector(-0.418, 0.732, 0.538),
		FVector(0.110, -0.487, 0.866),
		FVector(0.828, -0.373, 0.419),
		FVector(-0.963, -0.219, 0.159),
		FVector(-0.804, -0.593, 0.054),
		FVector(-0.029, 0.350, 0.936),
		FVector(0.544, -0.797, -0.262),
		FVector(-0.435, -0.866, 0.248),
		FVector(0.615, 0.468, -0.634),
		FVector(0.361, -0.902, 0.236),
		FVector(-0.705, 0.707, -0.048),
		FVector(0.120, -0.135, 0.984),
		FVector(0.913, -0.142, -0.383),
		FVector(-0.900, 0.436, -0.003),
		FVector(0.094, 0.030, 0.995),
		FVector(-0.428, -0.889, -0.161),
		FVector(0.841, 0.537, 0.070),
		FVector(-0.651, -0.682, -0.333),
		FVector(0.033, 0.054, 0.998),
		FVector(0.778, 0.626, 0.054),
		FVector(-0.669, -0.604, -0.432),
		FVector(0.795, -0.002, -0.607),
		FVector(0.384, -0.922, 0.037),
		FVector(-0.321, -0.064, 0.945),
		FVector(0.700, -0.449, 0.556),
		FVector(0.041, -0.827, -0.560),
		FVector(0.568, 0.763, -0.309),
		FVector(-0.155, 0.844, -0.513),
		FVector(0.742, -0.616, -0.264),
		FVector(-0.651, 0.332, 0.682),
		FVector(-0.752, 0.610, -0.250),
		FVector(0.610, 0.301, 0.733),
		FVector(-0.232, -0.862, 0.451),
		FVector(0.642, -0.267, -0.718),
		FVector(0.444, -0.275, -0.853),
		FVector(-0.414, 0.552, 0.724),
		FVector(0.386, 0.286, -0.877),
		FVector(-0.928, 0.344, -0.145),
		FVector(-0.569, 0.366, -0.737),
		FVector(0.121, 0.726, -0.677),
		FVector(-0.527, -0.021, -0.849),
		FVector(0.438, -0.873, -0.215),
		FVector(0.360, -0.883, -0.300),
		FVector(-0.046, 0.995, -0.092),
		FVector(-0.083, -0.970, 0.230),
		FVector(0.532, 0.784, 0.320),
		FVector(0.134, 0.988, 0.077),
		FVector(-0.385, 0.629, 0.676),
		FVector(-0.287, -0.905, 0.314),
		FVector(0.308, -0.633, -0.710),
		FVector(-0.625, 0.778, -0.063),
		FVector(0.181, 0.800, 0.571),
		FVector(-0.436, -0.874, 0.216),
		FVector(-0.288, 0.863, 0.416),
		FVector(0.603, -0.658, 0.452),
		FVector(0.233, 0.870, -0.434),
		FVector(0.843, 0.329, 0.426),
		FVector(-0.801, -0.491, -0.341),
		FVector(-0.345, 0.931, -0.120),
		FVector(0.501, -0.865, 0.023),
		FVector(-0.978, 0.032, -0.204),
		FVector(-0.729, 0.115, -0.675),
		FVector(0.502, 0.461, -0.732),
		FVector(0.777, 0.031, -0.629),
		FVector(0.225, 0.892, -0.393),
		FVector(-0.072, -0.239, -0.968),
		FVector(-0.774, 0.071, 0.629),
		FVector(-0.101, 0.970, 0.219),
		FVector(-0.454, 0.843, 0.287),
		FVector(0.682, -0.223, -0.697),
		FVector(-0.276, -0.735, 0.619),
		FVector(0.942, 0.068, 0.328),
		FVector(0.565, 0.590, -0.576),
		FVector(-0.846, 0.529, 0.070),
		FVector(-0.835, -0.334, 0.437),
		FVector(0.909, -0.162, 0.384),
		FVector(0.325, 0.932, 0.161),
		FVector(-0.564, 0.560, 0.607),
		FVector(-0.371, -0.467, 0.802),
		FVector(0.067, -0.410, 0.910)
	};
};

