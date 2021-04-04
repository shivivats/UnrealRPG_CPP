// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleAI.h"
#include "FireboltProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "LootPickup.h"
#include "RPGCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "SimpleAIController.h"
#include "AIController.h"
#include "AITypes.h"


// Sets default values
ASimpleAI::ASimpleAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bCanBeHitWithMelee = true;

	PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensor");
	PawnSensor->SetPeripheralVisionAngle(45.f);
	PawnSensor->SightRadius = 900.f;

}

// Called when the game starts or when spawned
void ASimpleAI::BeginPlay()
{
	Super::BeginPlay();
	if (PawnSensor)
	{
		PawnSensor->OnSeePawn.AddDynamic(this, &ASimpleAI::OnSeePawn);
		PawnSensor->OnHearNoise.AddDynamic(this, &ASimpleAI::OnHearNoise);	
	}
}

// Called every frame
void ASimpleAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
//void ASimpleAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

void ASimpleAI::ModifyHealth(float Damage)
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

void ASimpleAI::OnHearNoise(APawn* OtherActor, const FVector& Location, float Volume)
{
}

void ASimpleAI::OnSeePawn(APawn* OtherPawn)
{

	ASimpleAIController* AIController = Cast<ASimpleAIController>(GetController());
	ARPGCharacter* PlayerCharacter = Cast<ARPGCharacter>(OtherPawn);

	if (AIController && PlayerCharacter)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(PlayerCharacter);

		AIController->MoveTo(MoveRequest);
	}
		
}


