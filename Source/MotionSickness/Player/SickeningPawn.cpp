// Fill out your copyright notice in the Description page of Project Settings.

#include "MotionSickness.h"
#include "UMG.h"
#include "UI/TrialResponseWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#include "SickeningPawn.h"


// Sets default values
ASickeningPawn::ASickeningPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;
	Mesh->SetVisibility(false);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	Camera->SetupAttachment(Mesh);
	Camera->bLockToHmd = false;

	Window = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WINDOW"));
	Window->SetupAttachment(Camera);

	TrialResponseWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("RESPONSEWIDGET"));
	TrialResponseWidgetComponent->SetupAttachment(Camera);
	TrialResponseWidgetComponent->bAbsoluteLocation = false;
	TrialResponseWidgetComponent->bAbsoluteRotation = false;
	TrialResponseWidgetComponent->bAbsoluteScale = false;
	TrialResponseWidgetComponent->RelativeLocation = FVector(150.f, 0.f, 0.f);
	TrialResponseWidgetComponent->RelativeRotation = FRotator(0.f, 180.f, 0.f);
	TrialResponseWidgetComponent->RelativeScale3D = FVector(0.2f, 0.2f, 0.2f);

	//SessionFinishedWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SESSIONFINISHEDWIDGET"));
	//SessionFinishedWidgetComponent->SetupAttachment(Camera);
	//SessionFinishedWidgetComponent->bAbsoluteLocation = false;
	//SessionFinishedWidgetComponent->bAbsoluteRotation = false;
	//SessionFinishedWidgetComponent->bAbsoluteScale = false;
	//SessionFinishedWidgetComponent->RelativeLocation = FVector(150.f, 0.f, 0.f);
	//SessionFinishedWidgetComponent->RelativeRotation = FRotator(0.f, 180.f, 0.f);
	//SessionFinishedWidgetComponent->RelativeScale3D = FVector(0.2f, 0.2f, 0.2f);

	SickeningSpeed = 5.f;
}

// Called when the game starts or when spawned
void ASickeningPawn::BeginPlay()
{
	Super::BeginPlay();
	bUseWindow = Window->bVisible;
	if (TrialResponseWidgetClass)
	{
		TrialResponseWidget = CreateWidget<UTrialResponseWidget>(GetWorld()->GetFirstPlayerController(), TrialResponseWidgetClass);
		TrialResponseWidgetComponent->SetWidget(TrialResponseWidget);
	}
	TrialResponseWidgetComponent->SetVisibility(false);

	//if (SessionFinishedWidgetClass)
	//{
	//	SessionFinishedWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), SessionFinishedWidgetClass);
	//	SessionFinishedWidgetComponent->SetWidget(SessionFinishedWidget);
	//}
	//SessionFinishedWidgetComponent->SetVisibility(false);

	UIModeOff();
	if (TrialMode == ETrialMode::SpeedMultiplier)
	{
		TimesIndexTested.SetNum(SpeedMultiplierTestPool.Num());
	}
	else if (TrialMode == ETrialMode::DirectionChangeRate)
	{
		TimesIndexTested.SetNum(DirectionChangeFrequencyTestPool.Num());
	}

	FString SaveDirectory = FString("TrialResults/");
	FString FileName = FString("trial.csv");

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (PlatformFile.CreateDirectoryTree(*SaveDirectory))
	{
		OutputFilePath = SaveDirectory + "/" + FileName;
	}
	Results.Append("SpeedMultiplier,DirectionChangeRate,SicknessRating,UseWindow,UseX,UseY,UseZ,AverageFPS,TrialLength\n");
}

// Called every frame
void ASickeningPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsSickening)
	{
		if (SickeningTimer >= SickeningRotatorChangeSeconds)
		{
			ChooseNewSickeningDirection();
			SickeningTimer = 0.f;
		}
		AddControllerPitchInput(CurrentSickeningDirection.X * SickeningSpeed * DeltaTime);
		AddControllerYawInput(CurrentSickeningDirection.Y * SickeningSpeed * DeltaTime);
		AddControllerRollInput(CurrentSickeningDirection.Z * SickeningSpeed * DeltaTime);
		SetActorRotation(this->GetControlRotation());
		if (TrialState == ETrialState::InTrial && TrialTimer >= TrialTime)
		{
			OnPostTrial();
		}
	}
	if (TrialState == ETrialState::InTrial)
	{
		InTrialFPSPerTick.Add(1.f / DeltaTime);
	}
	TrialTimer += DeltaTime;
	SickeningTimer += DeltaTime;
}

// Called to bind functionality to input
void ASickeningPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	PlayerInputComponent->BindAction("ToggleSickening", IE_Pressed, this, &ASickeningPawn::ToggleSickening);
	PlayerInputComponent->BindAction("ToggleBlinders", IE_Pressed, this, &ASickeningPawn::ToggleWindow);
	PlayerInputComponent->BindAction("IncreaseSickenSpeed", IE_Pressed, this, &ASickeningPawn::IncreaseSickenSpeed);
	PlayerInputComponent->BindAction("DecreaseSickenSpeed", IE_Pressed, this, &ASickeningPawn::DecreaseSickenSpeed);
	PlayerInputComponent->BindAction("IncreaseTimerSpeed", IE_Pressed, this, &ASickeningPawn::IncreaseTimerSpeed);
	PlayerInputComponent->BindAction("DecreaseTimerSpeed", IE_Pressed, this, &ASickeningPawn::DecreaseTimerSpeed);
	PlayerInputComponent->BindAction("SetupTrial", IE_Pressed, this, &ASickeningPawn::OnPreTrial);
	PlayerInputComponent->BindAction("EnableX", IE_Pressed, this, &ASickeningPawn::EnableX);
	PlayerInputComponent->BindAction("EnableY", IE_Pressed, this, &ASickeningPawn::EnableY);
	PlayerInputComponent->BindAction("EnableZ", IE_Pressed, this, &ASickeningPawn::EnableZ);
	PlayerInputComponent->BindAction("IncreaseFOV", IE_Pressed, this, &ASickeningPawn::IncreaseFOV);
	PlayerInputComponent->BindAction("DecreaseFOV", IE_Pressed, this, &ASickeningPawn::DecreaseFOV);
	PlayerInputComponent->BindAction("ResetCamera", IE_Pressed, this, &ASickeningPawn::ResetCamera);
	PlayerInputComponent->BindAction("IncrementSicknessRating", IE_Pressed, this, &ASickeningPawn::IncrementSicknessRating);
	PlayerInputComponent->BindAction("DecrementSicknessRating", IE_Pressed, this, &ASickeningPawn::DecrementSicknessRating);
	PlayerInputComponent->BindAction("AcceptTrialInput", IE_Pressed, this, &ASickeningPawn::AcceptTrialInput);
}

void ASickeningPawn::ToggleSickening()
{
	bIsSickening = !bIsSickening;
	SickeningTimer = 999.f;
}

void ASickeningPawn::ToggleWindow()
{
	bUseWindow = !bUseWindow;
	Window->SetVisibility(bUseWindow);
}

void ASickeningPawn::IncreaseSickenSpeed()
{
	SickeningSpeed += 1.f;
	SickeningTimer = 999.f;
}

void ASickeningPawn::DecreaseSickenSpeed()
{
	SickeningSpeed -= 1.f;
	SickeningTimer = 999.f;
}

void ASickeningPawn::IncreaseTimerSpeed()
{
	SickeningRotatorChangeSeconds += 1.f;
	SickeningTimer = 999.f;
}

void ASickeningPawn::DecreaseTimerSpeed()
{
	SickeningRotatorChangeSeconds -= 1.f;
	SickeningTimer = 999.f;
}

void ASickeningPawn::EnableX()
{
	bSickenX = !bSickenX;
	SickeningTimer = 999.f;
}

void ASickeningPawn::EnableY()
{
	bSickenY = !bSickenY;
	SickeningTimer = 999.f;
}

void ASickeningPawn::EnableZ()
{
	bSickenZ = !bSickenZ;
	SickeningTimer = 999.f;
}

void ASickeningPawn::IncreaseFOV()
{
	Camera->SetFieldOfView(Camera->FieldOfView + 5.0f);
}

void ASickeningPawn::DecreaseFOV()
{
	Camera->SetFieldOfView(Camera->FieldOfView - 5.0f);
}

void ASickeningPawn::OnPreTrial()
{
	if (CheckFinishedTesting())
	{
		OnFinishedAllTrials();
	}
	else
	{
		int32 index;
		if (TrialMode == ETrialMode::SpeedMultiplier)
		{
			index = (int32)FMath::RandRange(0, SpeedMultiplierTestPool.Num() - 1);
			while (TimesIndexTested[index] >= NumberTimesToTestParameter)
			{
				index = (int32)FMath::RandRange(0, SpeedMultiplierTestPool.Num() - 1);
			}
			SickeningSpeed = SpeedMultiplierTestPool[index];
			SickeningRotatorChangeSeconds = 2.f;
			TimesIndexTested[index]++;
		}
		else if (TrialMode == ETrialMode::DirectionChangeRate)
		{
			index = (int32)FMath::RandRange(0, DirectionChangeFrequencyTestPool.Num() - 1);
			while (TimesIndexTested[index] >= NumberTimesToTestParameter)
			{
				index = (int32)FMath::RandRange(0, SpeedMultiplierTestPool.Num() - 1);
			}
			SickeningRotatorChangeSeconds = DirectionChangeFrequencyTestPool[index];
			SickeningSpeed = 4.f;
			TimesIndexTested[index]++;
		}
	}
	InTrialFPSPerTick.Empty();
	GeneratedVectorListCurrentIndex = 0;
	OnInTrial();
}

void ASickeningPawn::OnInTrial()
{
	ResetCamera();
	bIsSickening = true;
	SickeningTimer = 999.f;
	TrialTimer = 0.f;
	UIModeOff();
	if (bUseWindow)
		Window->SetVisibility(true);
	TrialResponseWidgetComponent->SetVisibility(false);
	TrialState = ETrialState::InTrial;
}

void ASickeningPawn::OnPostTrial()
{
	SickeningTimer = 0.f;
	TrialTimer = 0.f;
	bIsSickening = false;
	UIModeOn();
	TrialResponseWidgetComponent->SetVisibility(true);
	TrialState = ETrialState::PostTrial;
}

void ASickeningPawn::OnFinishedAllTrials()
{
	TrialState = ETrialState::FinishedAllTrials;
}

void ASickeningPawn::ResetCamera()
{
	GetController()->SetControlRotation(FRotator::ZeroRotator);
	SetActorRotation(GetControlRotation());
}

void ASickeningPawn::UIModeOn()
{
	auto ControllerRef = GetWorld()->GetFirstPlayerController();
	ControllerRef->bShowMouseCursor = true;
	ControllerRef->bEnableClickEvents = true;
	ControllerRef->bEnableMouseOverEvents = true;
	Window->SetVisibility(false);
}

void ASickeningPawn::UIModeOff()
{
	auto ControllerRef = GetWorld()->GetFirstPlayerController();
	ControllerRef->bShowMouseCursor = false;
	ControllerRef->bEnableClickEvents = false;
	ControllerRef->bEnableMouseOverEvents = false;
	if (bUseWindow)
		Window->SetVisibility(true);
}

void ASickeningPawn::IncrementSicknessRating()
{
	TrialResponseWidget->SicknessRating++;
	if (TrialResponseWidget->SicknessRating > 10)
		TrialResponseWidget->SicknessRating = 10;
}

void ASickeningPawn::DecrementSicknessRating()
{
	TrialResponseWidget->SicknessRating--;
	if (TrialResponseWidget->SicknessRating < 1)
		TrialResponseWidget->SicknessRating = 1;
}

void ASickeningPawn::AcceptTrialInput()
{
	if (TrialState == ETrialState::PostTrial)
	{
		float sum = 0.f;
		for (float f : InTrialFPSPerTick)
			sum += f;
		float AverageFPS = sum / InTrialFPSPerTick.Num();
		FString Result = FString::Printf(TEXT("%f,%f,%d,%d,%d,%d,%d,%f,%f\n"), SickeningSpeed, SickeningRotatorChangeSeconds, TrialResponseWidget->SicknessRating, 
			bUseWindow, bSickenX, bSickenY, bSickenZ, AverageFPS, TrialTime);
		Results.Append(Result);
		FFileHelper::SaveStringToFile(Results, *OutputFilePath);
		TrialResponseWidget->SicknessRating = 5;
		UIModeOff();
		TrialResponseWidgetComponent->SetVisibility(false);
		TrialState = ETrialState::OutOfTrial;
	}
}

void ASickeningPawn::SetupTrialResultsFile()
{

}

bool ASickeningPawn::CheckFinishedTesting()
{
	bool bFinishedTesting = true;
	for (int TimesTested : TimesIndexTested)
	{
		if (TimesTested < NumberTimesToTestParameter)
		{
			bFinishedTesting = false;
		}
	}
	return bFinishedTesting;
}

void ASickeningPawn::ChooseNewSickeningDirection()
{
	if (bUseGeneratedVectorList)
	{
		CurrentSickeningDirection = GeneratedVectorList[GeneratedVectorListCurrentIndex];
		GeneratedVectorListCurrentIndex++;
	}
	else
	{
		CurrentSickeningDirection = UKismetMathLibrary::RandomUnitVector();
	}
	if (!bSickenX)
	{
		CurrentSickeningDirection.X = 0;
	}
	if (!bSickenY)
	{
		CurrentSickeningDirection.Y = 0;
	}
	if (!bSickenZ)
	{
		CurrentSickeningDirection.Z = 0;
	}

	if (CurrentSickeningDirection.Size() > 0 && (bSickenX || bSickenY || bSickenZ))
	{
		CurrentSickeningDirection /= CurrentSickeningDirection.Size();
	}
	else
	{
		ChooseNewSickeningDirection();
	}
}
