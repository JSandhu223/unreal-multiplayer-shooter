#include "Weapon/Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	this->CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	this->SetRootComponent(this->CollisionBox);
	this->CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	this->CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	this->CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	this->CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	this->ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	this->ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (this->Tracer)
	{
		this->TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			this->Tracer,
			this->CollisionBox,
			FName(),
			this->GetActorLocation(),
			this->GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}

	if (HasAuthority())
	{
		this->CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// The destruction of this actor will get replicated down to the clients since we set 'bReplicates = true' in the constructor
	this->Destroy();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::Destroyed()
{
	Super::Destroyed();

	if (this->ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			this->ImpactParticles,
			this->GetActorTransform()
		);
	}

	if (this->ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			this->ImpactSound,
			this->GetActorLocation()
		);
	}
}

