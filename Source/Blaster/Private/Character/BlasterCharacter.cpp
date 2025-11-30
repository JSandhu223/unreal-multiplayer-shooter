// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "BlasterComponents/CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"


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

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	// Fixes annoying bug where character collides with camera at certain angles
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
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

	/*if (this->OverlappingWeapon)
	{
		this->OverlappingWeapon->ShowPickupWidget(true);
	}*/

	AimOffset(DeltaTime);
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

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (this->Combat && this->Combat->EquippedWeapon == nullptr) { return; }

	FVector Velocity = this->GetVelocity();
	Velocity.Z = 0.0f;
	float Speed = Velocity.Size();

	bool bIsInAir = this->GetCharacterMovement()->IsFalling();

	// Standing still and not jumping
	if (Speed == 0.0f && !bIsInAir)
	{
		FRotator CurrentAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, this->StartingAimRotation);
		this->AO_Yaw = DeltaAimRotation.Yaw;
		this->bUseControllerRotationYaw = false;
	}
	
	// Running or jumping
	if (Speed > 0.0f || bIsInAir)
	{
		this->StartingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		this->AO_Yaw = 0.0f;
		this->bUseControllerRotationYaw = true;
	}

	this->AO_Pitch = GetBaseAimRotation().Pitch;
	if (this->AO_Pitch > 90.0f && !this->IsLocallyControlled())
	{
		// Map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.0f, 360.0f);
		FVector2D OutRange(-90.0f, 0.0f);
		this->AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, this->AO_Pitch);
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

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (this->Combat && this->Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (this->Combat && this->Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetOverlappingWeapon()
{
	return this->OverlappingWeapon;
}

UCombatComponent* ABlasterCharacter::GetCombatComponent()
{
	return this->Combat;
}

float ABlasterCharacter::GetAO_Yaw() const
{
	return this->AO_Yaw;
}

float ABlasterCharacter::GetAO_Pitch() const
{
	return this->AO_Pitch;
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (this->Combat == nullptr) { return nullptr; }

	return this->Combat->EquippedWeapon;
}

ETurningInPlace ABlasterCharacter::GetTurningInPlace() const
{
	return this->TurningInPlace;
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
