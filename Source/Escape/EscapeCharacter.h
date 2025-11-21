// EscapeCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Blueprint/UserWidget.h"
#include "InputAction.h"
#include "EscapeCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInventoryComponent;
class UInventoryWidgetController;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class AEscapeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEscapeCharacter();

    // Update the getters to match the new names
    USkeletalMeshComponent* GetFirstPersonMesh() const { return FPSMesh; }
    UCameraComponent* GetFirstPersonCameraComponent() const { return FPSCamera; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* FPSMesh; // Renamed from FirstPersonMesh

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FPSCamera; // Renamed from FirstPersonCameraComponent

    // Enhanced Input actions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MouseLookAction;


    // ---------------------- COMPONENTS ----------------------

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    UInventoryComponent* InventoryComp;

    // ---------------------- WIDGETS SELECTED IN BLUEPRINT ----------------------
    // Blueprint chooses these widget classes in the editor
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> InventoryWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> MessageWidgetClass;

    // Runtime-created widgets (C++)
    UPROPERTY()
    UUserWidget* InventoryWidget;

    UPROPERTY()
    UUserWidget* MessageWidget;

    UPROPERTY()
    UInventoryWidgetController* InventoryWidgetController;

    // ---------------------- INTERNAL ----------------------
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

    void MoveInput(const FInputActionValue& Value);
    void LookInput(const FInputActionValue& Value);

    void SelectInventorySlot(int32 Slot);

    // Input helper functions
    void SelectSlot1(); void SelectSlot2(); void SelectSlot3();
    void SelectSlot4(); void SelectSlot5(); void SelectSlot6();

    void UseSelectedItem();
    void TryPickupItem();

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoAim(float Yaw, float Pitch);

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoMove(float Right, float Forward);

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoJumpStart();

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoJumpEnd();

    // Internal fallback for message timeout
    void ShowEmptySlotMessage();
};
