#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MSG_FireRule.h"
#include "MSG_FireRuleComponent.generated.h"

UCLASS(ClassGroup = (MSG), meta = (BlueprintSpawnableComponent))
class MYSHOOTINGGAME_API UMSG_FireRuleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 组件对外只提供一个普通函数：返回规则结果
	UFUNCTION(BlueprintCallable, Category = "FireRule")
	FMSG_FireRuleResult EvaluateRule(const FMSG_FireRuleContext& Ctx) const;
};