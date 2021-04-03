// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "SpellCharacter.h"
#include "SwordCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AHealthPickup::AHealthPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Particle System component, set from blueprint
	PickupParticles = CreateDefaultSubobject<UParticleSystemComponent>("PickupParticles");

	//tRootComponent(PickupParticles);
	RootComponent = PickupParticles;

	// Sphere Collision component
	PickupCollision = CreateDefaultSubobject<UBoxComponent>("PickupCollision");

	PickupCollision->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	PickupCollision->SetWorldScale3D(FVector(1.8f, 1.8f, 1.8f));
}

// Called when the game starts or when spawned
void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
	//PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::BeginOverlap);
	OnActorBeginOverlap.AddDynamic(this, &AHealthPickup::BeginOverlap);	

	RootComponent = PickupParticles;

	PickupCollision->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);



}

// Called every frame
void AHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealthPickup::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//UE_LOG(LogTemp, Warning, TEXT("overlapped with something"));

	ASpellCharacter* PlayerSpellCharacter = Cast<ASpellCharacter>(OtherActor);
	
	if (PlayerSpellCharacter != nullptr)
	{
		if (!PlayerSpellCharacter->IsHealthFull())
		{
			PlayerSpellCharacter->ModifyHealth(0.2f);

			FTransform EffectTransform;
			EffectTransform.SetLocation(PlayerSpellCharacter->GetActorLocation());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupEffect, EffectTransform, true);

			Destroy();
		}
		
	}
	else
	{
		ASwordCharacter* PlayerSwordCharacter = Cast<ASwordCharacter>(OtherActor);
		if (PlayerSwordCharacter != nullptr)
		{
			if (!PlayerSwordCharacter->IsHealthFull())
			{
				PlayerSwordCharacter->ModifyHealth(0.2f);

				FTransform EffectTransform;
				EffectTransform.SetLocation(PlayerSwordCharacter->GetActorLocation());
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupEffect, EffectTransform, true);

				Destroy();
			}
		}
	}
}

