// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterComponents/CombatComponent.h"
#include "Weapon/Weapon.h"
#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (this->Character == nullptr || WeaponToEquip == nullptr) { return; }

	this->EquippedWeapon = WeaponToEquip;
	this->EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	// Attach the equipped weapon to the right hand socket
	const USkeletalMeshSocket* HandSocket = this->Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(this->EquippedWeapon, this->Character->GetMesh());
	}

	// Set the owner of the weapon to be the character
	this->EquippedWeapon->SetOwner(this->Character);
	// Hide the pickup widget
	this->EquippedWeapon->ShowPickupWidget(false);
}

