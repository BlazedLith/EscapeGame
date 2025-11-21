#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "KeyItem.generated.h"

UCLASS()
class ESCAPE_API AKeyItem : public AItemBase
{
    GENERATED_BODY()

public:
    AKeyItem();

    // Override the use behavior
    virtual bool OnUse(AEscapeCharacter* UsingCharacter) override;
};
