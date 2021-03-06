// Fill out your copyright notice in the Description page of Project Settings.


#include "FireboltProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "SimpleAI.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFireboltProjectile::AFireboltProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Particle System component, set from blueprint
	ProjectileParticles = CreateDefaultSubobject<UParticleSystemComponent>("ProjectileParticles");
	//SetRootComponent(ProjectileParticles);

	ProjectileParticles->SetRelativeLocation(FVector(4.f, 0.f, -14.f));

	// Sphere Collision component
	ProjectileCollision = CreateDefaultSubobject<USphereComponent>("ProjectileCollision");
	ProjectileCollision->SetRelativeLocation(FVector(35.5f, -3.56f, -0.7f));
	ProjectileCollision->SetRelativeScale3D(FVector(1.32f, 1.12f, 1.12f));

	// Projectile Movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;



}

// Called when the game starts or when spawned
void AFireboltProjectile::BeginPlay()
{
	Super::BeginPlay();

	RootComponent = ProjectileParticles;

	ProjectileCollision->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	OnActorBeginOverlap.AddDynamic(this, &AFireboltProjectile::BeginOverlap);
}

// Called every frame
void AFireboltProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFireboltProjectile::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ASimpleAI* AIActor = Cast<ASimpleAI>(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("Firebolt hit something"));

	if (AIActor != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Firebolt hit AI"));
		FTransform EffectTransform;
		EffectTransform.SetLocation(GetActorLocation());
		EffectTransform.SetScale3D(FVector(0.5f));

		if (HitEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, EffectTransform, true);
		}

		AIActor->ModifyHealth(0.25f);

		Destroy();
	}

}

