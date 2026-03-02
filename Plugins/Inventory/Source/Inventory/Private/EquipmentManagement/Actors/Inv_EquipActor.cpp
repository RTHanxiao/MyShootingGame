#include "EquipmentManagement/Actors/Inv_EquipActor.h"
#include "Net/UnrealNetwork.h"


AInv_EquipActor::AInv_EquipActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}


void AInv_EquipActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInv_EquipActor, EquipmentType);
}