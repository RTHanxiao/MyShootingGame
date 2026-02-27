#include "Inv_WeaponFragments.h"
#include "Items/Manifest/Inv_ItemManifest.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


#include "Engine/DamageEvents.h"

#include "MyShootingGame/GameCore/MSG_PlayerController.h"
#include "MyShootingGame/Character/ZombieCharacter.h"
#include "MyShootingGame/Logic/MSG_EventManager.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Sound/SoundBase.h"
#include "Items/Inv_InventoryItem.h"


static float ApplySurfaceMultiplier(const FInv_WeaponDamageFragment* Dmg, EPhysicalSurface Surface, bool& bHeadShot)
{
	bHeadShot = false;
	if (!Dmg) return 1.f;

	switch (Surface)
	{
	case EPhysicalSurface::SurfaceType7: bHeadShot = true; return Dmg->HeadshotMultiplier;
	case EPhysicalSurface::SurfaceType8: return Dmg->ArmMultiplier;
	case EPhysicalSurface::SurfaceType9: return Dmg->LegMultiplier;
	case EPhysicalSurface::SurfaceType10: return Dmg->ChestMultiplier;
	case EPhysicalSurface::SurfaceType11: return Dmg->AbsMultiplier;
	default: return 1.f;
	}
}

void FInv_WeaponHitResolveFragment::ResolveHit(const UInv_InventoryItem* Item, APawn* InstigatorPawn, AActor* WeaponActor, const FHitResult& Hit) const
{
	if (!Item || !WeaponActor) return;

	const FInv_ItemManifest& Manifest = Item->GetItemManifest();

	const FInv_WeaponDamageFragment* DmgFrag = Manifest.GetFragmentOfType<FInv_WeaponDamageFragment>();
	const FInv_WeaponImpactFxFragment* FxFrag = Manifest.GetFragmentOfType<FInv_WeaponImpactFxFragment>();

	UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
	const EPhysicalSurface Surface = PhysMat ? PhysMat->SurfaceType.GetValue() : EPhysicalSurface::SurfaceType_Default;

	AActor* HitActor = Hit.GetActor();

	// 1) 伤害结算：只对 Zombie
	if (AZombieCharacter* Zombie = Cast<AZombieCharacter>(HitActor))
	{
		AMSG_PlayerController* PC = InstigatorPawn ? Cast<AMSG_PlayerController>(InstigatorPawn->GetController()) : nullptr;

		float Damage = DmgFrag ? DmgFrag->BaseDamage : 0.f;
		bool bHeadShot = false;

		Damage *= ApplySurfaceMultiplier(DmgFrag, Surface, bHeadShot);

		// 应用减伤
		Damage *= Zombie->DamageReductionFactor;

		// 造成伤害
		FDamageEvent DamageEvent;
		if (DmgFrag && *DmgFrag->DamageTypeClass)
		{
			DamageEvent.DamageTypeClass = DmgFrag->DamageTypeClass;
		}
		Zombie->TakeDamage(Damage, DamageEvent, PC, WeaponActor);


		const bool bDead = Zombie->IsDead();

		// 击杀/命中反馈回传（你原逻辑）
		if (bDead)
		{
			if (bHeadShot)
			{
				UMSG_EventManager::GetEventManagerInstance()->ShowKillFeedbackDelegate.ExecuteIfBound(true);
				UMSG_EventManager::GetEventManagerInstance()->HeadShotEventDelegate.ExecuteIfBound();
			}
			else
			{
				UMSG_EventManager::GetEventManagerInstance()->ShowKillFeedbackDelegate.ExecuteIfBound(false);
			}
		}
		else
		{
			UMSG_EventManager::GetEventManagerInstance()->ShowHitFeedbackDelegate.ExecuteIfBound(bHeadShot);
		}

		// 血液粒子
		if (FxFrag && FxFrag->FleshParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				WeaponActor->GetWorld(),
				FxFrag->FleshParticle,
				Hit.Location,
				UKismetMathLibrary::Conv_VectorToRotator(Hit.Normal),
				FVector(.5f, .5f, .5f));
		}

		// 爆头音
		if (bHeadShot && FxFrag && FxFrag->HeadShotSound)
		{
			UGameplayStatics::PlaySoundAtLocation(WeaponActor->GetWorld(), FxFrag->HeadShotSound, Hit.Location);
		}
	}

	// 2) 地表命中反馈（粒子/音效/贴花）
	if (FxFrag)
	{
		if (const TObjectPtr<UParticleSystem>* P = FxFrag->SurfaceParticles.Find(Surface))
		{
			if (*P)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					WeaponActor->GetWorld(),
					*P,
					Hit.Location,
					UKismetMathLibrary::Conv_VectorToRotator(Hit.Normal),
					FVector(.5f, .5f, .5f));
			}
		}

		if (const TObjectPtr<USoundBase>* S = FxFrag->SurfaceSounds.Find(Surface))
		{
			if (*S)
			{
				UGameplayStatics::PlaySoundAtLocation(WeaponActor->GetWorld(), *S, Hit.Location);
			}
		}

		if (FxFrag->BulletDecalMaterial)
		{
			UGameplayStatics::SpawnDecalAtLocation(
				WeaponActor->GetWorld(),
				FxFrag->BulletDecalMaterial,
				FVector(5.f, 5.f, 5.f),
				Hit.Location,
				UKismetMathLibrary::Conv_VectorToRotator(Hit.Normal));
		}
	}
}
