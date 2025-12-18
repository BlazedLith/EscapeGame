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
#include "DataCube.h" // <--- MOVED UP! Must be above .generated.h
#include "EscapeCharacter.generated.h" // <--- THIS MUST BE LAST

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
    void ShowInspectionUI(FText Content);
    void ShowNotification(FText Message);

    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveGame();

    UFUNCTION(BlueprintCallable, Category = "Save")
    void LoadGame();

    void SetInventoryVisible(bool bVisible);

    // --- Getters ---
    FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComp; }
    USkeletalMeshComponent* GetFirstPersonMesh() const { return FPSMesh; }
    UCameraComponent* GetFirstPersonCameraComponent() const { return FPSCamera; }

    // Handles saving the specific "Next Level" name and then opening it
    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void CompleteLevel(FName NextLevelName);

    // Deletes the save file (Used when the game is fully beaten)
    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void ClearSaveGameData();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION()
    void HideNotification();

    // --- UI COMPONENTS ---
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UEscapeMessageWidget> MessageWidgetClass;

    UPROPERTY()
    UEscapeMessageWidget* MessageWidget;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UEscapeInspectionWidget> InspectionWidgetClass;

    UPROPERTY()
    UEscapeInspectionWidget* InspectionWidget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> InventoryWidgetClass;

    UPROPERTY()
    UUserWidget* InventoryWidget;

    UPROPERTY()
    UInventoryWidgetController* InventoryWidgetController;

    // --- INTERACTION SETTINGS ---
    float InteractionDistance = 300.0f;
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
    UInputAction* InteractAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MouseLookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* QuitAction;

    // --- INTERNAL FUNCTIONS ---
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    void MoveInput(const FInputActionValue& Value);
    void LookInput(const FInputActionValue& Value);
    void SelectInventorySlot(int32 Slot);
    void SelectSlot1(); void SelectSlot2(); void SelectSlot3();
    void SelectSlot4(); void SelectSlot5(); void SelectSlot6();
    void UseSelectedItem();
    void TryPickupItem();
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

    // 1. The Stack (Undo Mechanic)
    UPROPERTY()
    TArray<FVector> LocationStack;

    // The "Kill Height" for auto-rewind
    float KillZThreshold = -2000.0f;

    // 2. The Physics Grabbing Variable
    UPROPERTY()
    ADataCube* HeldCube = nullptr;

public:
    // Stack Functions
    UFUNCTION(BlueprintCallable, Category = "Level3")
    void PushSafeLocation();

    UFUNCTION(BlueprintCallable, Category = "Level3")
    void PopAndRewind();
};