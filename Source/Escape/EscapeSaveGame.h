#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "EscapeSaveGame.generated.h"

UCLASS()
class ESCAPE_API UEscapeSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UEscapeSaveGame();

    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString SaveSlotName;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    uint32 UserIndex;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    FName CurrentLevelName;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    TArray<FName> InventoryItems;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    FVector PlayerLocation;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    FRotator PlayerRotation;
};