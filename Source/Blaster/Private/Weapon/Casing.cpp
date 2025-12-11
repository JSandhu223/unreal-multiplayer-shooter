// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Casing.h"


ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	this->CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(this->CasingMesh);

}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
}
