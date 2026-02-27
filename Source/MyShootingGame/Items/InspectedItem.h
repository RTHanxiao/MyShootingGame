// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "InspectedItem.generated.h"
class USceneComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class USceneCaptureComponent2D;

UCLASS()
class MYSHOOTINGGAME_API AInspectedItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInspectedItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ItemStaticMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* ItemSkeletalMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USceneCaptureComponent2D* ItemCaptureComponent;

	void ShowStaticMesh();
	void ShowSkeletalMesh();	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
