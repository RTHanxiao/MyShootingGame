#include "Items/Fragments/Inv_FragmentTags.h"

namespace FragmentTags
{
	UE_DEFINE_GAMEPLAY_TAG(GridFragment,"FragmentTags.GridFragment");
	UE_DEFINE_GAMEPLAY_TAG(IconFragment, "FragmentTags.IconFragment");
	UE_DEFINE_GAMEPLAY_TAG(StackableFragment, "FragmentTags.StackableFragment");
	UE_DEFINE_GAMEPLAY_TAG(ConsumableFragment, "FragmentTags.ConsumableFragment");
	UE_DEFINE_GAMEPLAY_TAG(EquipmentFragment, "FragmentTags.EquipmentFragment");

	UE_DEFINE_GAMEPLAY_TAG(TexteFragment, "FragmentTags.TextFragment");
	UE_DEFINE_GAMEPLAY_TAG(PrimaryStateFragment, "FragmentTags.PrimaryStateFragment");

	namespace StatMod
	{
		UE_DEFINE_GAMEPLAY_TAG(StatMod1, "FragmentTags.StatMod.1");
		UE_DEFINE_GAMEPLAY_TAG(StatMod2, "FragmentTags.StatMod.2");
		UE_DEFINE_GAMEPLAY_TAG(StatMod3, "FragmentTags.StatMod.3");
	}

	UE_DEFINE_GAMEPLAY_TAG(WeaponAmmoFragment, "FragmentTags.Weapon.WeaponAmmoFragment");
	UE_DEFINE_GAMEPLAY_TAG(WeaponFireControlFragment, "FragmentTags.Weapon.WeaponFireControlFragment");
	UE_DEFINE_GAMEPLAY_TAG(WeaponFireConfigFragment, "FragmentTags.Weapon.WeaponFireConfigFragment");
	UE_DEFINE_GAMEPLAY_TAG(WeaponDamageFragment, "FragmentTags.Weapon.WeaponDamageFragment");
	UE_DEFINE_GAMEPLAY_TAG(WeaponImpactFxFragment, "FragmentTags.Weapon.WeaponImpactFxFragment");
	UE_DEFINE_GAMEPLAY_TAG(WeaponHitResolveFragment, "FragmentTags.Weapon.WeaponHitResolveFragment");
	UE_DEFINE_GAMEPLAY_TAG(WeaponRecoilFragment, "FragmentTags.Weapon.WeaponRecoilFragment");
	UE_DEFINE_GAMEPLAY_TAG(WeaponReloadConfigFragment, "FragmentTags.Weapon.WeaponReloadConfigFragment");
} 