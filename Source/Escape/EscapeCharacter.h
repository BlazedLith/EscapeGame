// EscapeCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Blueprint/UserWidget.h"
#include "InputAction.h"
#include "EscapeInspectionWidget.h" 
#include "EscapeMessageWidget.h"    
#include "Kismet/GameplayStatics.h" 
#include "EscapeSaveGame.h"
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

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // --- UI Functions ---
    // Show the Note Reader UI
    void ShowInspectionUI(FText Content);

    // Show a notification on screen (Locked door, Empty slot, etc.)
    void ShowNotification(FText Message);

    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveGame();

    UFUNCTION(BlueprintCallable, Category = "Save")
    void LoadGame();

    void SetInventoryVisible(bool bVisible);

    // --- Getters ---
    // Returns the inventory component so other classes (like Door) can check for keys
    FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComp; }

    USkeletalMeshComponent* GetFirstPersonMesh() const { return FPSMesh; }
    UCameraComponent* GetFirstPersonCameraComponent() const { return FPSCamera; }

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HideNotification();

    // --- UI COMPONENTS ---

    // Notification Widget (Locked, Needs Key, etc.)
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UEscapeMessageWidget> MessageWidgetClass;

    UPROPERTY()
    UEscapeMessageWidget* MessageWidget;

    // Note Reader Widget
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UEscapeInspectionWidget> InspectionWidgetClass;

    UPROPERTY()
    UEscapeInspectionWidget* InspectionWidget;

    // Inventory Widget
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> InventoryWidgetClass;

    UPROPERTY()
    UUserWidget* InventoryWidget;

    UPROPERTY()
    UInventoryWidgetController* InventoryWidgetController;

    // --- INTERACTION SETTINGS ---

    // Trace distance (3 meters)
    float InteractionDistance = 300.0f;

    // Helper to do the trace
    bool GetLookTarget(FHitResult& OutHit);

    // --- VISUAL COMPONENTS ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* FPSMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FPSCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    UInventoryComponent* InventoryComp;

    // --- INPUT ACTIONS ---

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* InteractAction; // Key: E

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MouseLookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* QuitAction;

    // --- INTERNAL FUNCTIONS ---
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

    void MoveInput(const FInputActionValue& Value);
    void LookInput(const FInputActionValue& Value);

    void SelectInventorySlot(int32 Slot);

    // Input helper functions
    void SelectSlot1(); void SelectSlot2(); void SelectSlot3();
    void SelectSlot4(); void SelectSlot5(); void SelectSlot6();

    void UseSelectedItem(); // Key: F
    void TryPickupItem();   // Key: E
    void QuitGame();

    bool bIsPacmanMode = false;

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoAim(float Yaw, float Pitch);

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoMove(float Right, float Forward);

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoJumpStart();

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoJumpEnd();
};