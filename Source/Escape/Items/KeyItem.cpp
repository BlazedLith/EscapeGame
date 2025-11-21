#include "KeyItem.h"
#include "EscapeCharacter.h"

AKeyItem::AKeyItem()
{
    ItemType = EItemType::Key;
}

bool AKeyItem::OnUse(AEscapeCharacter* UsingCharacter)
{
    // For now, this simply returns true
    // Later you'll implement: door unlocking logic here

    UE_LOG(LogTemp, Warning, TEXT("KeyItem used by player."));

    // Destroy the key after use if enabled
    if (bDestroyOnUse)
    {
        Destroy();
    }

    return true;
}
