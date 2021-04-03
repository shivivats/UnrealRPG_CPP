// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SimpleAI.generated.h"

UCLASS()
class RPGPROJECT_CPP_API ASimpleAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASimpleAI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	class UParticleSystem* HitEffect;

	UPROPERTY(EditAnywhere, Category = "Drops")
	TSubclassOf<class ALootPickup> LootPickupActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TakeDamage(float Damage);

	void Death();

	// Overlap function
	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
