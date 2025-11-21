// ItemBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    None UMETA(DisplayName = "None"),
    Key UMETA(DisplayName = "Key"),
    PuzzlePiece UMETA(DisplayName = "Puzzle Piece"),
    Document UMETA(DisplayName = "Document"),
    Tool UMETA(DisplayName = "Tool"),
    Health UMETA(DisplayName = "Health")
};

UCLASS()
class ESCAPE_API AItemBase : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AItemBase();

protected:

    // Mesh the player sees in the world
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    UStaticMeshComponent* Mesh;

    // Icon for inventory UI
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UTexture2D* Icon;

    // What type of item this is
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    EItemType ItemType = EItemType::None;

    // Destroy actor after use?
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    bool bDestroyOnUse = true;

protected:
    virtual void BeginPlay() override;

public:
    // Called when the player picks the item up
    virtual void OnPickup();

    // Called when the player uses this item
    virtual bool OnUse(class AEscapeCharacter* UsingCharacter);
    bool ShouldDestroyOnUse() const { return bDestroyOnUse; }


    // Getter for the icon
    FORCEINLINE UTexture2D* GetIcon() const { return Icon; }

    FORCEINLINE EItemType GetItemType() const { return ItemType; }
};
