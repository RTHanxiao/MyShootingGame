// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/Character/HallPrevChar.h"
#include "MyShootingGame/Logic/MSG_EventManager.h"


// Sets default values
AHallPrevChar::AHallPrevChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//UMSG_EventManager::GetEventManagerInstance()->ChangeHallCharacterMeshDelegate.BindUObject(this, &AHallPrevChar::SetSkeletalMesh);


}

// Called when the game starts or when spawned
void AHallPrevChar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHallPrevChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHallPrevChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHallPrevChar::SetSkeletalMesh(USkeletalMesh* NewMesh)
{
	if (NewMesh)
	{
		GetMesh()->SetSkeletalMesh(NewMesh);
	}
}

