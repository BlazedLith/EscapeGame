// EscapeCharacter.cpp

#include "EscapeCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Escape.h"
#include "Components/InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Components/InventoryWidgetController.h"
#include "Interface/InteractionInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Items/ItemBase.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AEscapeCharacter::AEscapeCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    // 1. Setup Camera
    FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
    FPSCamera->SetupAttachment(GetCapsuleComponent());
    FPSCamera->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
    FPSCamera->bUsePawnControlRotation = true;

    // 2. Setup Mesh
    FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSMesh"));
    FPSMesh->SetupAttachment(FPSCamera);
    FPSMesh->SetOnlyOwnerSee(true);
    FPSMesh->SetCastShadow(false);
    FPSMesh->SetCollisionProfileName(FName("NoCollision"));
    FPSMesh->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

    // Movement tuning
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
    GetCharacterMovement()->AirControl = 0.5f;
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;

    // Inventory component
    InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void AEscapeCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Get the name of the current map
    FString LevelName = GetWorld()->GetName();

    // If we are in Level 2, we enable Pacman Mode and SKIP creating the Inventory UI
    if (LevelName.Contains("Level2") || LevelName.Contains("Level3"))
    {
        bIsPacmanMode = true;
        SetInventoryVisible(false);
    }
    else
    {
        // --- LEVEL 1 SETUP (Normal Game) ---

        // 2. Create Inventory UI
        if (InventoryWidgetClass)
        {
            InventoryWidget = CreateWidget<UUserWidget>(GetWorld(), InventoryWidgetClass);
            if (InventoryWidget)
            {
                InventoryWidget->AddToViewport();

                InventoryWidgetController = NewObject<UInventoryWidgetController>(this);
                if (UInventoryWidgetController* C = Cast<UInventoryWidgetController>(InventoryWidgetController))
                {
                    C->Initialize(InventoryComp, InventoryWidget);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("InventoryWidgetClass not selected in Blueprint!"));
        }

        // 3. Load previous progress (Restore State from Save File)
        LoadGame();
    }

    // 4. Create Message Widget (Unified System)
    // We create this in ALL levels because we might need notifications in Level 2 as well
    if (MessageWidgetClass)
    {
        // Use the specific C++ class UEscapeMessageWidget
        MessageWidget = CreateWidget<UEscapeMessageWidget>(GetWorld(), MessageWidgetClass);
        if (MessageWidget)
        {
            MessageWidget->AddToViewport();
            MessageWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void AEscapeCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Continuous check for visual feedback (Green Dot)
    FHitResult Hit;
    if (GetLookTarget(Hit))
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor)
        {
            // Check if it is something we care about
            bool bIsInteractable = HitActor->Implements<UInteractionInterface>();
            bool bIsItem = HitActor->IsA<AItemBase>();

            if (bIsInteractable || bIsItem)
            {
                // Draw a GREEN dot at the hit location for 1 frame
                DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.0f, FColor::Green, false, 0.03f);
            }
        }
    }
}

void AEscapeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Enhanced input
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (JumpAction)
        {
            EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AEscapeCharacter::DoJumpStart);
            EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &AEscapeCharacter::DoJumpEnd);
        }

        if (MoveAction)
            EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEscapeCharacter::MoveInput);

        if (LookAction)
            EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEscapeCharacter::LookInput);

        if (MouseLookAction)
            EnhancedInput->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AEscapeCharacter::LookInput);

        if (InteractAction)
        {
            EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &AEscapeCharacter::TryPickupItem);
        }

        if (QuitAction)
        {
            EnhancedInput->BindAction(QuitAction, ETriggerEvent::Triggered, this, &AEscapeCharacter::QuitGame);
        }
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Warning, TEXT("EnhancedInputComponent not found."));
    }

    // Legacy Input for Inventory Slots (Hotkeys 1-6)
    if (PlayerInputComponent)
    {
        // Use Item (F Key)
        PlayerInputComponent->BindAction("UseItem", IE_Pressed, this, &AEscapeCharacter::UseSelectedItem);

        // Slot Selection
        PlayerInputComponent->BindAction("Slot1", IE_Pressed, this, &AEscapeCharacter::SelectSlot1);
        PlayerInputComponent->BindAction("Slot2", IE_Pressed, this, &AEscapeCharacter::SelectSlot2);
        PlayerInputComponent->BindAction("Slot3", IE_Pressed, this, &AEscapeCharacter::SelectSlot3);
        PlayerInputComponent->BindAction("Slot4", IE_Pressed, this, &AEscapeCharacter::SelectSlot4);
        PlayerInputComponent->BindAction("Slot5", IE_Pressed, this, &AEscapeCharacter::SelectSlot5);
        PlayerInputComponent->BindAction("Slot6", IE_Pressed, this, &AEscapeCharacter::SelectSlot6);
    }
}

// ---------------------- MOVEMENT & LOOK ----------------------

void AEscapeCharacter::MoveInput(const FInputActionValue& Value)
{
    FVector2D Axis = Value.Get<FVector2D>();
    DoMove(Axis.X, Axis.Y);
}

void AEscapeCharacter::LookInput(const FInputActionValue& Value)
{
    FVector2D Axis = Value.Get<FVector2D>();
    DoAim(Axis.X, Axis.Y);
}

void AEscapeCharacter::DoMove(float Right, float Forward)
{
    if (Controller)
    {
        AddMovementInput(GetActorRightVector(), Right);
        AddMovementInput(GetActorForwardVector(), Forward);
    }
}

void AEscapeCharacter::DoAim(float Yaw, float Pitch)
{
    if (Controller)
    {
        AddControllerYawInput(Yaw);
        AddControllerPitchInput(Pitch);
    }
}

void AEscapeCharacter::DoJumpStart() { Jump(); }
void AEscapeCharacter::DoJumpEnd() { StopJumping(); }

// ---------------------- INVENTORY & INTERACTION ----------------------

void AEscapeCharacter::SelectInventorySlot(int32 Slot)
{
    if (InventoryComp)
        InventoryComp->SelectSlot(Slot);
}

void AEscapeCharacter::SelectSlot1() { SelectInventorySlot(0); }
void AEscapeCharacter::SelectSlot2() { SelectInventorySlot(1); }
void AEscapeCharacter::SelectSlot3() { SelectInventorySlot(2); }
void AEscapeCharacter::SelectSlot4() { SelectInventorySlot(3); }
void AEscapeCharacter::SelectSlot5() { SelectInventorySlot(4); }
void AEscapeCharacter::SelectSlot6() { SelectInventorySlot(5); }

void AEscapeCharacter::UseSelectedItem()
{
    if (!InventoryComp) return;

    AItemBase* Item = InventoryComp->GetSelectedItem();
    if (!Item)
    {
        // Call the unified notification system
        ShowNotification(FText::FromString("Slot is empty."));
        return;
    }

    InventoryComp->UseSelectedItem(this);
}

void AEscapeCharacter::TryPickupItem()
{
    FHitResult Hit;
    // Use the helper so logic is consistent with the UI/Tick
    if (GetLookTarget(Hit))
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor) return;

        // --- PRIORITY 1: INTERACTABLES (Doors, Drawers, Levers) ---
        if (IInteractionInterface* Interface = Cast<IInteractionInterface>(HitActor))
        {
            Interface->OnInteract(this);
        }
        // --- PRIORITY 2: ITEMS (Pickup) ---
        else if (AItemBase* Item = Cast<AItemBase>(HitActor))
        {
            if (InventoryComp && InventoryComp->AddItem(Item))
            {
                Item->OnPickup();
            }
        }
    }
}

bool AEscapeCharacter::GetLookTarget(FHitResult& OutHit)
{
    if (!FPSCamera) return false;

    FVector Start = FPSCamera->GetComponentLocation();
    FVector End = Start + (FPSCamera->GetForwardVector() * InteractionDistance);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // SHAPE: Create a sphere with radius 20cm (Thickness)
    FCollisionShape Shape;
    Shape.SetSphere(20.0f);

    // TRACE: Use "Sweep" instead of "LineTrace"
    return GetWorld()->SweepSingleByChannel(
        OutHit,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        Shape,
        Params
    );
}

// ---------------------- UI FEEDBACK ----------------------

void AEscapeCharacter::ShowInspectionUI(FText Content)
{
    // 1. Create widget if it doesn't exist
    if (!InspectionWidget && InspectionWidgetClass)
    {
        InspectionWidget = CreateWidget<UEscapeInspectionWidget>(GetWorld(), InspectionWidgetClass);
        if (InspectionWidget)
        {
            InspectionWidget->AddToViewport(10); // High Z-order
        }
    }

    // 2. Update and Show
    if (InspectionWidget)
    {
        // If already visible, toggle it OFF
        if (InspectionWidget->IsVisible())
        {
            InspectionWidget->SetVisibility(ESlateVisibility::Hidden);

            // Re-enable Game Input
            if (APlayerController* PC = Cast<APlayerController>(GetController()))
            {
                PC->SetInputMode(FInputModeGameOnly());
                PC->bShowMouseCursor = false;
            }
        }
        else
        {
            // Show it
            InspectionWidget->SetNoteContent(Content);
            InspectionWidget->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void AEscapeCharacter::ShowNotification(FText Message)
{
    if (MessageWidget)
    {
        // 1. Show the message
        MessageWidget->ShowMessage(Message);

        // 2. Set a Safe Time
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AEscapeCharacter::HideNotification, 3.0f, false);
    }
}

void AEscapeCharacter::HideNotification()
{
    // This function only runs if the Character is still alive!
    if (MessageWidget)
    {
        MessageWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AEscapeCharacter::SetInventoryVisible(bool bVisible)
{
    // In Level 2, InventoryWidget is NULL. We MUST check this before using it.
    if (!InventoryWidget)
    {
        return;
    }

    InventoryWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
void AEscapeCharacter::SaveGame()
{
    UEscapeSaveGame* SaveInst = Cast<UEscapeSaveGame>(UGameplayStatics::CreateSaveGameObject(UEscapeSaveGame::StaticClass()));
    if (SaveInst)
    {
        // Record Basic Data
        SaveInst->PlayerLocation = GetActorLocation();
        SaveInst->PlayerRotation = GetActorRotation();
        SaveInst->CurrentLevelName = FName(*GetWorld()->GetName());

        // Record Inventory IDs
        if (InventoryComp)
        {
            for (AItemBase* Item : InventoryComp->GetItems())
            {
                if (Item) SaveInst->InventoryItems.Add(Item->ItemID);
            }
        }

        UGameplayStatics::SaveGameToSlot(SaveInst, SaveInst->SaveSlotName, SaveInst->UserIndex);
        UE_LOG(LogTemp, Log, TEXT("Game Saved!"));
    }
}

void AEscapeCharacter::LoadGame()
{
    if (UGameplayStatics::DoesSaveGameExist("EscapeSaveSlot", 0))
    {
        UEscapeSaveGame* LoadedGame = Cast<UEscapeSaveGame>(UGameplayStatics::LoadGameFromSlot("EscapeSaveSlot", 0));
        if (LoadedGame && InventoryComp && !bIsPacmanMode)
        {
            // In a real full game, we would respawn items here. 
            // For now, we just acknowledge the data exists.
            UE_LOG(LogTemp, Log, TEXT("Game Loaded. Found %d items."), LoadedGame->InventoryItems.Num());
        }
    }
}

void AEscapeCharacter::QuitGame()
{
    // Get the controller and request exit
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, true);
    }
}

void AEscapeCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // 1. Remove Inventory UI
    if (InventoryWidget)
    {
        InventoryWidget->RemoveFromParent();
        InventoryWidget = nullptr;
    }

    // 2. Remove Message UI
    if (MessageWidget)
    {
        MessageWidget->RemoveFromParent();
        MessageWidget = nullptr;
    }

    // 3. Remove Inspection UI
    if (InspectionWidget)
    {
        InspectionWidget->RemoveFromParent();
        InspectionWidget = nullptr;
    }

    // 4. Ensure Input is reset to Game Mode (fixes mouse stuck issues)
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }
}