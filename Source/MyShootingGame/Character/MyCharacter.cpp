// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/Character/MyCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	LastRepHealth = CurrentHealth;
	bDeathHandled = false;
	bDead = false;
	SetCanBeDamaged(true);
}

float AMyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Actual = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 服务器权威：客户端不在这里结算血量/死亡
	if (!HasAuthority())
	{
		return Actual;
	}

	// 死亡/不可受伤后直接忽略
	if (bDead || !CanBeDamaged())
	{
		return 0.f;
	}

	const float OldHealth = CurrentHealth;
	SetCurrentHealth(CurrentHealth - Actual);

	// 判死（服务器权威）
	if (CurrentHealth <= 0.f && !bDead)
	{
		bDead = true;
		OnRep_Dead(); // 服务器自己也要执行一次表现/收口
	}

	return Actual;
}

void AMyCharacter::SetCurrentHealth(float NewHealth)
{
	CurrentHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);

	// 服务器端维护缓存；客户端由 OnRep 维护
	if (HasAuthority())
	{
		LastRepHealth = CurrentHealth;
	}
}

void AMyCharacter::Ragdoll()
{
	//رײ
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (GetMesh())
	{
		GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
		GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);


		//ģ
		GetMesh()->SetAllBodiesBelowSimulatePhysics("pelvis", true);
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
	} while (RandomIndex == LastIndex && MontageArray.Num() > 1); // ѭ

	LastIndex = RandomIndex;

	if (UAnimMontage* SelectedMontage = MontageArray[RandomIndex])
	{
		PlayAnimMontage(SelectedMontage);
	}
}

void AMyCharacter::MeleeAttackCollision(AController* EventInstigator)
{
	if (!HasAuthority()) return;
	if (bDead) return;

	FVector SpherePos = GetActorLocation() + GetActorForwardVector() * 100.f;
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

	for (AActor* Actor : OutActors)
	{
		if (AMyCharacter* AttackTarget = Cast<AMyCharacter>(Actor))
		{
			if (AttackTarget->IsDead()) { continue; }
			UGameplayStatics::ApplyDamage(AttackTarget, MeleeDamage, EventInstigator, this, UDamageType::StaticClass());
			//(MeleeDamage, FDamageEvent(), EventInstigator, this);
		}
	}
}



void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCharacter, bDead);
	DOREPLIFETIME(AMyCharacter, CurrentHealth);
}




void AMyCharacter::OnTookDamage(float DamageAmount, AActor* DamageCauser)
{
	// Base: no-op
}

void AMyCharacter::OnDied()
{
	// Base: no-op
}


void AMyCharacter::OnRep_CurrentHealth()
{
	if (bDead)
	{
		LastRepHealth = CurrentHealth;
		return;
	}

	if (CurrentHealth < LastRepHealth)
	{
		const float Damage = LastRepHealth - CurrentHealth;
		OnTookDamage(Damage, nullptr);
	}

	LastRepHealth = CurrentHealth;
}

void AMyCharacter::OnRep_Dead()
{
	if (!bDead) return;
	if (bDeathHandled) return;
	bDeathHandled = true;

	SetCanBeDamaged(false);

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->DisableMovement();
	}

	if (UCapsuleComponent* Cap = GetCapsuleComponent())
	{
		Cap->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	OnDied();
}

void AMyCharacter::BP_MeleeAttackCollision()
{
	AController* Inst = GetController();
	if (HasAuthority())
	{
		MeleeAttackCollision(Inst);
		return;
	}
	Server_MeleeAttackCollision(Inst);
}

void AMyCharacter::Server_MeleeAttackCollision_Implementation(AController* EventInstigator)
{
	MeleeAttackCollision(EventInstigator);
}
