// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickup.generated.h"

UCLASS()
class RPGPROJECT_CPP_API AHealthPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealthPickup();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UParticleSystemComponent* PickupParticles;


	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* PickupCollision;

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* PickupEffect;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Overlap function
	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

};
