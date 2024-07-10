// Copyright Epic Games, Inc. All Rights Reserved.

#include "TagGameGameMode.h"
#include "TagGameCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"

ATagGameGameMode::ATagGameGameMode()
{
    PrimaryActorTick.bCanEverTick = false;

    // set default pawn class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
}

void ATagGameGameMode::BeginPlay()
{
    Super::BeginPlay();

    ResetMatch();
}

void ATagGameGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    for (int32 i = 0; i < BallActors.Num(); i++)
    {
        if (BallActors[i]->GetAttachParentActor() != GetWorld()->GetFirstPlayerController()->GetPawn())
        {
            return;
        }
    }

    ResetMatch();
}

void ATagGameGameMode::ResetMatch()
{
    SpawnPoints.Empty();
    BallActors.Empty();

    // Search for each TargetPoint in game
    for (TActorIterator<ATargetPoint> It(GetWorld()); It; ++It)
    {
        SpawnPoints.Add(*It);
    }

    // Search for each AGrabbable in game
    for (TActorIterator<ABallGrab> It(GetWorld()); It; ++It)
    {
        if (It->GetAttachParentActor())
        {
            It->DetachRootComponentFromParent(true);
        }

        BallActors.Add(*It);
    }

    TArray<ATargetPoint*> RandomTargetPoints = SpawnPoints;

    for (int32 i = 0; i < BallActors.Num(); i++)
    {
        const int32 RandomTargetIndex = FMath::RandRange(0, RandomTargetPoints.Num() - 1);

        BallActors[i]->SetActorLocation(RandomTargetPoints[RandomTargetIndex]->GetActorLocation());

        RandomTargetPoints.RemoveAt(RandomTargetIndex);
    }
}

void ATagGameGameMode::CheckForEndGame()
{
    for (int32 i = 0; i < BallActors.Num(); i++)
    {
        if (BallActors[i]->GetAttachParentActor() != GetWorld()->GetFirstPlayerController()->GetPawn())
        {
            return;
        }
    }

    ResetMatch();
}

const TArray<ABallGrab*>& ATagGameGameMode::GetBalls() const
{
    return BallActors;
}