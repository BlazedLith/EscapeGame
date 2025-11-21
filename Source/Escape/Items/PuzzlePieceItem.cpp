#include "PuzzlePieceItem.h"
#include "EscapeCharacter.h"

APuzzlePieceItem::APuzzlePieceItem()
{
    ItemType = EItemType::PuzzlePiece;

    // Puzzle pieces should NOT destroy on use
    bDestroyOnUse = false;
}

bool APuzzlePieceItem::OnUse(AEscapeCharacter* UsingCharacter)
{
    UE_LOG(LogTemp, Warning, TEXT("Puzzle piece used (maybe show UI popup or clue)."));

    // Later you will tie this into a Notebook UI or clue system
    return true;
}
