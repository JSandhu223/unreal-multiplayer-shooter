#include "Weapon/Projectile.h"
#include "Components/BoxComponent.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	this->CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	this->SetRootComponent(this->CollisionBox);
	this->CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	this->CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	this->CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	this->CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

