#include "DataCube.h"

ADataCube::ADataCube()
{
    PrimaryActorTick.bCanEverTick = true;
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    // Physics enabled so we can carry it
    MeshComp->SetSimulatePhysics(true);
    MeshComp->SetMobility(EComponentMobility::Movable);
}

void ADataCube::BeginPlay()
{
    Super::BeginPlay();
    // Remember where we started so we can return here if we fall
    StartLocation = GetActorLocation();
}

void ADataCube::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // If the cube falls into the void (Z < -2000)
    if (GetActorLocation().Z < -2000.0f)
    {
        // 1. Stop it from moving (Kill physics velocity)
        MeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
        MeshComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

        // 2. Teleport it home
        SetActorLocation(StartLocation);
    }
}