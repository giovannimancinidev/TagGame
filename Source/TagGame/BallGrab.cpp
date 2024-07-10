// Fill out your copyright notice in the Description page of Project Settings.


#include "BallGrab.h"

// Sets default values
ABallGrab::ABallGrab()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BallGrabMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = BallGrabMesh;
}

// Called when the game starts or when spawned
void ABallGrab::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABallGrab::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

