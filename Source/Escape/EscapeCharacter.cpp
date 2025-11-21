// EscapeCharacter.cpp

#include "EscapeCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Escape.h"
#include "Components/InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Components/InventoryWidgetController.h"
#include "Items/ItemBase.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AEscapeCharacter::AEscapeCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    // 1. Setup Camera (Renamed to FPSCamera)
    FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
    FPSCamera->SetupAttachment(GetCapsuleComponent());
    FPSCamera->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
    FPSCamera->bUsePawnControlRotation = true;

    // 2. Setup Mesh (Renamed to FPSMesh)
    FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSMesh"));
    // IMPORTANT: Attach Mesh to Camera so arms move with view
    FPSMesh->SetupAttachment(FPSCamera);
    FPSMesh->SetOnlyOwnerSee(true);
    FPSMesh->SetCastShadow(false);
    FPSMesh->SetCollisionProfileName(FName("NoCollision"));
    FPSMesh->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

    // Movement tuning
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
    GetCharacterMovement()->AirControl = 0.5f;

    // Inventory component
    InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void AEscapeCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Inventory UI
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

    // Empty-slot message UI
    if (MessageWidgetClass)
    {
        MessageWidget = CreateWidget<UUserWidget>(GetWorld(), MessageWidgetClass);
        if (MessageWidget)
        {
            MessageWidget->AddToViewport();
            MessageWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}


void AEscapeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Enhanced input (movement / look / jump)
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
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Warning, TEXT("EnhancedInputComponent not found - enhanced input won't be used."));
    }

    // Classic input bindings (inventory) – uses old Input system names
    if (PlayerInputComponent)
    {
        PlayerInputComponent->BindAction("UseItem", IE_Pressed, this, &AEscapeCharacter::UseSelectedItem);
        PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AEscapeCharacter::TryPickupItem);

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

// ---------------------- INVENTORY ----------------------

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
    if (!InventoryComp)
        return;

    AItemBase* Item = InventoryComp->GetSelectedItem();
    if (!Item)
    {
        ShowEmptySlotMessage();
        return;
    }

    InventoryComp->UseSelectedItem(this);
}

void AEscapeCharacter::TryPickupItem()
{
    if (!FPSCamera || !InventoryComp) return;

    FVector Start = FPSCamera->GetComponentLocation();
    FVector End = Start + (FPSCamera->GetForwardVector() * 300.0f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        AItemBase* Item = Cast<AItemBase>(Hit.GetActor());
        if (Item)
        {
            if (InventoryComp->AddItem(Item))
            {
                Item->OnPickup();
            }
        }
    }
}

// ---------------------- EMPTY SLOT MESSAGE ----------------------

void AEscapeCharacter::ShowEmptySlotMessage()
{
    if (!MessageWidget)
        return;

    // Show the message
    MessageWidget->SetVisibility(ESlateVisibility::Visible);

    // Hide it after 5 seconds
    FTimerDelegate Delegate = FTimerDelegate::CreateLambda([this]()
        {
            if (MessageWidget)
            {
                MessageWidget->SetVisibility(ESlateVisibility::Hidden);
            }
        });

    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, Delegate, 3.0f, false);
}

