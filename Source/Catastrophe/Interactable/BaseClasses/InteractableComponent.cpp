// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Characters/PlayerCharacter/PlayerCharacter.h"
#include "Characters/PlayerCharacter/PlayerWidget.h"


// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	// This component will tick
	PrimaryComponentTick.bCanEverTick = true;

}

void UInteractableComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(PlayerRef) &&
		IsValid(this) &&
		bShowingUi)
	{
		PlayerRef->GetPlayerWidget()->UpdateInteractionUi(this);
	}
}

void UInteractableComponent::OnTriggerWithPlayer(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!PlayerRef && OtherActor->ActorHasTag("Player"))
	{
		PlayerRef = Cast<APlayerCharacter>(OtherActor);
	}

	// See if we already holds the player character pointer
	if (IsValid(PlayerRef) && 
		PlayerRef == OtherActor)
	{
		if (bCanInteract)
		{
			PlayerRef->SetInteractionTarget(this);
			SetInteractionUiVisible(true);
		}
		TriggerCounter++;
	}

	// If this component has set to auto interact, interact immediatly
	if (bAutoInteract && IsValid(PlayerRef))
	{
		Interact(PlayerRef, 0.0f);
	}
}

void UInteractableComponent::OnTriggerEndWithPlayer(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (PlayerRef == OtherActor)
	{
		TriggerCounter--;
		if (TriggerCounter <= 0)
		{
			HoldingTime = 0.0f;
			PlayerRef->ResetInteractionAction();
			PlayerRef->RemoveInteractionTarget(this);
			SetInteractionUiVisible(false);
		}
	}
}

void UInteractableComponent::Interact(class APlayerCharacter* _playerCharacter, float _holdTime)
{
	HoldingTime = _holdTime;

	UPlayerWidget* playerWidget = _playerCharacter->GetPlayerWidget();
	if (bCanInteract && _holdTime >= RequiredHoldTime)
	{
		// Calls the interact functions
		OnInteract.Broadcast(_playerCharacter);

		// Reset the holding time
		HoldingTime = 0.0f;
		_playerCharacter->ResetInteractionAction();

		// If the component has set to one time use, disable after interaction
		if (bOneTimeUse)
		{
			bCanInteract = false;
			SetInteractionUiVisible(false);
		}
	}

	playerWidget->UpdateInteractionUi(this);
}

void UInteractableComponent::RegisterTriggerVolume(class UPrimitiveComponent* _registeringComponent)
{
	// Force the component to generate overlap events
	_registeringComponent->SetGenerateOverlapEvents(true);

	// Binds the function to the component event
	_registeringComponent->OnComponentBeginOverlap.AddDynamic(
		this, &UInteractableComponent::OnTriggerWithPlayer);
	_registeringComponent->OnComponentEndOverlap.AddDynamic(
		this, &UInteractableComponent::OnTriggerEndWithPlayer);
}

void UInteractableComponent::SetInteractionUiVisible(bool _visibility)
{
	bShowingUi = _visibility;
	if (IsValid(PlayerRef))
	{
		PlayerRef->GetPlayerWidget()->SetInteractionUiVisible(_visibility);
	}
}
