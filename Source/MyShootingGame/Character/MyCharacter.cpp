// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/Character/MyCharacter.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "PlayerCharacter.h"
#include "../Logic/MSG_EventManager.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Engine/DamageEvents.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"


// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	Initilize();
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyCharacter::Initilize()
{
	CurrentHealth = MaxHealth;
}

float AMyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Warning, TEXT("TakeDamage; %.1f FRom: %s"), DamageAmount, *GetNameSafe(DamageCauser));

	SetCurrentHealth(CurrentHealth - DamageAmount);


	if (CurrentHealth <= 0)
	{
		bDead = true;
		UE_LOG(LogTemp, Warning, TEXT("Character died!"));
		Ragdoll();
	}
	return DamageAmount;
}

void AMyCharacter::SetCurrentHealth(float NewHealth)
{
	CurrentHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);
}

void AMyCharacter::Ragdoll()
{
	//关闭碰撞
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if(GetMesh())
	{
		GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
		GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);


		//开启物理模拟
		GetMesh()->SetAllBodiesBelowSimulatePhysics("pelvis",true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;


		
	}
}

void AMyCharacter::PlayRandomMontageFromArray(const TArray<UAnimMontage*>& MontageArray)
{
	if (MontageArray.Num() == 0) return;

	LastIndex = -1;
	int32 RandomIndex;

	do
	{
		RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, MontageArray.Num() - 1);
	} while (RandomIndex == LastIndex && MontageArray.Num() > 1); // 避免无限循环

	LastIndex = RandomIndex;

	if (UAnimMontage* SelectedMontage = MontageArray[RandomIndex])
	{
		PlayAnimMontage(SelectedMontage);
	}
}

void AMyCharacter::MeleeAttackCollision(AController* EventInstigator)
{
	FVector SpherePos = GetActorLocation() + GetActorForwardVector()*100.f;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	TArray<AActor*> OutActors;


	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		SpherePos,
		100.f,
		ObjectTypes,
		AttackableTargetClass,
		IgnoreActors,
		OutActors
	);

	for(AActor* Actor : OutActors)
	{
		if(AMyCharacter* AttackTarget = Cast<AMyCharacter>(Actor))
		{
			AttackTarget->TakeDamage(MeleeDamage, FDamageEvent(), EventInstigator, this);
		}
	}
}

