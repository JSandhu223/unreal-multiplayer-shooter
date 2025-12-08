// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterComponents/CombatComponent.h"
#include "Weapon/Weapon.h"
#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	this->BaseWalkSpeed = 600.0f;
	this->AimWalkSpeed = 450.0f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (this->Character)
	{
		this->Character->GetCharacterMovement()->MaxWalkSpeed = this->BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (this->EquippedWeapon && this->Character)
	{
		this->Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		this->Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	
	if (bFireButtonPressed)
	{
		ServerFire();
	}
}

void UCombatComponent::ServerFire_Implementation()
{
	MulticastFire();
}

void UCombatComponent::MulticastFire_Implementation()
{
	if (this->EquippedWeapon == nullptr) { return; }

	if (this->Character)
	{
		this->Character->PlayFireMontage(this->bAiming);
		this->EquippedWeapon->Fire();
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	this->bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (this->Character)
	{
		this->Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	this->bAiming = bIsAiming;
	if (this->Character)
	{
		this->Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
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

	// Set the owner of the weapon to be the character. Note that this is replicated to all clients (see docs).
	this->EquippedWeapon->SetOwner(this->Character);
	
	this->Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	this->Character->bUseControllerRotationYaw = true;
}

