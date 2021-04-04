// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon_Sword.h"

// Sets default values
ASwordCharacter::ASwordCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

	bIsSlashing = false;
}

// Called every frame
void ASwordCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASwordCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Ability mappings
	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &ASwordCharacter::Ability1_Slash);
	//PlayerInputComponent->BindAction("Ability2", IE_Pressed, this, &ASwordCharacter::Ability2_Projectile);
}

void ASwordCharacter::BeginPlay()
{
	Super::BeginPlay();

	FTransform EffectTransform;
	EffectTransform = GetMesh()->GetSocketTransform("WeaponHold");

	if (SwordWeapon)
	{
		AWeapon_Sword* SpawnedSword =  GetWorld()->SpawnActor<AWeapon_Sword>(SwordWeapon, EffectTransform, FActorSpawnParameters());
		if (SpawnedSword != nullptr)
		{
			SpawnedSword->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("WeaponHold"));
		}
	}
}

void ASwordCharacter::Ability1_Slash()
{
	// If cooldown is 1, then we are allowed to slash
	if (Ability1Cooldown >= 1.f)
	{
		if (bIsSlashing == false)// && bCasting1HCombat == false)
		{
			//if (Mana >= 0.15f)
			if (true)
			{
				// Disable movement and stop character as they start to cast spell
				this->GetCharacterMovement()->DisableMovement();
				this->GetCharacterMovement()->StopMovementImmediately();

				// Subtract mana cost of spell
				//ModifyMana(-0.15f);

				// Actual spell effect here
				//ModifyHealth(0.15f);

				// Character is casting a 1-handed spell
				bIsSlashing = true;

				// Spawn the ability effect
				//FTransform EffectTransform;
				//EffectTransform.SetLocation(GetMesh()->GetSocketLocation("RightFoot"));
				//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Ability1Effect, EffectTransform, true);

				// Set the ability cooldown to 0, i.e., ability is now on max cooldown
				Ability1Cooldown = 0.f;

				// Timer delegate and timer handle are used for the timer
				FTimerDelegate ResetCastingStateDel;

				ResetCastingStateDel = FTimerDelegate::CreateLambda([=]()
					{
						ResetCastingState(bIsSlashing);
					});

				// timer params go: InRate, InbLoop, InFirstDelay
				// we want this to run once per second after a 2 second delay, and not repeat
				GetWorldTimerManager().SetTimer(CastingStateTimerHandle, ResetCastingStateDel, 1.f, false, 1.45f);

				// similarly, we set up another timer but this one for the ability cooldown
				FTimerDelegate ResetAbilityCooldownDel;

				ResetAbilityCooldownDel = FTimerDelegate::CreateLambda([=]()
					{
						ResetAbilityCooldown(Ability1Cooldown);
					});

				// we want this to run every 0.5 seconds till the cooldown reaches 1
				GetWorldTimerManager().SetTimer(AbilityCooldownTimerHandle, ResetAbilityCooldownDel, 0.05f, true, 0.f);
			}
			else
			{
				NoManaPrompt();
			}
		}
	}
}