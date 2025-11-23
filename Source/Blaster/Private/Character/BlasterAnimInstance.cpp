// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	this->BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (this->BlasterCharacter == nullptr)
	{
		this->BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
		return;
	}

	FVector Velocity = this->BlasterCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	Speed = Velocity.Size();

	bIsInAir = this->BlasterCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = this->BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;

	bWeaponEquipped = this->BlasterCharacter->IsWeaponEquipped();
}
