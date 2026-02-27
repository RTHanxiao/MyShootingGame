// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/Components/CameraBreathComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/GameFramework/Actor.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SceneComponent.h"


// Sets default values for this component's properties
UCameraBreathComponent::UCameraBreathComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UCameraBreathComponent::OnRegister()
{
	Super::OnRegister();

	// 注册时尝试找一次目标组件
	if (!TargetComponent)
	{
		FindDefaultTargetComponent();
	}
}

void UCameraBreathComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!TargetComponent)
	{
		FindDefaultTargetComponent();
	}

	CacheBaseTransform();
}

void UCameraBreathComponent::FindDefaultTargetComponent()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// 1) 优先 SpringArm
	TArray<USpringArmComponent*> SpringArms;
	Owner->GetComponents(SpringArms);
	if (SpringArms.Num() > 0)
	{
		TargetComponent = SpringArms[0];
		CachedTargetComponent = TargetComponent;
		return;
	}

	// 2) 再找 CameraComponent
	TArray<UCameraComponent*> Cameras;
	Owner->GetComponents(Cameras);
	if (Cameras.Num() > 0)
	{
		TargetComponent = Cameras[0];
		CachedTargetComponent = TargetComponent;
		return;
	}

	// 3) 最后用 RootComponent
	if (USceneComponent* Root = Cast<USceneComponent>(Owner->GetRootComponent()))
	{
		TargetComponent = Root;
		CachedTargetComponent = TargetComponent;
	}
}

USceneComponent* UCameraBreathComponent::GetEffectiveTargetComponent() const
{
	if (TargetComponent)
	{
		return TargetComponent;
	}

	return CachedTargetComponent.Get();
}

void UCameraBreathComponent::CacheBaseTransform()
{
	if (USceneComponent* Target = GetEffectiveTargetComponent())
	{
		BaseRelativeLocation = Target->GetRelativeLocation();
		BaseRelativeRotation = Target->GetRelativeRotation();
		bHasCachedBase = true;
	}
}

void UCameraBreathComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bEnableCameraBreath)
	{
		return;
	}

	USceneComponent* Target = GetEffectiveTargetComponent();
	if (!Target)
	{
		return;
	}

	if (!bHasCachedBase)
	{
		CacheBaseTransform();
	}

	BreathTime += DeltaTime;

	// 正弦呼吸部分
	const float Breath = (BreathFrequency > 0.f)
		? FMath::Sin(BreathTime * BreathFrequency * 2.0f * PI)
		: 0.0f;

	// 用目标组件的朝向，模拟胸腔前后+上下微小起伏
	const FVector Forward = Target->GetForwardVector();
	const FVector Up = Target->GetUpVector();

	FVector BreathOffset = FVector::ZeroVector;
	if (bAffectLocation)
	{
		BreathOffset =
			Forward * (Breath * BreathAmplitude * 0.5f) +
			Up * (Breath * BreathAmplitude * 0.5f);
	}

	float BreathPitchOffset = 0.0f;
	if (bAffectRotation)
	{
		BreathPitchOffset = Breath * RotAmplitude;
	}

	// 柏林噪声部分
	FVector NoiseOffset = FVector::ZeroVector;
	float NoisePitchOffset = 0.0f;

	if (NoiseAmplitude > 0.0f)
	{
		const float LocFreq = FMath::Max(LocationNoiseFrequency, 0.0001f);
		const float RotFreq = FMath::Max(RotationNoiseFrequency, 0.0001f);

		// 1D Perlin，分别用不同的频率与相位偏移避免完全同相
		const float NoiseX = FMath::PerlinNoise1D(BreathTime * LocFreq * 0.7f);
		const float NoiseY = FMath::PerlinNoise1D(BreathTime * LocFreq * 1.3f + 10.0f);
		const float NoiseZ = FMath::PerlinNoise1D(BreathTime * LocFreq * 0.9f + 20.0f);

		if (bAffectLocation)
		{
			NoiseOffset = FVector(NoiseX, NoiseY, NoiseZ) * NoiseAmplitude;
		}

		// 旋转噪声：只作用在 Pitch 上
		if (bAffectRotation)
		{
			const float NoiseRotPitch = FMath::PerlinNoise1D(BreathTime * RotFreq * 1.1f + 33.0f);
			NoisePitchOffset = NoiseRotPitch * RotAmplitude * 0.5f;
		}
	}

	// 基准 + 呼吸 + 柏林噪声 
	FVector FinalLocation = BaseRelativeLocation;
	FRotator FinalRotation = BaseRelativeRotation;

	if (bAffectLocation)
	{
		FinalLocation += BreathOffset + NoiseOffset;
	}

	if (bAffectRotation)
	{
		const float FinalPitch = BaseRelativeRotation.Pitch + BreathPitchOffset + NoisePitchOffset;
		FinalRotation = FRotator(FinalPitch, BaseRelativeRotation.Yaw, BaseRelativeRotation.Roll);
	}

	Target->SetRelativeLocation(FinalLocation);
	Target->SetRelativeRotation(FinalRotation);
}