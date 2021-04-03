// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleAI.h"
#include "FireboltProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "LootPickup.h"

// Sets default values
ASimpleAI::ASimpleAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bCanBeHitWithMelee = true;

}

// Called when the game starts or when spawned
void ASimpleAI::BeginPlay()
{
	Super::BeginPlay();
	
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
	GetWorld()->SpawnActor<ALootPickup>(LootPickupActor, LootTransform, FActorSpawnParameters());

	Destroy();
}

void ASimpleAI::AIHit()
{
	bCanBeHitWithMelee = false;
	// Timer delegate and timer handle are used for the timer
	FTimerDelegate ResetCastingStateDel;

	ResetCastingStateDel = FTimerDelegate::CreateLambda([=]()
		{
			bCanBeHitWithMelee = true;
		});

	// we want this to run once per second after a 2 second delay, and not repeat
	GetWorldTimerManager().SetTimer(CanBeHitHandle, ResetCastingStateDel, 1.f, false, 1.f);
}


