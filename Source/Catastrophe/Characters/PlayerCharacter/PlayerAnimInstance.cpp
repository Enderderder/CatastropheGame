// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "PlayerCharacter.h"

#include "DebugUtility/CatastropheDebug.h"

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	// Store player character reference
	CustomPlayerCharacter = Cast<APlayerCharacter>(GetOwningActor());
	if (!CustomPlayerCharacter)
	{
		CatastropheDebug::OnScreenErrorMsg(TEXT("PlayerAnimInstance: CustomPlayerCharacter is null"));
		UE_LOG(LogTemp, Warning, TEXT("PlayerAnimInstance: CustomPlayerCharacter is null"));
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	// Verify again after parent function call to make sure
	if (!IsValid(CustomPlayerCharacter)) return;
	
	// Update animation data
	if (UCharacterMovementComponent* PlayerMovementComponent 
		= CustomPlayerCharacter->GetCharacterMovement())
	{
		FVector playerVelocity = PlayerMovementComponent->Velocity;
		FRotator playerRotation = CustomPlayerCharacter->GetActorRotation();
		FVector unrotatedVelocity = playerRotation.UnrotateVector(playerVelocity);
		
		Speed = playerVelocity.Size();
		RightSpeed = unrotatedVelocity.Y;
		ForwardSpeed = unrotatedVelocity.X;
		bInAir = PlayerMovementComponent->IsFalling();
		bCrouch = PlayerMovementComponent->IsCrouching();

	}
}
