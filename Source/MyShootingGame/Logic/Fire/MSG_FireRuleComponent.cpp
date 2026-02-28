#include "MyShootingGame/Logic/Fire/MSG_FireRuleComponent.h"
#include "MyShootingGame/Logic/Fire/MSG_FireRuleOwnerInterface.h"
#include "GameFramework/Actor.h"

FMSG_FireRuleResult UMSG_FireRuleComponent::EvaluateRule(const FMSG_FireRuleContext& Ctx) const
{
	if (AActor* Owner = GetOwner())
	{
		if (Owner->GetClass()->ImplementsInterface(UMSG_FireRuleOwnerInterface::StaticClass()))
		{
			return IMSG_FireRuleOwnerInterface::Execute_EvaluateFireRule(Owner, Ctx);
		}
	}

	// fallback：默认允许开火，不覆盖任何参数
	return FMSG_FireRuleResult{};
}