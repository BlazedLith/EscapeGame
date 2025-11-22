#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "Components/TextRenderComponent.h"
#include "PasswordSafe.generated.h"

UCLASS()
class ESCAPE_API APasswordSafe : public AActor, public IInteractionInterface
{
    GENERATED_BODY()

public:
    APasswordSafe();
    virtual void OnInteract(class AEscapeCharacter* Caller) override;

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* SafeMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* DigitDisplay;

    UPROPERTY(EditAnywhere, Category = "Loot")
    TSubclassOf<class AItemBase> ItemToSpawn;

    // --- NEW DIFFICULTY SETTINGS ---
    // Instead of a single int, we use a Sequence string
    UPROPERTY(EditAnywhere, Category = "Lock")
    FString TargetSequence = "482";

    int32 CurrentDialNumber = 0;
    int32 CurrentSequenceIndex = 0; // Which digit are we solving? (0, 1, or 2)

    bool bIsCracked = false;

    // Timer to auto-submit the number
    FTimerHandle SubmissionTimer;

    void UpdateDisplay();
    void CheckSubmission(); // The Algorithm
    void ResetPuzzle();
};