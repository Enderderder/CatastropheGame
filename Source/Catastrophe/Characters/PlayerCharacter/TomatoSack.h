// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSack.h"
#include "TomatoSack.generated.h"

UCLASS()
class CATASTROPHE_API ATomatoSack : public AItemSack
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ATomatoSack();

protected:
	TSubclassOf<class ATomato> TomatoClass;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void UseItem() override;
};
