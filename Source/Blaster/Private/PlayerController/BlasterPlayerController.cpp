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

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABlasterPlayerController::DoJump);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterPlayerController::DoMove);
	EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ABlasterPlayerController::DoMouseLook);
	EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Completed, this, &ABlasterPlayerController::EquipButtonPressed);
}

void ABlasterPlayerController::DoJump(const FInputActionValue& InputActionValue)
{
	float Value = InputActionValue.Get<float>();

	//UE_LOG(LogTemp, Warning, TEXT("Jump value: %f"), Value);

	// For now, we'll just call the existing Jump function from the Character class
	if (ACharacter* ControlledCharacter = this->GetCharacter())
	{
		ControlledCharacter->Jump();
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

		// DEBUG
		UE_LOG(LogTemp, Warning, TEXT("Equipped weapon!"));
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