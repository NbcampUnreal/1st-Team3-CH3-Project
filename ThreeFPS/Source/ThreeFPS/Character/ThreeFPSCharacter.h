// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ThreeFPS/Item/ItemDatabase.h"
#include "ThreeFPSCharacter.generated.h"

class UThreeFPSUIComponent;
class USpringArmComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UHUDWidget;
class UInventoryWidget;
struct FInputActionValue;
struct FItemData;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AThreeFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	// 무기 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;
	
	/** camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	//3인칭 카메라
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* ThirdPersonCameraComponent;
	//스프링 암
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	//속도 변수
	UPROPERTY(EditAnywhere, Category = "Movement")
	float OriginSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed;
	UPROPERTY(EditAnywhere,Category = "Movement")
	float SprintRate;
	UPROPERTY(EditAnywhere,Category = "Movement")
	bool bIsSprinting;
	UPROPERTY(EditAnywhere,Category = "Movement")
	bool bShouldMove;

	//체력, 스태미너 변수
	UPROPERTY(EditAnywhere,Category = "Status")
	float MaxHealth;
	UPROPERTY(EditAnywhere,Category = "Status")
	float CurrentHealth;
	UPROPERTY(EditAnywhere,Category = "Status")
	float MaxStamina;
	UPROPERTY(EditAnywhere,Category = "Status")
	float CurrentStamina;
	UPROPERTY(EditAnywhere,Category = "Status")
	float StaminaConsumeRate;
	UPROPERTY(EditAnywhere,Category = "Status")
	float StaminaRegenRate;
	UPROPERTY(EditAnywhere,Category = "Status")
	bool bIsStaminaEmpty;

	//조준, 발사
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	float OriginSpringArmLength;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	float AimedSpringArmLength;
	UPROPERTY(EditAnywhere, Category = "Fire")
	bool bIsFiring;
	UPROPERTY(EditAnywhere, Category = "Fire")
	float FireRate;
	
	//HUD
	UPROPERTY(EditAnywhere,Category = "HUD")
	TSubclassOf<UHUDWidget> HUDClass;
	UPROPERTY()
	UHUDWidget* HUDInstance;
	UPROPERTY(visibleAnywhere, Category = "Movement")
	UThreeFPSUIComponent* UIComponent;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> InventoryWidgetClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> InteractWidgetClass;
	UPROPERTY()
	UUserWidget* InteractWidget;

	// 아이템 베이스
	UPROPERTY(EditDefaultsOnly)
	UItemDatabase* ItemDatabase;

	
	//타이머 핸들 변수
	FTimerHandle UpdateStaminaTimer;
	FTimerHandle FireTimer;

	// 인터렉션 관련 변수
	FVector ViewVector;
	FRotator ViewRotation;
	FVector InteractVectorEnd;
	FHitResult InteractHitResult;
	
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	//달리기 함수
	void StartSprint();
	void StopSprint();
	//앉기 함수
	void StartCrouch();
	void StopCrouch();
	//조준 함수
	void StartAim();
	void StopAim();
	//발사 함수
	void StartFiring();
	void StopFiring();
	void Fire();
	// 인터렉션 함수
	void Interact();
	void InteractCheck();
	// 인벤토리 함수
	void ToggleInventory();

public:
	AThreeFPSCharacter();
	void GameStart();
	void UpdateStamina();
	void UpdateHP();

	//Getter 함수
	FORCEINLINE bool GetIsSprinting() const { return bIsSprinting; }
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; }

	TArray<FItemData> Inventory;
	UPROPERTY()
	UInventoryWidget* InventoryWidget;

};

