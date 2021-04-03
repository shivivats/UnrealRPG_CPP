// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Sword.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "SimpleAI.h"
#include "Kismet/GameplayStatics.h"
#include "SwordCharacter.h"

// Sets default values
AWeapon_Sword::AWeapon_Sword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SwordMesh");

	RootComponent = SwordMesh;


	// Sphere Collision component
	SwordCollision = CreateDefaultSubobject<UBoxComponent>("SwordCollision");
	//SwordCollision->SetRelativeLocation(FVector(35.5f, -3.56f, -0.7f));
	//SwordCollision->SetRelativeScale3D(FVector(1.32f, 1.12f, 1.12f));


}

// Called when the game starts or when spawned
void AWeapon_Sword::BeginPlay()
{
	Super::BeginPlay();

	RootComponent = SwordMesh;

	SwordCollision->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	OnActorBeginOverlap.AddDynamic(this, &AWeapon_Sword::BeginOverlap);
	
}

// Called every frame
void AWeapon_Sword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon_Sword::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ASimpleAI* AIActor = Cast<ASimpleAI>(OtherActor);

	ASwordCharacter* PlayerCharacter = Cast<ASwordCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));

	if (PlayerCharacter != nullptr)
	{
		if (PlayerCharacter->bIsSlashing)
		{
			if (AIActor != nullptr)
			{
				if (AIActor->bCanBeHitWithMelee)
				{
					UE_LOG(LogTemp, Warning, TEXT("Sword hit AI"));

					/*
					FTransform EffectTransform;
					EffectTransform.SetLocation(GetActorLocation());
					EffectTransform.SetScale3D(FVector(0.5f));

					if (HitEffect)
					{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, EffectTransform, true);
					}
					*/

					AIActor->TakeDamage(0.25f);
					AIActor->AIHit();

				}

			}
		}
	}
}

