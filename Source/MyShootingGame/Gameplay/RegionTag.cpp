#include "RegionTag.h"

namespace GameplayTags
{
	namespace Levels
	{
		UE_DEFINE_GAMEPLAY_TAG(Level_A,"GameplayTags.Levels.Level_A");
	}

	namespace LevelA
	{
		namespace RegionTags
		{
			UE_DEFINE_GAMEPLAY_TAG(Region_1, "GameplayTags.LevelA.RegionTags.Region_1");
			UE_DEFINE_GAMEPLAY_TAG(Region_2, "GameplayTags.LevelA.RegionTags.Region_2");
			UE_DEFINE_GAMEPLAY_TAG(Region_3, "GameplayTags.LevelA.RegionTags.Region_3");
			UE_DEFINE_GAMEPLAY_TAG(Region_4, "GameplayTags.LevelA.RegionTags.Region_4");
			UE_DEFINE_GAMEPLAY_TAG(Region_5, "GameplayTags.LevelA.RegionTags.Region_5");
		}
	}
}