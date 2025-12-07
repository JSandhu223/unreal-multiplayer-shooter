// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/BlasterPlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "Character/BlasterCharacter.h"
#include "BlasterComponents/CombatComponent.h"


ABlasterPlayerController::ABlasterPlayerController()
{
	this->bReplicates = true;
}

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(KeyboardContext);
	check(MouseContext);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(KeyboardContext, 0);
		Subsystem->AddMappingContext(MouseContext, 0);
	}
}

void ABlasterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(this->InputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABlasterPlayerController::DoJump);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterPlayerController::DoMove);
	EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ABlasterPlayerController::DoMouseLook);
	EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Completed, this, &ABlasterPlayerController::EquipButtonPressed);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ABlasterPlayerController::CrouchButtonPressed);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABlasterPlayerController::AimButtonPressed);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABlasterPlayerController::AimButtonReleased);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABlasterPlayerController::FireButtonPressed);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABlasterPlayerController::FireButtonReleased);
}

void ABlasterPlayerController::DoJump(const FInputActionValue& InputActionValue)
{
	float Value = InputActionValue.Get<float>();

	//UE_LOG(LogTemp, Warning, TEXT("Jump value: %f"), Value);

	// For now, we'll just call the existing Jump function from the Character class
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(this->GetCharacter()))
	{
		if (BlasterCharacter->bIsCrouched)
		{
			BlasterCharacter->UnCrouch();
		}
		else
		{
			BlasterCharacter->Jump();
		}
	}
}

void ABlasterPlayerController::DoMove(const FInputActionValue& InputActionValue)
{
	FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	//UE_LOG(LogTemp, Warning, TEXT("Jump value: %s"), *InputAxisVector.ToString());

	const FRotator YawRotation(0.0f, this->GetControlRotation().Yaw, 0.0f);
	const FVector ForwardDirection(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
	const FVector RightDirection(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

	if (ACharacter* ControlledCharacter = this->GetCharacter())
	{
		ControlledCharacter->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledCharacter->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void ABlasterPlayerController::DoMouseLook(const FInputActionValue& InputActionValue)
{
	FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	//UE_LOG(LogTemp, Warning, TEXT("Jump value: %s"), *InputAxisVector.ToString());

	AddYawInput(InputAxisVector.X);
	AddPitchInput(InputAxisVector.Y);
}

void ABlasterPlayerController::EquipButtonPressed(const FInputActionValue& InputActionValue)
{
	bool InputValue = InputActionValue.Get<bool>();

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(this->GetCharacter());
	if (!BlasterCharacter) { return; }

	// Equip the weapon on the SERVER
	if (BlasterCharacter->GetCombatComponent())
	{
		if (BlasterCharacter->HasAuthority())
		{
			BlasterCharacter->GetCombatComponent()->EquipWeapon(BlasterCharacter->GetOverlappingWeapon());
		}

		// If a client is trying to equip the weapon, execute the RPC
		else
		{
			this->ServerEquipButtonPressed();
		}
	}
}

void ABlasterPlayerController::ServerEquipButtonPressed_Implementation()
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(this->GetCharacter());
	if (!BlasterCharacter) { return; }

	if (BlasterCharacter->GetCombatComponent())
	{
		BlasterCharacter->GetCombatComponent()->EquipWeapon(BlasterCharacter->GetOverlappingWeapon());
	}
}

void ABlasterPlayerController::CrouchButtonPressed(const FInputActionValue& InputActionValue)
{
	if (ACharacter* ControlledCharacter = this->GetCharacter())
	{
		ControlledCharacter->bIsCrouched ? ControlledCharacter->UnCrouch() : ControlledCharacter->Crouch();
	}
}

void ABlasterPlayerController::AimButtonPressed(const FInputActionValue& InputActionValue)
{
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(this->GetCharacter()))
	{
		if (BlasterCharacter->GetCombatComponent())
		{
			BlasterCharacter->GetCombatComponent()->SetAiming(true);
		}
	}
}

void ABlasterPlayerController::AimButtonReleased(const FInputActionValue& InputActionValue)
{
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(this->GetCharacter()))
	{
		if (BlasterCharacter->GetCombatComponent())
		{
			BlasterCharacter->GetCombatComponent()->SetAiming(false);
		}
	}
}

void ABlasterPlayerController::FireButtonPressed(const FInputActionValue& InputActionValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Fire button pressed!"));
}

void ABlasterPlayerController::FireButtonReleased(const FInputActionValue& InputActionValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Fire button released!"));
}
