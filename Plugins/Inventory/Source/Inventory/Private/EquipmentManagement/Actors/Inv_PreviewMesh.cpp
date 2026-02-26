#include "EquipmentManagement/Actors/Inv_PreviewMesh.h"
#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"

AInv_PreviewMesh::AInv_PreviewMesh()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	EquipmentComponent = CreateDefaultSubobject<UInv_EquipmentComponent>("Equipment");
	EquipmentComponent->SetOwningSkeletal(Mesh);
	EquipmentComponent->SetIsPreview(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture");
	SceneCaptureComponent->SetupAttachment(SpringArm);

	
}

void AInv_PreviewMesh::BeginPlay()
{
	Super::BeginPlay();
	DelayInitizeOwner();

}

void AInv_PreviewMesh::DelayedInitialization()
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &ThisClass::DelayInitizeOwner);
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
	
	
}

void AInv_PreviewMesh::DelayInitizeOwner()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		DelayedInitialization();
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!IsValid(PC))
	{
		DelayedInitialization();
		return;
	}

	ACharacter* Character = Cast<ACharacter>(PC->GetPawn());
	if (!IsValid(Character))
	{
		DelayedInitialization();
		return;
	}

	USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	if (!IsValid(CharacterMesh))
	{
		DelayedInitialization();
		return;
	}

	SourceMesh = CharacterMesh;
	Mesh->SetSkeletalMesh(SourceMesh->GetSkeletalMeshAsset());
	Mesh->SetAnimInstanceClass(SourceMesh->GetAnimInstance()->GetClass());
	EquipmentComponent->InitializeOwner(PC);

	SceneCaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCaptureComponent->ClearShowOnlyComponents();
	SceneCaptureComponent->ShowOnlyComponent(Mesh);
}

