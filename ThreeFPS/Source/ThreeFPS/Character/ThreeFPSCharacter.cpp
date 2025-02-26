// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThreeFPSCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Game/ThreeFPSPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HUDWidget.h"
#include "InputActionValue.h"
#include "HUDWidget.h"
#include "MovieSceneTracksComponentTypes.h"
#include "Blueprint/UserWidget.h"
#include "Engine/LocalPlayer.h"
#include "ViewportToolbar/UnrealEdViewportToolbar.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AThreeFPSCharacter

AThreeFPSCharacter::AThreeFPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	//3인칭 카메라 설정
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	SpringArm->SetRelativeRotation(FRotator(0, -90.f, 0));
	SpringArm->bUsePawnControlRotation = true;
	
	ThirdPersonCameraComponent=CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(SpringArm);
	ThirdPersonCameraComponent->SetActive(true);
	ThirdPersonCameraComponent->bUsePawnControlRotation = false;
	
	//3인칭 시점 메쉬 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/ParagonTwinblast/Characters/Heroes/TwinBlast/Meshes/TwinBlast"));
	if (MeshAsset.Succeeded())
	{
 		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	GetMesh()->SetRelativeRotation(FRotator(0, -90.f, 0.f));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));

	//무기 메쉬 초기화
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	WeaponMesh->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> WeaponMeshAsset(TEXT("/Game/SHyeon/M4A1/Meshes/SK_M4A1"));
	if (WeaponMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(WeaponMeshAsset.Object);
	}
	
	// if (WeaponMesh && !GetMesh())
	// {
	// 	FAttachmentTransformRules attachmentRules(EAttachmentRule::SnapToTarget, true);
	// 	WeaponMesh-> AttachToComponent(GetMesh(), attachmentRules, FName("hr_weapon"));
	// }
	
	//앉기 활성화
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	
	//달리기 변수 초기화.
	OriginSpeed = GetCharacterMovement()->MaxWalkSpeed;
	SprintRate = 1.5f;
	SprintSpeed = OriginSpeed * SprintRate;
	bIsSprinting = false;
	
	//체력 스태미너
	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;
	MaxStamina = 100.f;
	CurrentStamina = MaxStamina;
	bIsStaminaEmpty = false;
	StaminaConsumeRate = 5.0f;
	StaminaRegenRate = 3.0f;

	//HUD
	HUDClass =nullptr;
	HUDInstance = nullptr;
}

// Input
void AThreeFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AThreeFPSPlayerController* PlayerController = Cast<AThreeFPSPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction){EnhancedInputComponent->BindAction(PlayerController->MoveAction,ETriggerEvent::Triggered,this, &AThreeFPSCharacter::Move);}
			if (PlayerController->LookAction){EnhancedInputComponent->BindAction(PlayerController->LookAction,ETriggerEvent::Triggered,this, &AThreeFPSCharacter::Look);}
			if (PlayerController->JumpAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->JumpAction,ETriggerEvent::Started,this, &ACharacter::Jump);
				EnhancedInputComponent->BindAction(PlayerController->JumpAction,ETriggerEvent::Completed,this, &ACharacter::StopJumping);
			}
			if (PlayerController->CrouchAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->CrouchAction,ETriggerEvent::Started,this, &AThreeFPSCharacter::StartCrouch);
				EnhancedInputComponent->BindAction(PlayerController->CrouchAction,ETriggerEvent::Completed,this, &AThreeFPSCharacter::StopCrouch);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->SprintAction,ETriggerEvent::Triggered,this, &AThreeFPSCharacter::StartSprint);
				EnhancedInputComponent->BindAction(PlayerController->SprintAction,ETriggerEvent::Completed,this, &AThreeFPSCharacter::StopSprint);
			}
			if (PlayerController->AimAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->AimAction,ETriggerEvent::Started, this, &AThreeFPSCharacter::StartAim);
				EnhancedInputComponent->BindAction(PlayerController->AimAction,ETriggerEvent::Completed, this, &AThreeFPSCharacter::StopAim);
			}
			
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AThreeFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HUDClass)
	{
		AThreeFPSPlayerController* PlayerController = Cast<AThreeFPSPlayerController>(GetController());
		HUDInstance = CreateWidget<UHUDWidget>(PlayerController, HUDClass);
		HUDInstance->AddToViewport();
	}
	
	//스테미너 업데이트 타이머
	GetWorldTimerManager().SetTimer(UpdateStaminaTimer, this, &AThreeFPSCharacter::UpdateStamina, 0.1f, true);
	
	if (WeaponMesh && GetMesh())
	{
		FAttachmentTransformRules attachmentRules(EAttachmentRule::SnapToTarget, true);
		WeaponMesh->AttachToComponent(GetMesh(), attachmentRules, FName("hr_weapon"));
	}
}

//------------------------//
//        업데이트 함수     //
//-----------------------//
void AThreeFPSCharacter::UpdateStamina()
{
	if (bIsSprinting)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - StaminaConsumeRate, 0.0f, MaxStamina);
		if (CurrentStamina <= 0.0f)
		{
			bIsStaminaEmpty = true;
		}
		else bIsStaminaEmpty = false;
	}
	else CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenRate, 0.0f, MaxStamina);
	/* 설빈 - 임시로 추가*/
	if (HUDInstance) {
		HUDInstance->SetStaminaBar(CurrentStamina, MaxStamina);
	}
	/***/
}

//------------------------//
//        입력 함수        //
//-----------------------//
void AThreeFPSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AThreeFPSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AThreeFPSCharacter::StartSprint()
{
	if (!bIsSprinting && !GetCharacterMovement()->IsFalling())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsSprinting = true;
	}
	else GetCharacterMovement()->MaxWalkSpeed = OriginSpeed;
	
}
void AThreeFPSCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = OriginSpeed;
	bIsSprinting = false;
}
void AThreeFPSCharacter::StartCrouch()
{
	Crouch();
}
void AThreeFPSCharacter::StopCrouch()
{
	UnCrouch();
}
void AThreeFPSCharacter::StartAim()
{
	bIsAiming = true;
	GetCharacterMovement()->bOrientRotationToMovement=false;
	GetCharacterMovement()->bUseControllerDesiredRotation=true;
}
void AThreeFPSCharacter::StopAim()
{
	bIsAiming = false;
}
