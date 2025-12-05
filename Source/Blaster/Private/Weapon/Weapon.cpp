// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"


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

	this->PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	this->PickupWidget->SetupAttachment(this->RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (this->PickupWidget)
	{
		this->PickupWidget->SetVisibility(false);
	}

	if (HasAuthority())
	{
		this->AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		this->WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		this->AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		this->AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("AreaSphere overlapped with: %s"), *OtherActor->GetName());

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (this->PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	this->WeaponState = State;

	switch (this->WeaponState)
	{
		case EWeaponState::EWS_Equipped:
			this->ShowPickupWidget(false);
			this->AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch (this->WeaponState)
	{
		case EWeaponState::EWS_Equipped:
			this->ShowPickupWidget(false);
			break;
	}
}

USphereComponent* AWeapon::GetAreaSphere() const
{
	return this->AreaSphere;
}

USkeletalMeshComponent* AWeapon::GetWeaponMesh()
{
	return this->WeaponMesh;
}
