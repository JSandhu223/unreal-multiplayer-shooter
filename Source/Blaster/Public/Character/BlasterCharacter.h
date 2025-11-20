// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

public:
	void SetOverlappingWeapon(AWeapon* Weapon);

private:
	UPROPERTY(EditAnywhere, Category = "Enhanced Input|Mapping Contexts")
	class UInputMappingContext* KeyboardContext;

	UPROPERTY(EditAnywhere, Category = "Enhanced Input|Mapping Contexts")
	class UInputMappingContext* MouseContext;

	UPROPERTY(EditAnywhere, Category = "Enhanced Input|Input Actions")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Enhanced Input|Input Actions")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Enhanced Input|Input Actions")
	class UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category = "Enhanced Input|Input Actions")
	class UInputAction* EquipAction;

protected:
	void DoJump(const struct FInputActionValue& InputActionValue);

	void DoMove(const FInputActionValue& InputActionValue);

	void DoMouseLook(const FInputActionValue& InputActionValue);

	void DoEquip(const FInputActionValue& InputActionValue);
};
