// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleAI.h"
#include "FireboltProjectile.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASimpleAI::ASimpleAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASimpleAI::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &ASimpleAI::BeginOverlap);
	
}

// Called every frame
void ASimpleAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASimpleAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASimpleAI::TakeDamage(float Damage)
{
	Health -= Damage;
	if (Health <= 0.f)
	{
		Death();
	}
}

void ASimpleAI::Death()
{
	FTransform LootTransform;
	LootTransform.SetLocation(GetMesh()->GetSocketLocation("foot_r"));
	
	// Spawn Loot Pickup here
	//GetWorld()->SpawnActor<AFireboltProjectile>(Ability2Actor, EffectTransform, FActorSpawnParameters());

	Destroy();
}

void ASimpleAI::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	AFireboltProjectile* MagicProjectile = Cast<AFireboltProjectile>(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("Uhh, something hit"));


	if (MagicProjectile != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Uhh, magic missile hit"));
		FTransform EffectTransform;
		EffectTransform.SetLocation(MagicProjectile->GetActorLocation());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, EffectTransform, true);

		GetWorld()->DestroyActor(MagicProjectile);

		TakeDamage(0.25f);
	}
}

