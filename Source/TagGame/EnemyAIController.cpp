// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "TagGameGameMode.h"

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    GoToPlayerState = MakeShared<FAivState>(
        [](AAIController* AIController) {
            AIController->MoveToActor(AIController->GetWorld()->GetFirstPlayerController()->GetPawn(), 100.0f);
        },
        nullptr,
        [this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
            EPathFollowingStatus::Type Status = AIController->GetMoveStatus();

            if (Status == EPathFollowingStatus::Moving)
            {
                return nullptr;
            }

            if (TargetBall)
            {
                TargetBall->AttachToActor(AIController->GetWorld()->GetFirstPlayerController()->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
                TargetBall->SetActorRelativeLocation(FVector::Zero());

                AGameModeBase* GameModeBase = AIController->GetWorld()->GetAuthGameMode();
                ATagGameGameMode* TagGameMode = Cast<ATagGameGameMode>(GameModeBase);
                TagGameMode->CheckForEndGame();

                TargetBall = nullptr;
            }

            return SearchFreeBallState;
        }
    );

    SearchFreeBallState = MakeShared<FAivState>(
        [this](AAIController* AIController) {
            AGameModeBase* GameModeBase = AIController->GetWorld()->GetAuthGameMode();
            ATagGameGameMode* TagGameMode = Cast<ATagGameGameMode>(GameModeBase);
            const TArray<ABallGrab*>& BallList = TagGameMode->GetBalls();

            ABallGrab* ClosestBall = nullptr;

            for (int32 i = 0; i < BallList.Num(); i++)
            {
                if (!BallList[i]->GetAttachParentActor() &&
                    (!ClosestBall ||
                        FVector::Distance(AIController->GetPawn()->GetActorLocation(), BallList[i]->GetActorLocation()) <
                        FVector::Distance(AIController->GetPawn()->GetActorLocation(), ClosestBall->GetActorLocation())))
                {
                    ClosestBall = BallList[i];
                }
            }

            TargetBall = ClosestBall;
        },
        nullptr,
        [this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
            if (TargetBall)
            {
                return GoToBallState;
            }

            SearchFreeBallState->CallEnter(this);
            return SearchFreeBallState;
        }
    );

    GoToBallState = MakeShared<FAivState>(
        [this](AAIController* AIController) {
            AIController->MoveToActor(TargetBall, 100.0f);
        },
        nullptr,
        [this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
            EPathFollowingStatus::Type Status = AIController->GetMoveStatus();

            if (Status == EPathFollowingStatus::Moving)
            {
                return nullptr;
            }

            return TryToGrabBallState;
        }
    );

    TryToGrabBallState = MakeShared<FAivState>(
        [this](AAIController* AIController) {
            if (TargetBall->GetAttachParentActor())
            {
                TargetBall = nullptr;
            }
        },
        nullptr,
        [this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
            if (!TargetBall)
            {
                return SearchFreeBallState;
            }

            TargetBall->AttachToActor(AIController->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
            TargetBall->SetActorRelativeLocation(FVector::Zero());

            return GoToPlayerState;
        }
    );

    CurrentState = SearchFreeBallState;
    CurrentState->CallEnter(this);
}

void AEnemyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState)
    {
        CurrentState = CurrentState->CallTick(this, DeltaTime);
    }
}