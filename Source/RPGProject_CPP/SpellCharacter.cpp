// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FireboltProjectile.h"


// Sets default values
ASpellCharacter::ASpellCharacter()
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set attributes for different characters

	// Initial Health
	Health = 0.3f;

	// Regen this much health per second
	HealthRegenAmount = 0.01f;

	// Initial Mana
	Mana = 0.7f;

	// Regen this much mana per second
	ManaRegenAmount = 0.01f;

	// Initially we are not casting anything
	bCasting1H = false;
	bCasting1HCombat = false;

}

// Called when the game starts or when spawned
void ASpellCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpellCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
}

// Called to bind functionality to input
void ASpellCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Ability mappings
	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &ASpellCharacter::Ability1_Heal);
	PlayerInputComponent->BindAction("Ability2", IE_Pressed, this, &ASpellCharacter::Ability2_Projectile);
}

void ASpellCharacter::Ability1_Heal()
{
	// If cooldown is 1, then we are allowed to cast
	if (Ability1Cooldown >= 1.f)
	{
		// can only cast a spell if we're not already casting one
		if (bCasting1H == false && bCasting1HCombat == false)
		{
			if (Mana >= 0.15f)
			{
				// Disable movement and stop character as they start to cast spell
				//this->GetCharacterMovement()->DisableMovement();
				//this->GetCharacterMovement()->StopMovementImmediately();

				// Subtract mana cost of spell
				ModifyMana(-0.15f);

				// Actual spell effect here
				ModifyHealth(0.15f);

				// Character is casting a 1-handed spell
				bCasting1H = true;

				// Spawn the ability effect
				FTransform EffectTransform;
				EffectTransform.SetLocation(GetMesh()->GetSocketLocation("RightFoot"));
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Ability1Effect, EffectTransform, true);

				// Set the ability cooldown to 0, i.e., ability is now on max cooldown
				Ability1Cooldown = 0.f;

				// Timer delegate and timer handle are used for the timer
				FTimerDelegate ResetCastingStateDel;

				//UE_LOG(LogTemp, Warning, TEXT("bool timer should run"));

				//ResetCastingStateDel.BindUFunction(this, FName("ResetCastingState"), &bCasting1H);
				//ResetCastingStateDel = FTimerDelegate::CreateUObject(this, &ASpellCharacter::ResetCastingState, bCasting1H);
				ResetCastingStateDel = FTimerDelegate::CreateLambda([=]()
					{
						ResetCastingState(bCasting1H);
					});

				// this is currently once per second, starting two seconds from now
				// timer params go: InRate, InbLoop, InFirstDelay
				// we want this to run once per second after a 2 second delay, and not repeat
				GetWorldTimerManager().SetTimer(CastingStateTimerHandle, ResetCastingStateDel, 1.f, false, 2.f);

				// similarly, we set up another timer but this one for the ability cooldown
				FTimerDelegate ResetAbilityCooldownDel;

				//ResetAbilityCooldownDel = FTimerDelegate::CreateUObject(this, &ASpellCharacter::ResetAbilityCooldown, Ability1Cooldown);
				ResetAbilityCooldownDel = FTimerDelegate::CreateLambda([=]()
					{
						ResetAbilityCooldown(Ability1Cooldown);
					});

				// we want this to run every 0.5 seconds till the cooldown reaches 1
				GetWorldTimerManager().SetTimer(AbilityCooldownTimerHandle, ResetAbilityCooldownDel, 0.5f, true, 0.f);
			}
			else
			{
				NoManaPrompt();
			}
		}
	}
}

void ASpellCharacter::Ability2_Projectile()
{
	// If cooldown is 1, then we are allowed to cast
	if (Ability2Cooldown >= 1.f)
	{
		// can only cast a spell if we're not already casting one
		if (bCasting1H == false && bCasting1HCombat == false)
		{
			if (Mana >= 0.15f)
			{
				// Disable movement and stop character as they start to cast spell
				//this->GetCharacterMovement()->DisableMovement();
				//this->GetCharacterMovement()->StopMovementImmediately();

				// Subtract mana cost of spell
				ModifyMana(-0.15f);

				// Character is casting a 1-handed combat spell
				bCasting1HCombat = true;

				// Actual spell effect here: Spawn the ability effect after a delay
				FTransform EffectTransform;
				EffectTransform.SetLocation(GetMesh()->GetSocketLocation("RightHand"));
				EffectTransform.SetRotation(GetActorTransform().GetRotation());
				EffectTransform.SetScale3D(GetActorTransform().GetScale3D());

				FTimerHandle AnimDelayHandle;
				GetWorld()->GetTimerManager().SetTimer(AnimDelayHandle, [this, EffectTransform]() {
					GetWorld()->SpawnActor<AFireboltProjectile>(Ability2Actor, EffectTransform, FActorSpawnParameters());
					//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Ability2Effect, EffectTransform, true);
					}, 1.f, false, 0.9f);


				// Set the ability cooldown to 0, i.e., ability is now on max cooldown
				Ability2Cooldown = 0.f;

				// Timer delegate and timer handle are used for the timer
				FTimerDelegate ResetCastingStateDel;

				//UE_LOG(LogTemp, Warning, TEXT("bool timer should run"));

				//ResetCastingStateDel.BindUFunction(this, FName("ResetCastingState"), &bCasting1H);
				//ResetCastingStateDel = FTimerDelegate::CreateUObject(this, &ASpellCharacter::ResetCastingState, bCasting1H);
				ResetCastingStateDel = FTimerDelegate::CreateLambda([=]()
					{
						ResetCastingState(bCasting1HCombat);
					});

				// this is currently once per second, starting two seconds from now
				// timer params go: InRate, InbLoop, InFirstDelay
				// we want this to run once per second after a 1.4 second delay, and not repeat
				GetWorldTimerManager().SetTimer(CastingStateTimerHandle, ResetCastingStateDel, 1.f, false, 1.4f);

				// similarly, we set up another timer but this one for the ability cooldown
				FTimerDelegate ResetAbilityCooldownDel;

				//ResetAbilityCooldownDel = FTimerDelegate::CreateUObject(this, &ASpellCharacter::ResetAbilityCooldown, Ability1Cooldown);
				ResetAbilityCooldownDel = FTimerDelegate::CreateLambda([=]()
					{
						ResetAbilityCooldown(Ability2Cooldown);
					});

				// we want this to run every 0.1 seconds till the cooldown reaches 1
				GetWorldTimerManager().SetTimer(AbilityCooldownTimerHandle, ResetAbilityCooldownDel, 0.2f, true, 0.f);
			}
			else
			{
				NoManaPrompt();
			}
		}
	}
}