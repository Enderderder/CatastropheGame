// Fill out your copyright notice in the Description page of Project Settings.


#include "BackPackComponent.h"

// ============================= BP stuff
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/InheritableComponentHandler.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
// ======================================

#include "Gameplay/Items/ItemStack.h"
#include "Gameplay/Items/ItemBase.h"
#include "Characters/PlayerCharacter/PlayerCharacter.h"

#include "DebugUtility/CatastropheDebug.h"

// Sets default values for this component's properties
UBackPackComponent::UBackPackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

// Called when the game starts
void UBackPackComponent::BeginPlay()
{
	Super::BeginPlay();
	
	playerRef = Cast<APlayerCharacter>(GetOwner());
	if (!playerRef)
		CatastropheDebug::OnScreenErrorMsg(
			TEXT("BackPackComponent: The Owner is not Player"));
}

bool UBackPackComponent::HasValidData() const
{
	return IsValid(playerRef);
}

AItemBase* UBackPackComponent::GetItemCDO(TSubclassOf<class AItemBase> _itemClass) const
{
	return Cast<AItemBase>(_itemClass->GetDefaultObject());
}

bool UBackPackComponent::RemoveItemFromStack(class UItemStack* _itemStack)
{
	if (!_itemStack ||_itemStack->StackSize <= 0) 
		return false;

	_itemStack->StackSize--;
	if (_itemStack->StackSize == 0)
	{
		int32 numB4Remove = BackPack.Num();
		BackPack.RemoveSingle(_itemStack);
		int32 numAfterRemove = BackPack.Num();
		if (numB4Remove != numAfterRemove + 1)
		{
			CatastropheDebug::OnScreenErrorMsg(TEXT("Backpack: ????Didnt remove?"));
			return false;
		}
	}
	return true;
}

class UItemStack* UBackPackComponent::FindItemStack(TSubclassOf<class AItemBase> _itemClass) const
{
	for (UItemStack* stack : BackPack)
	{
		if (stack->ItemClass == _itemClass)
			return stack;
	}

	return nullptr;
}

// Find similar stack, try put the item in
bool UBackPackComponent::InsertItem(class AItemBase* _itemActor)
{
	// Check backpack size
	if (BackPack.Num() >= MaxBackPackSize)
		return false;

	// Insert item to a stack if possible, otherwise create a new stack
	UItemStack* stack = FindItemStack(_itemActor->GetClass());
	if (stack && stack->StackSize < _itemActor->GetItemData().ItemMaxStackSize)
	{
		stack->StackSize++;
	}
	else
	{
		UItemStack* newStack = NewObject<UItemStack>();
		newStack->ItemClass = _itemActor->GetClass();
		newStack->StackSize++;
		BackPack.Add(newStack);

		//TArray<UObject*> candidates;
		//
		////Blueprint Classes
		//{
		//	UBlueprintGeneratedClass* currentClass =
		//		Cast<UBlueprintGeneratedClass>(newStack->ItemClass);
		//	while (currentClass)
		//	{
		//		//UInheritableComponentHandler
		//		if (UInheritableComponentHandler* handler = currentClass->InheritableComponentHandler)
		//		{
		//			TArray<UActorComponent*> templates;
		//			handler->GetAllTemplates(templates);
		//			for (auto componentTemplate : templates)
		//			{
		//				candidates.AddUnique(componentTemplate);
		//			}
		//		}

		//		//USimpleConstructionScript
		//		if (USimpleConstructionScript* scs = currentClass->SimpleConstructionScript) {
		//			for (auto node : scs->GetAllNodes())
		//			{
		//				candidates.AddUnique(node->ComponentTemplate);
		//			}
		//		}

		//		currentClass = Cast<UBlueprintGeneratedClass>(currentClass->GetSuperClass());
		//	}
		//}
		//
		//// C++ Classes
		//{
		//	TSubclassOf<AActor> currentClass = newStack->ItemClass;

		//	while (currentClass)
		//	{
		//		TArray<UObject*> currentSubobjects;
		//		currentClass->GetDefaultObjectSubobjects(currentSubobjects);
		//		for (auto object : currentSubobjects)
		//		{
		//			candidates.AddUnique(object);
		//		}
		//		currentClass = currentClass->GetSuperClass();
		//	}
		//}

		//FString itemName;
		////Loop through candidate objects until we find the first UItemComponent
		//for (auto object : candidates)
		//{
		//	if (auto itemComponent = Cast<UItemComponent>(object))
		//	{
		//		itemName = itemComponent->GetItemName();
		//		break;
		//	}
		//}
	}

	return true;
}

bool UBackPackComponent::UseItem(TSubclassOf<class AItemBase> _itemClass)
{
	if (!HasValidData()) return false;

	UItemStack* itemStack = FindItemStack(_itemClass);
	AItemBase* itemCDO = GetItemCDO(_itemClass);
	if (itemStack && itemCDO)
	{
		itemCDO->UseItem(playerRef);
		if (itemCDO->GetItemData().bConsumeOnUse)
			return RemoveItemFromStack(itemStack);
		
		return true;
	}

	return false;
}

