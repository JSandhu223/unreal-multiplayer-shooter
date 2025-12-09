#include "Weapon/ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/Projectile.h"


void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	// The projectile logic should only be handled by the server to prevent clients from cheating
	if (!HasAuthority()) { return; }

	APawn* InstigatorPawn = Cast<APawn>(this->GetOwner());

	const USkeletalMeshSocket* MuzzleFlashSocket = this->GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(this->GetWeaponMesh());

		// vector from barrel to the target hit location by the line trace
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		// Get the rotation of the vector going from the barrel of the weapon to the hit location. This ensure the projectile is oriented towards to hit target when firing.
		FRotator TargetRotation = ToTarget.Rotation();

		// DEBUG
		UE_LOG(LogTemp, Warning, TEXT("ToTarget: %s"), *ToTarget.ToString());
		UE_LOG(LogTemp, Warning, TEXT("TargetRotation: %s"), *TargetRotation.ToString());

		if (this->ProjectileClass && InstigatorPawn)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this->GetOwner();
				SpawnParams.Instigator = InstigatorPawn;
				// Spawn the projectile at the tip of the weapon's barrel
				World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,				
					SpawnParams
				);
			}
		}
	}
}
