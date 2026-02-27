// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/Hall/HallPawn.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SceneComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "MyShootingGame/Components/CameraBreathComponent.h"

// Sets default values
AHallPawn::AHallPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
		
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(SceneRoot);
	SpringArmComp->TargetArmLength = 20.f;
	SpringArmComp->bEnableCameraRotationLag = true;
	SpringArmComp->CameraRotationLagSpeed = 1.f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	CameraBreathComp = CreateDefaultSubobject<UCameraBreathComponent>(TEXT("CameraBreathComp"));
	if (CameraBreathComp)
	{
		CameraBreathComp->TargetComponent = SpringArmComp;
	}


}

// Called when the game starts or when spawned
void AHallPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHallPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHallPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

