// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "BlasterComponents/CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"


ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	this->CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	this->CameraBoom->SetupAttachment(GetMesh());
	this->CameraBoom->TargetArmLength = 600.0f;
	this->CameraBoom->bUsePawnControlRotation = true; // allow the camera boom to rotate along with the controller (i.e. mouse movement)

	this->FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	this->FollowCamera->SetupAttachment(this->CameraBoom, USpringArmComponent::SocketName);
	this->FollowCamera->bUsePawnControlRotation = false;

	// Prevents character from rotating along with the controller rotation
	bUseControllerRotationYaw = false;
	// Makes the character face toward the direction of movement
	GetCharacterMovement()->bOrientRotationToMovement = true;

	this->OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	this->OverheadWidget->SetupAttachment(this->RootComponent);

	this->Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	this->Combat->SetIsReplicated(true);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Register OverlappingWeapon to be replicated
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();	
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->OverlappingWeapon)
	{
		this->OverlappingWeapon->ShowPickupWidget(true);
	}
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (this->Combat)
	{
		this->Combat->Character = this;
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (this->OverlappingWeapon)
	{
		this->OverlappingWeapon->ShowPickupWidget(false);
	}

	this->OverlappingWeapon = Weapon;
	if (this->IsLocallyControlled())
	{
		if (this->OverlappingWeapon)
		{
			this->OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

AWeapon* ABlasterCharacter::GetOverlappingWeapon()
{
	return this->OverlappingWeapon;
}

UCombatComponent* ABlasterCharacter::GetCombatComponent()
{
	return this->Combat;
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (this->OverlappingWeapon)
	{
		this->OverlappingWeapon->ShowPickupWidget(true);
	}

	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}
