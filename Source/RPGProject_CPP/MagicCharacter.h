// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MagicCharacter.generated.h"

UCLASS(config=Game)
class AMagicCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


public:
	AMagicCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float HealthRegenAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float Mana;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float ManaRegenAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	bool bCasting1H;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	bool bCasting1HCombat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Magic")
	float Ability1Cooldown;

	UPROPERTY(EditAnywhere, Category="Magic")
	class UParticleSystem* Ability1Effect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Magic")
	float Ability2Cooldown;

	UPROPERTY(EditAnywhere, Category = "Magic")
	TSubclassOf<class AFireboltProjectile> Ability2Actor;

	FTimerHandle CastingStateTimerHandle;
	FTimerHandle AbilityCooldownTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Magic")
	TSubclassOf<class UUserWidget> NoManaWidget;


protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	// Magic Ability functions
	void Ability1_Heal();
	void Ability2_Projectile();

	// Ability helper functions
	UFUNCTION()
	void ResetCastingState(bool &bCastingState);

	UFUNCTION()
	void ResetAbilityCooldown(float &AbilityCooldown);

	void HealthRegen(float DeltaTime);

	void ManaRegen(float DeltaTime);

	void NoManaPrompt();


	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ModifyHealth(float Modifier);

	void ModifyMana(float Modifier);

	bool IsHealthFull();
};

