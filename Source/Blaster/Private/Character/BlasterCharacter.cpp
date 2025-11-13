// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"


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
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// TODO: move enhanced input code to a player controller class
	check(KeyboardContext);
	check(MouseContext);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetController<APlayerController>()->GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(KeyboardContext, 0);
		Subsystem->AddMappingContext(MouseContext, 0);
	}
	
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// TODO: Move enhanced input code to a player controller class
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::DoJump);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::DoMove);
	EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::DoMouseLook);
}

void ABlasterCharacter::DoJump(const FInputActionValue& InputActionValue)
{
	float Value = InputActionValue.Get<float>();

	//UE_LOG(LogTemp, Warning, TEXT("Jump value: %f"), Value);

	// For now, we'll just call the existing Jump function from the Character class
	Jump();
}

void ABlasterCharacter::DoMove(const FInputActionValue& InputActionValue)
{
	FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	//UE_LOG(LogTemp, Warning, TEXT("Jump value: %s"), *InputAxisVector.ToString());

	if (GetController() != nullptr)
	{
		const FRotator YawRotation(0.0f, GetController()->GetControlRotation().Yaw, 0.0f);
		const FVector ForwardDirection(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		const FVector RightDirection(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		AddMovementInput(ForwardDirection, InputAxisVector.Y);
		AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void ABlasterCharacter::DoMouseLook(const FInputActionValue& InputActionValue)
{
	FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	//UE_LOG(LogTemp, Warning, TEXT("Jump value: %s"), *InputAxisVector.ToString());

	if (GetController() != nullptr)
	{
		AddControllerYawInput(InputAxisVector.X);
		AddControllerPitchInput(InputAxisVector.Y);
	}
}
