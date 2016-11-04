// Fill out your copyright notice in the Description page of Project Settings.

#include "MotionSickness.h"
#include "UMG.h"
#include "UI/TrialResponseWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

	SessionFinishedWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("RESPONSEWIDGET"));
	SessionFinishedWidgetComponent->SetupAttachment(Camera);
	SessionFinishedWidgetComponent->bAbsoluteLocation = false;
	SessionFinishedWidgetComponent->bAbsoluteRotation = false;
	SessionFinishedWidgetComponent->bAbsoluteScale = false;

	SickeningSpeed = 5.f;
}

// Called when the game starts or when spawned
void ASickeningPawn::BeginPlay()
{
	Super::BeginPlay();

	if (TrialResponseWidgetClass)
	{
		TrialResponseWidget = CreateWidget<UTrialResponseWidget>(GetWorld()->GetFirstPlayerController(), TrialResponseWidgetClass);
		TrialResponseWidgetComponent->SetWidget(TrialResponseWidget);
	}
	TrialResponseWidgetComponent->SetVisibility(false);

	if (SessionFinishedWidgetClass)
	{
		SessionFinishedWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), SessionFinishedWidgetClass);
		SessionFinishedWidgetComponent->SetWidget(SessionFinishedWidget);
	}
	TrialResponseWidgetComponent->SetVisibility(false);

	UIModeOff();
	if (bTestingOnlySpeedMultiplier)
	{
		TimesIndexTested.SetNum(SpeedMultiplierTestPool.Num());
	}
	else
	{
		TimesIndexTested.SetNum(DirectionChangeFrequencyTestPool.Num());
	}
}

// Called every frame
void ASickeningPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsSickening)
	{
		if (SickeningTimer >= SickeningRotatorChangeSeconds)
		{
			CurrentSickeningDirection = UKismetMathLibrary::RandomUnitVector();
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
			if (CurrentSickeningDirection.Size() > 0)
			{
				CurrentSickeningDirection /= CurrentSickeningDirection.Size();
			}
			SickeningTimer = 0.f;
		}
		AddControllerPitchInput(CurrentSickeningDirection.X * SickeningSpeed * DeltaTime);
		AddControllerYawInput(CurrentSickeningDirection.Y * SickeningSpeed * DeltaTime);
		AddControllerRollInput(CurrentSickeningDirection.Z * SickeningSpeed * DeltaTime);
		SetActorRotation(this->GetControlRotation());
		if (bIsInTrial && TrialTimer >= TrialTime)
		{
			OnPostTrial();
		}
	}
	TrialTimer += DeltaTime;
	SickeningTimer += DeltaTime;
}

// Called to bind functionality to input
void ASickeningPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("ToggleSickening", IE_Pressed, this, &ASickeningPawn::ToggleSickening);
	InputComponent->BindAction("ToggleBlinders", IE_Pressed, this, &ASickeningPawn::ToggleWindow);
	InputComponent->BindAction("IncreaseSickenSpeed", IE_Pressed, this, &ASickeningPawn::IncreaseSickenSpeed);
	InputComponent->BindAction("DecreaseSickenSpeed", IE_Pressed, this, &ASickeningPawn::DecreaseSickenSpeed);
	InputComponent->BindAction("IncreaseTimerSpeed", IE_Pressed, this, &ASickeningPawn::IncreaseTimerSpeed);
	InputComponent->BindAction("DecreaseTimerSpeed", IE_Pressed, this, &ASickeningPawn::DecreaseTimerSpeed);
	InputComponent->BindAction("SetupTrial", IE_Pressed, this, &ASickeningPawn::OnPreTrial);
	InputComponent->BindAction("EnableX", IE_Pressed, this, &ASickeningPawn::EnableX);
	InputComponent->BindAction("EnableY", IE_Pressed, this, &ASickeningPawn::EnableY);
	InputComponent->BindAction("EnableZ", IE_Pressed, this, &ASickeningPawn::EnableZ);
	InputComponent->BindAction("IncreaseFOV", IE_Pressed, this, &ASickeningPawn::IncreaseFOV);
	InputComponent->BindAction("DecreaseFOV", IE_Pressed, this, &ASickeningPawn::DecreaseFOV);
	InputComponent->BindAction("ResetCamera", IE_Pressed, this, &ASickeningPawn::ResetCamera);
	InputComponent->BindAction("IncrementSicknessRating", IE_Pressed, this, &ASickeningPawn::IncrementSicknessRating);
	InputComponent->BindAction("DecrementSicknessRating", IE_Pressed, this, &ASickeningPawn::DecrementSicknessRating);
	InputComponent->BindAction("AcceptTrialInput", IE_Pressed, this, &ASickeningPawn::AcceptTrialInput);
}

void ASickeningPawn::ToggleSickening()
{
	bIsSickening = !bIsSickening;
	SickeningTimer = 999.f;
}

void ASickeningPawn::ToggleWindow()
{
	Window->SetVisibility(!Window->bVisible);
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
	bool bFinishedTesting = true;
	for (int TimesTested : TimesIndexTested)
	{
		if (TimesTested < NUM_TIMES_TO_TEST)
		{
			bFinishedTesting = false;
		}
	}

	if (bFinishedTesting)
	{
		return;
	}

	int32 index;
	if(bTestingOnlySpeedMultiplier)
	{
		index = (int32)FMath::RandRange(0, SpeedMultiplierTestPool.Num() - 1);
		while (TimesIndexTested[index] >= NUM_TIMES_TO_TEST)
		{
			index = (int32)FMath::RandRange(0, SpeedMultiplierTestPool.Num() - 1);
		}
		SickeningSpeed = SpeedMultiplierTestPool[index];
		SickeningRotatorChangeSeconds = 5.f;
		TimesIndexTested[index]++;
	}
	else
	{
		index = (int32)FMath::RandRange(0, DirectionChangeFrequencyTestPool.Num() - 1);
		while (TimesIndexTested[index] >= NUM_TIMES_TO_TEST)
		{
			index = (int32)FMath::RandRange(0, SpeedMultiplierTestPool.Num() - 1);
		}
		SickeningRotatorChangeSeconds = DirectionChangeFrequencyTestPool[index];
		SickeningSpeed = 4.f;
		TimesIndexTested[index]++;
	}
	ResetCamera();
	SickeningTimer = 0.f;
	TrialTimer = 0.f;
	bIsInTrial = true;
	bIsSickening = true;
	TrialResponseWidgetComponent->SetVisibility(false);
	bNeedsToUpdateUI = true;
}

void ASickeningPawn::OnPostTrial()
{
	bIsSickening = false;
	SickeningTimer = 0.f;
	TrialTimer = 0.f;
	TrialResponseWidgetComponent->SetVisibility(true);
	UIModeOn();
	bNeedsToUpdateUI = true;
}

void ASickeningPawn::ResetCamera()
{
	GetController()->SetControlRotation(FRotator::ZeroRotator);
	SetActorRotation(GetControlRotation());
}

void ASickeningPawn::UIModeOn()
{
	auto Controller = GetWorld()->GetFirstPlayerController();
	Controller->bShowMouseCursor = true;
	Controller->bEnableClickEvents = true;
	Controller->bEnableMouseOverEvents = true;
	Window->SetVisibility(false);
}

void ASickeningPawn::UIModeOff()
{
	auto Controller = GetWorld()->GetFirstPlayerController();
	Controller->bShowMouseCursor = false;
	Controller->bEnableClickEvents = false;
	Controller->bEnableMouseOverEvents = false;
	Window->SetVisibility(true);
}

void ASickeningPawn::IncrementSicknessRating()
{
	TrialResponseWidget->SicknessRating++;
}

void ASickeningPawn::DecrementSicknessRating()
{
	TrialResponseWidget->SicknessRating--;
}

void ASickeningPawn::AcceptTrialInput()
{
	if (TrialResponseWidgetComponent->bVisible == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("{ SpeedMultiplier: %f, DirectionChangeRate: %f, SicknessRating: %d}"), SickeningSpeed, SickeningRotatorChangeSeconds, TrialResponseWidget->SicknessRating);
		TrialResponseWidget->SicknessRating = 5;
		UIModeOff();
		TrialResponseWidgetComponent->SetVisibility(false);
	}
}

void ASickeningPawn::SetupTrialResultsFile()
{
	//FString SaveDirectory = FString("TrialResults/");
	//FString FileName = FString("trial.sav");
	//FString TextToSave = FString("Lorem ipsum");
	//bool AllowOverwriting = true;

	//IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	//if (PlatformFile.CreateDirectoryTree(*SaveDirectory))
	//{
	//	FString AbsoluteFilePath = SaveDirectory + "/" + FileName;
	//	if (AllowOverwriting || !PlatformFile::FileExists(*AbsoluteFilePath))
	//	{
	//		FFileHelper::SaveStringToFile(TextToSave, *AbsoluteFilePath);
	//	}
	//}
}

