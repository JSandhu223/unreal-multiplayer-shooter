// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABlasterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(class AWeapon* WeaponToEquip);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	class ABlasterCharacter* Character;

	class ABlasterPlayerController* Controller;

	class ABlasterHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

public:
	UPROPERTY(Replicated)
	bool bAiming = false;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(struct FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

private:
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	/**
	* HUD and crosshairs	
	*/
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	FVector HitTarget;

	/*
	* Aiming and FOV
	*/
	// Field of view when not aiming set to the target's base FOV in BeginPlay
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV = 30.0f;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed = 20.0f;

	void InterpFOV(float DeltaTime);
};
