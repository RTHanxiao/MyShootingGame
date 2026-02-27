// Fill out your copyright notice in the Description page of Project Settings.


#include "MyShootingGame/Logic/MSG_EventManager.h"

UMSG_EventManager* UMSG_EventManager::Instance = nullptr;
UMSG_EventManager* UMSG_EventManager::GetEventManagerInstance()
{
	if(Instance == nullptr)
	{
		Instance = NewObject<UMSG_EventManager>();
		Instance->AddToRoot(); // Prevent garbage collection
	}
	return Instance;
}


