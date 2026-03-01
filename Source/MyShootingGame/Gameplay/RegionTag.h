#pragma once 

#include "NativeGameplayTags.h"

namespace GameplayTags
{
	namespace Level
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(LevelA);
	}

	namespace LevelA
	{
		namespace RegionTags
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Region_1);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Region_2);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Region_3);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Region_4);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Region_5);
		}
	}
}