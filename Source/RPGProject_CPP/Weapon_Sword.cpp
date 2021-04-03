// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Sword.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AWeapon_Sword::AWeapon_Sword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SwordMesh");

	RootComponent = SwordMesh;
}

// Called when the game starts or when spawned
void AWeapon_Sword::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapon_Sword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

