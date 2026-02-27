// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraBreathComponent.generated.h"

class USceneComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYSHOOTINGGAME_API UCameraBreathComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCameraBreathComponent();

	//是否启用呼吸抖动 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraBreath")
	bool bEnableCameraBreath = true;

	// 呼吸位移振幅
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraBreath", meta = (ClampMin = "0.0"))
	float BreathAmplitude = 2.0f;

	//呼吸频率（越大呼吸越快，0.2~0.4 比较自然）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraBreath", meta = (ClampMin = "0.0"))
	float BreathFrequency = 0.25f;

	//呼吸引起的俯仰旋转幅度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraBreath", meta = (ClampMin = "0.0"))
	float RotAmplitude = 0.5f;

	// 柏林噪声位移强度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraBreath", meta = (ClampMin = "0.0"))
	float NoiseAmplitude = 1.0f;

	//柏林噪声作用在位置上的频率缩放 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraBreath", meta = (ClampMin = "0.0"))
	float LocationNoiseFrequency = 1.0f;

	// 柏林噪声作用在旋转上的频率缩放 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraBreath", meta = (ClampMin = "0.0"))
	float RotationNoiseFrequency = 1.0f;

	// 是否影响位置 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraBreath")
	bool bAffectLocation = true;

	// 是否影响旋转 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraBreath")
	bool bAffectRotation = true;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraBreath")
	TObjectPtr<USceneComponent> TargetComponent = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:	
	// 累计时间
	float BreathTime = 0.0f;

	// 缓存的基准相对 Transform
	FVector BaseRelativeLocation = FVector::ZeroVector;
	FRotator BaseRelativeRotation = FRotator::ZeroRotator;
	bool bHasCachedBase = false;

	// 缓存最终选中的目标组件
	TWeakObjectPtr<USceneComponent> CachedTargetComponent;

	// 查找默认目标组件
	void FindDefaultTargetComponent();

	// 获取当前使用的目标组件
	USceneComponent* GetEffectiveTargetComponent() const;

	// 缓存目标组件的初始相对 Transform，避免累积漂移
	void CacheBaseTransform();
		
	
};
