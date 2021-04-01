// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGProject_CPPCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "FireboltProjectile.h"

//////////////////////////////////////////////////////////////////////////
// ARPGProject_CPPCharacter

ARPGProject_CPPCharacter::ARPGProject_CPPCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Set Attributes

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
	
	// Initially abilities are off cooldown 
	Ability1Cooldown = 1.f;
	Ability2Cooldown = 1.f;

	//UE_LOG(LogTemp, Warning, TEXT("Variables set"));
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARPGProject_CPPCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Ability mappings
	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &ARPGProject_CPPCharacter::Ability1_Heal);
	PlayerInputComponent->BindAction("Ability2", IE_Pressed, this, &ARPGProject_CPPCharacter::Ability2_Projectile);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARPGProject_CPPCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARPGProject_CPPCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARPGProject_CPPCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARPGProject_CPPCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ARPGProject_CPPCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ARPGProject_CPPCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ARPGProject_CPPCharacter::OnResetVR);
}

void ARPGProject_CPPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// When you multiply something with delta time that value becomes "units per second"

	HealthRegen(DeltaTime);

	ManaRegen(DeltaTime);
}


void ARPGProject_CPPCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ARPGProject_CPPCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ARPGProject_CPPCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ARPGProject_CPPCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProject_CPPCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProject_CPPCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ARPGProject_CPPCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ARPGProject_CPPCharacter::Ability1_Heal()
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
				this->GetCharacterMovement()->DisableMovement();
				this->GetCharacterMovement()->StopMovementImmediately();

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
				//ResetCastingStateDel = FTimerDelegate::CreateUObject(this, &ARPGProject_CPPCharacter::ResetCastingState, bCasting1H);
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

				//ResetAbilityCooldownDel = FTimerDelegate::CreateUObject(this, &ARPGProject_CPPCharacter::ResetAbilityCooldown, Ability1Cooldown);
				ResetAbilityCooldownDel = FTimerDelegate::CreateLambda([=]()
					{
						ResetAbilityCooldown(Ability1Cooldown);
					});

				// we want this to run every 0.5 seconds till the cooldown reaches 1
				GetWorldTimerManager().SetTimer(AbilityCooldownTimerHandle, ResetAbilityCooldownDel, 0.5f, true, 0.f);
			}
			else
			{
				// TODO: No mana prompt
			}
		}
	}
}

void ARPGProject_CPPCharacter::Ability2_Projectile()
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
				this->GetCharacterMovement()->DisableMovement();
				this->GetCharacterMovement()->StopMovementImmediately();

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
				//ResetCastingStateDel = FTimerDelegate::CreateUObject(this, &ARPGProject_CPPCharacter::ResetCastingState, bCasting1H);
				ResetCastingStateDel = FTimerDelegate::CreateLambda([=]()
					{
						ResetCastingState(bCasting1HCombat);
					});

				// this is currently once per second, starting two seconds from now
				// timer params go: InRate, InbLoop, InFirstDelay
				// we want this to run once per second after a 1.4 second delay, and not repeat
				GetWorldTimerManager().SetTimer(CastingStateTimerHandle, ResetCastingStateDel, 1.f, false, 2.f);

				// similarly, we set up another timer but this one for the ability cooldown
				FTimerDelegate ResetAbilityCooldownDel;

				//ResetAbilityCooldownDel = FTimerDelegate::CreateUObject(this, &ARPGProject_CPPCharacter::ResetAbilityCooldown, Ability1Cooldown);
				ResetAbilityCooldownDel = FTimerDelegate::CreateLambda([=]()
					{
						ResetAbilityCooldown(Ability2Cooldown);
					});

				// we want this to run every 0.5 seconds till the cooldown reaches 1
				GetWorldTimerManager().SetTimer(AbilityCooldownTimerHandle, ResetAbilityCooldownDel, 0.1f, true, 0.f);
			}
			else
			{
				// TODO: No mana prompt
			}
		}
	}
}

void ARPGProject_CPPCharacter::ResetCastingState(bool &bCastingState)
{
	// here we reset the casting state that was given to us
	bCastingState = false;

	// then we set the movement mode to walking to enable movement again
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	//UE_LOG(LogTemp, Warning, TEXT("bool timer run"));

	// clear the timer bc we only wanna run this function once
	//GetWorldTimerManager().ClearTimer(CastingStateTimerHandle);


}

void ARPGProject_CPPCharacter::ResetAbilityCooldown(float &AbilityCooldown)
{
	// keep adding to the cooldown at a rate of 0.05 per every 0.5 second (so 0.1 every 1 second), till it reaches 1
	if (AbilityCooldown < 1.f) 
	{
		AbilityCooldown += 0.05f;
	}
	else
	{
		//AbilityCooldown = 1f;
		// if it has reached 1 then we just clear this timer handle to stop it
		GetWorldTimerManager().ClearTimer(AbilityCooldownTimerHandle);
	}
}

void ARPGProject_CPPCharacter::HealthRegen(float DeltaTime)
{
	if (Health < 1.f)
	{
		ModifyHealth(DeltaTime * HealthRegenAmount);
	}
}

void ARPGProject_CPPCharacter::ManaRegen(float DeltaTime)
{
	if (Mana < 1.f)
	{
		ModifyMana(DeltaTime * ManaRegenAmount);
	}
}

void ARPGProject_CPPCharacter::ModifyHealth(float Modifier)
{
	Health += Modifier;
	if (Health > 1.f)
	{
		Health = 1.f;
	}
	if (Health < 0.f)
	{
		Health = 0.f;
	}

}

void ARPGProject_CPPCharacter::ModifyMana(float Modifier)
{
	Mana += Modifier;
	if (Mana > 1.f)
	{
		Mana = 1.f;
	}
	if (Mana < 0.f)
	{
		Mana = 0.f;
	}
}

bool ARPGProject_CPPCharacter::IsHealthFull()
{
	if (Health == 1.f)
	{
		return true;
	}
	else 
	{
		return false;
	}
}
