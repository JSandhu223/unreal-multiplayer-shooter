// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/BlasterHUD.h"


void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw crosshairs

}

void ABlasterHUD::SetHUDPackage(const FHUDPackage& Package)
{
	this->HUDPackage = Package;
}
