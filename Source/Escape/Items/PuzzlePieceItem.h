#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "PuzzlePieceItem.generated.h"

UCLASS()
class ESCAPE_API APuzzlePieceItem : public AItemBase
{
    GENERATED_BODY()

public:
    APuzzlePieceItem();

    // Override for puzzle piece behavior
    virtual bool OnUse(AEscapeCharacter* UsingCharacter) override;
};
