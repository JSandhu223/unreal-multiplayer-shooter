// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"
#include "Components/SphereComponent.h"


// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// The Weapon objects will be managed by the server
	this->bReplicates = true;

	this->WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	this->SetRootComponent(WeaponMesh);
	// The weapon will be allowed to be dropped and bounce off the world geometry
	this->WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	// The weapon collision settings should ignore any pawns when dropped
	this->WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// Initially, the weapon mesh should have its collision disabled since players will be picking up the weapon
	this->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// NOTE: Only the SERVER should handle overlap events with the AreaSphere
	this->AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	this->AreaSphere->SetupAttachment(this->RootComponent);
	this->AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		this->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		this->WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

