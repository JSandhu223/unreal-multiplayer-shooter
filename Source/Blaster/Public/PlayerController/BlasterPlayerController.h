// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABlasterPlayerController();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

private:
	// Remote Procedure Call (RPC) will be called on the server and executed on the client
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

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

	void EquipButtonPressed(const FInputActionValue& InputActionValue);
};
