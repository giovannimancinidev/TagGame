// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/TargetPoint.h"
#include "BallGrab.h"
#include "TagGameGameMode.generated.h"

UCLASS(minimalapi)
class ATagGameGameMode : public AGameModeBase
{
    GENERATED_BODY()

protected:
    TArray<ATargetPoint*> SpawnPoints;
    TArray<ABallGrab*> BallActors;

    void ResetMatch();

public:
    ATagGameGameMode();

    void BeginPlay() override;

    void Tick(float DeltaTime) override;

    void CheckForEndGame();

    const TArray<ABallGrab*>& GetBalls() const;
};