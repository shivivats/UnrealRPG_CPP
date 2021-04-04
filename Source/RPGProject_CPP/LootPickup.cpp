// Fill out your copyright notice in the Description page of Project Settings.


#include "LootPickup.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "RPGCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ALootPickup::ALootPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("LootPickupMesh");
		
	//PickupMesh->SetRelativeLocation(FVector(0.f, 0.f, -100.f));
	RootComponent = PickupMesh;

	RootComponent->SetRelativeLocation(FVector(0.f, 0.f, -30.f));

	PickupCollision = CreateDefaultSubobject<UBoxComponent>("LootPickupCollision");

	PickupParticles = CreateDefaultSubobject<UParticleSystemComponent>("LootPickupParticles");

	PickupParticles->SetWorldScale3D(FVector(0.5f));

	

}

// Called when the game starts or when spawned
void ALootPickup::BeginPlay()
{
	Super::BeginPlay();
	
	//PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::BeginOverlap);
	OnActorBeginOverlap.AddDynamic(this, &ALootPickup::BeginOverlap);

	RootComponent = PickupMesh;

	PickupCollision->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	PickupParticles->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

// Called every frame
void ALootPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALootPickup::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ARPGCharacter* PlayerCharacter = Cast<ARPGCharacter>(OtherActor);

	if (PlayerCharacter != nullptr)
	{
		FTransform EffectTransform;
		EffectTransform.SetLocation(PlayerCharacter->GetActorLocation());

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupEffect, EffectTransform, true);

		Destroy();
	}
}

