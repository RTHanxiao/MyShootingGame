#include "Service/LuaWorldService.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Service/HotReload.h"

static bool IsRuntimeWorld(const UWorld* World)
{
	if (!World) return false;
	// PIE / Game 才创建，避免 EditorPreview/EditorWorld 干扰
	return World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE;
}

bool ULuaWorldService::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return IsRuntimeWorld(World);
}

void ULuaWorldService::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LuaInit();
}

void ULuaWorldService::PostInitialize()
{
	Super::PostInitialize();
	LuaPostInit();
}

void ULuaWorldService::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	LuaOnWorldBeginPlay(&InWorld);
}

void ULuaWorldService::Deinitialize()
{
	LuaDeinit();
	Super::Deinitialize();
}

UUserWidget* ULuaWorldService::CreateWidgetByPath(const FString& InPath, APlayerController* OwningPlayer, int32 ZOrder, bool bAddToViewport)
{
	if (!OwningPlayer) return nullptr;

	// Normalize path into "/Game/.../BP_xxx.BP_xxx_C"
	auto NormalizeWidgetClassPath = [](FString Path) -> FString
		{
			Path.TrimStartAndEndInline();

			// Case A: /Script/UMGEditor.WidgetBlueprint'/Game/.../BP_UI_XXX.BP_UI_XXX'
			// Extract between the single quotes.
			const int32 QuoteStart = Path.Find(TEXT("'"));
			if (QuoteStart != INDEX_NONE)
			{
				const int32 QuoteEnd = Path.Find(TEXT("'"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				if (QuoteEnd != INDEX_NONE && QuoteEnd > QuoteStart)
				{
					Path = Path.Mid(QuoteStart + 1, QuoteEnd - QuoteStart - 1);
				}
			}

			// Now Path should look like "/Game/.../BP_UI_XXX.BP_UI_XXX"
			// Ensure it has ".AssetName" part. If not, derive from last segment.
			if (!Path.Contains(TEXT(".")))
			{
				int32 SlashIdx = INDEX_NONE;
				if (Path.FindLastChar(TEXT('/'), SlashIdx) && SlashIdx != INDEX_NONE && SlashIdx + 1 < Path.Len())
				{
					const FString AssetName = Path.Mid(SlashIdx + 1);
					Path = Path + TEXT(".") + AssetName;
				}
			}

			// Ensure it's class path ending with _C
			if (!Path.EndsWith(TEXT("_C")))
			{
				Path += TEXT("_C");
			}
			return Path;
		};

	const FString ClassPath = NormalizeWidgetClassPath(InPath);
	UClass* WidgetClass = StaticLoadClass(UUserWidget::StaticClass(), nullptr, *ClassPath);
	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CreateWidgetByPath] Failed to load widget class. InPath=%s, ClassPath=%s"), *InPath, *ClassPath);
		return nullptr;
	}

	UUserWidget* W = CreateWidget<UUserWidget>(OwningPlayer, WidgetClass);
	if (W && bAddToViewport)
	{
		W->AddToViewport(ZOrder);
	}
	return W;
}

void ULuaWorldService::HotReloadLua()
{
	const int32 EnvCount = ULuaHotReloadLibrary::HotReloadAllLuaEnvs();
	UE_LOG(LogTemp, Warning, TEXT("[LuaWorldService] HotReloadLua called"));
	LuaOnHotReloaded(EnvCount);
}