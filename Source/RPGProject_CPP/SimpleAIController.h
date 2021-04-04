// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SimpleAIController.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_CPP_API ASimpleAIController : public AAIController
{
	GENERATED_BODY()

private:

	class UBehaviorTreeComponent* BehaviorComp;

	class UBlackboardComponent* BlackboardComp;

public:
	
	ASimpleAIController();

	/*Blackboard key*/
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName BlackboardKey;

	/*Executes right when the controller possess a Pawn*/
	virtual void OnPossess(APawn* PossessedPawn) override;

	/*Sets the sensed target in the blackboard*/
	void SetSeenTarget(APawn* SeenPawn);
	
};
