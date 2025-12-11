// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	this->CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(this->CasingMesh);
	this->CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	this->CasingMesh->SetSimulatePhysics(true);
	this->CasingMesh->SetEnableGravity(true);
	this->CasingMesh->SetNotifyRigidBodyCollision(true);

	this->ShellEjectionImpulse = 10.0f;
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
	this->CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	this->CasingMesh->AddImpulse(this->GetActorForwardVector() * this->ShellEjectionImpulse);
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (this->ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, this->ShellSound, this->GetActorLocation());
	}

	this->Destroy();
}