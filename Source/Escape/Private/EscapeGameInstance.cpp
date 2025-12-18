#include "EscapeGameInstance.h"
#include "EscapeSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UEscapeGameInstance::StartNewGame()
{
    // 1. Create a FRESH SaveGame object
    if (UEscapeSaveGame* NewSave = Cast<UEscapeSaveGame>(UGameplayStatics::CreateSaveGameObject(UEscapeSaveGame::StaticClass())))
    {
        // 2. Set Default Values (Start of game)
        NewSave->CurrentLevelName = FName("Lvl_FirstPerson");
        NewSave->PlayerLocation = FVector(0.f, 0.f, 100.f); // Default start spot
        NewSave->InventoryItems.Empty(); // Empty inventory

        // 3. Overwrite any existing save file
        UGameplayStatics::SaveGameToSlot(NewSave, SaveSlotName, 0);

        // 4. Open the First Level
        UGameplayStatics::OpenLevel(this, NewSave->CurrentLevelName);

        UE_LOG(LogTemp, Log, TEXT("StartNewGame: Created new save and opening Level 1"));
    }
}

void UEscapeGameInstance::LoadSavedGame()
{
    // 1. Check if save exists
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
    {
        // 2. Load it to read the Level Name
        if (UEscapeSaveGame* LoadedSave = Cast<UEscapeSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0)))
        {
            // 3. Open the level stored in the save file
            // The Character will handle restoring position in its BeginPlay
            UGameplayStatics::OpenLevel(this, LoadedSave->CurrentLevelName);

            UE_LOG(LogTemp, Log, TEXT("LoadSavedGame: Found save, opening level: %s"), *LoadedSave->CurrentLevelName.ToString());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadSavedGame: No Save Game found!"));
    }
}