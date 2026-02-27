#pragma once

#include "CoreMinimal.h"
#include "CharacterAtt.generated.h"

UENUM(BlueprintType)
enum class EPlayerWeaponType : uint8
{
	FIST,
	RIFLE,
	PISTOL,
	SHOTGUN,
	SNIPER,
};