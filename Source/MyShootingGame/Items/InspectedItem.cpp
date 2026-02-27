// Fill out your copyright notice in the Description page of Project Settings.

#include "MyShootingGame/Items/InspectedItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"

// Sets default values
AInspectedItem::AInspectedItem()
{
 	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh")); // ÐÞÕýÀàÐÍ
	ItemStaticMesh->SetupAttachment(RootComponent);
	ItemStaticMesh->SetVisibility(false);
	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletalMesh"));
	ItemSkeletalMesh->SetupAttachment(RootComponent);
	ItemSkeletalMesh->SetVisibility(false);

	ItemCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("ItemCaptureComponent"));
	ItemCaptureComponent->SetupAttachment(RootComponent);
}


void AInspectedItem::ShowStaticMesh()
{
	ItemStaticMesh->SetVisibility(true);
	ItemSkeletalMesh->SetVisibility(false);
}


void AInspectedItem::ShowSkeletalMesh()
{
	ItemStaticMesh->SetVisibility(false);
	ItemSkeletalMesh->SetVisibility(true);
}

// Called when the game starts or when spawned
void AInspectedItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInspectedItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

