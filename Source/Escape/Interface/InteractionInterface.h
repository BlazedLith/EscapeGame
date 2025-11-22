#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for anything the player can 'E' on.
 */
class ESCAPE_API IInteractionInterface
{
    GENERATED_BODY()

public:
    // The function every interactable object must implement
    // Returns true if interaction was successful
    virtual void OnInteract(class AEscapeCharacter* Caller) = 0;

    // Optional: Text to show on screen (e.g., "Press E to Open")
    virtual FText GetInteractionPrompt() const { return FText::FromString("Interact"); }
};