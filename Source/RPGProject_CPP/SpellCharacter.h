// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGCharacter.h"

#include "SpellCharacter.generated.h"

UCLASS()
class RPGPROJECT_CPP_API ASpellCharacter : public ARPGCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpellCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	bool bCasting1H;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	bool bCasting1HCombat;

	UPROPERTY(EditAnywhere, Category="Magic")
	class UParticleSystem* Ability1Effect;

	UPROPERTY(EditAnywhere, Category = "Magic")
	TSubclassOf<class AFireboltProjectile> Ability2Actor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Magic Ability functions
	void Ability1_Heal();

	void Ability2_Projectile();
		
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
