#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MSG_FireRule.h"
#include "MSG_FireRuleOwnerInterface.generated.h"

UINTERFACE(Blueprintable)
class MYSHOOTINGGAME_API UMSG_FireRuleOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

class MYSHOOTINGGAME_API IMSG_FireRuleOwnerInterface
{
	GENERATED_BODY()

public:
	// 关键：让 UnLua/蓝图可以覆写它（Owner 是 Actor）
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FireRule")
	FMSG_FireRuleResult EvaluateFireRule(const FMSG_FireRuleContext& Ctx);
};