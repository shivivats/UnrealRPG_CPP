// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "SimpleAI.h"

ASimpleAIController::ASimpleAIController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));

	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
}

void ASimpleAIController::OnPossess(APawn* PossessedPawn)
{
	Super::OnPossess(PossessedPawn);

	// Get the possessed Pawn. If its the AI Character we craeted, initialise its blackboard and start its corresponding behaviour tree
	ASimpleAI* AICharacter = Cast<ASimpleAI>(PossessedPawn);
	if (AICharacter)
	{
		/*
		if (AICharacter->BehaviorTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*(AICharacter->BehaviorTree->BlackboardAsset));
			BehaviorComp->StartTree(*AICharacter->BehaviorTree);

		}
		*/
	}
}

void ASimpleAIController::SetSeenTarget(APawn* SeenPawn)
{
	/*
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(BlackboardKey, Pawn);
	}
	*/
}
