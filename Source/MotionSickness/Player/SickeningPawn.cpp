// Fill out your copyright notice in the Description page of Project Settings.

#include "MotionSickness.h"
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

	TrialResponseWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("RESPONSEWIDGET"));
	TrialResponseWidget->SetupAttachment(Camera);

	SickeningSpeed = 5.f;
}

// Called when the game starts or when spawned
void ASickeningPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASickeningPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
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
	int32 index_a = (int32)FMath::RandRange(0, SpeedMultiplierTestPool.Num() - 1);
	int32 index_b = (int32)FMath::RandRange(0, DirectionChangeFrequency.Num() - 1);
	SickeningSpeed = SpeedMultiplierTestPool[index_a];
	SickeningRotatorChangeSeconds = DirectionChangeFrequency[index_b];
	ResetCamera();
	SickeningTimer = 0.f;
	TrialTimer = 0.f;
	bIsInTrial = true;
	bIsSickening = true;
	TrialResponseWidget->SetVisibility(false);
	bNeedsToUpdateUI = true;
}

void ASickeningPawn::OnPostTrial()
{
	bIsSickening = false;
	SickeningTimer = 0.f;
	TrialTimer = 0.f;
	TrialResponseWidget->SetVisibility(true);
	bNeedsToUpdateUI = true;
}

void ASickeningPawn::ResetCamera()
{
	GetController()->SetControlRotation(FRotator::ZeroRotator);
	SetActorRotation(GetControlRotation());
}
