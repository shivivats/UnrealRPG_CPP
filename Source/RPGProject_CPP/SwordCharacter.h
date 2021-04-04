// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGCharacter.h"

#include "SwordCharacter.generated.h"

UCLASS()
class RPGPROJECT_CPP_API ASwordCharacter : public ARPGCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASwordCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	bool bIsSlashing;

	UPROPERTY(EditAnywhere, Category = "Weapons")
	TSubclassOf<class AWeapon_Sword> SwordWeapon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Magic Ability functions
	void Ability1_Slash();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
