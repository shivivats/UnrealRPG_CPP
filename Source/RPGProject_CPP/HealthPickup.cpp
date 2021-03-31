// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "RPGProject_CPPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AHealthPickup::AHealthPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Particle System component, set from blueprint
	PickupParticles = CreateDefaultSubobject<UParticleSystemComponent>("PickupParticles");

	SetRootComponent(PickupParticles);

	// Sphere Collision component
	PickupCollision = CreateDefaultSubobject<UBoxComponent>("PickupCollision");


}

// Called when the game starts or when spawned
void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
	//PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::BeginOverlap);
	OnActorBeginOverlap.AddDynamic(this, &AHealthPickup::BeginOverlap);	



	PickupCollision->SetupAttachment(PickupParticles);
	PickupCollision->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

	PickupCollision->SetWorldScale3D(FVector(1.8f, 1.8f, 1.8f));

	//PickupCollision->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	//PickupCollision->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	//PickupCollision->SetRelativeScale3D(FVector(1.8f, 1.8f, 1.8f));
}

// Called every frame
void AHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealthPickup::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("overlapped with something"));

	ARPGProject_CPPCharacter* PlayerCharacter = Cast<ARPGProject_CPPCharacter>(OtherActor);
	
	if (PlayerCharacter != nullptr)
	{
		if (!PlayerCharacter->IsHealthFull())
		{
			PlayerCharacter->ModifyHealth(0.2f);

			FTransform EffectTransform;
			EffectTransform.SetLocation(PlayerCharacter->GetActorLocation());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupEffect, EffectTransform, true);

			Destroy();
		}
		
	}
}

