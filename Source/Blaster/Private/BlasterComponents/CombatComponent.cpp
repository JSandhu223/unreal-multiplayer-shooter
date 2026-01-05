#include "BlasterComponents/CombatComponent.h"
#include "Weapon/Weapon.h"
#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "TimerManager.h"


UCombatComponent::UCombatComponent()
{
	RegisterComponent();
	PrimaryComponentTick.bCanEverTick = true;

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

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = this->BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		this->HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) { return; }

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;

		if (HUD)
		{
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}

			// Calculate crosshair spread based on the velocity
			FVector2D WalkSpeedRange(0.0f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.0f, 1.0f);
			FVector Velocity = Character->GetVelocity();
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size2D());

			// Also take into account if the character is in the air
			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.0f, DeltaTime, 30.0f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, -0.58f, DeltaTime, 30.0f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.0f, DeltaTime, 30.0f);
			}

			// Prevents the crosshairs from shrinking into each other
			float BaselineSpread = 0.5f;
			// The shooting factor should always be interpolating back to zero so that the crosshair shrinks when the player stops shooting
			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.0f, DeltaTime, 20.0f);

			HUDPackage.CrosshairsSpread = BaselineSpread + CrosshairVelocityFactor + CrosshairInAirFactor + CrosshairAimFactor + CrosshairShootingFactor;

			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) { return; }

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
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
	
	if (bFireButtonPressed && EquippedWeapon)
	{
		Fire();
	}
}

void UCombatComponent::Fire()
{
	if (bCanFire)
	{
		bCanFire = false;
		ServerFire(HitTarget);

		if (EquippedWeapon)
		{
			CrosshairShootingFactor = 0.75f;
		}
		StartFireTimer();
	}
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr) { return; }

	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedWeapon->FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr) { return; }

	bCanFire = true;
	if (bFireButtonPressed && EquippedWeapon && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (this->EquippedWeapon == nullptr) { return; }

	if (this->Character)
	{
		this->Character->PlayFireMontage(this->bAiming);
		this->EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport.Get()->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
	// The viewport x and y are screen space coordinates, so get them as world space coordinates
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		const float Trace_Length = 80000.0f;
		FVector Start = CrosshairWorldPosition;

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.0f);
		}

		FVector End = Start + (CrosshairWorldDirection * Trace_Length);

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		// Draw red crosshairs when crosshairs are over another character
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
		}

		// If the line trace didn't hit anything, manually set the impact point
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}

		DrawDebugSphere(
			GetWorld(),
			TraceHitResult.ImpactPoint,
			12.0f,
			12,
			FColor::Red
		);
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

