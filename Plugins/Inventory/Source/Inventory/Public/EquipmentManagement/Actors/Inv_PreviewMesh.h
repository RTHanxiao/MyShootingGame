

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inv_PreviewMesh.generated.h"

class USceneCaptureComponent2D;
class UInv_EquipmentComponent;
class USpringArmComponent;

UCLASS()
class INVENTORY_API AInv_PreviewMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	AInv_PreviewMesh();
	USkeletalMeshComponent* GetMesh() const { return Mesh; }
protected:
	virtual void BeginPlay() override;

private:
	//玩家操控的角色的
	TWeakObjectPtr<USkeletalMeshComponent> SourceMesh;
	
	//UI中看到的
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInv_EquipmentComponent> EquipmentComponent;
	
	FTimerHandle TimerForNextTick;
	void DelayedInitialization();
	void DelayInitizeOwner();
};
