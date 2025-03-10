// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Components/TimelineComponent.h"
#include "Weapon/EGunType.h"
#include "ThreeFPS/Item/ItemDatabase.h"
#include "Weapon/EPlayerMovementState.h"
#include "ThreeFPSCharacter.generated.h"

class UOnDiedWidget;
class UWeaponInventoryComponent;
enum class EGunType : uint8;
class UThreeFPSUIComponent;
class USpringArmComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UHUDWidget;
class UInventoryWidget;
class AGunBase;
struct FInputActionValue;
struct FItemData;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AThreeFPSCharacter : public ACharacter
{
	GENERATED_BODY()
	EPlayerMovementState CurrentMovementState;
	
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
	UPROPERTY(EditAnywhere,Category = "Movement", meta= (AllowPrivateAccess = "true"))
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

	//돌연변이 변수
	UPROPERTY(EditAnywhere,Category = "Status")
	float MaxMutation;
	UPROPERTY(EditAnywhere,Category = "Status")
	float CurrentMutation;
	UPROPERTY(EditAnywhere,Category = "Status")
	float MutationRate;
	FTimerHandle UpdateMutationTimer;

	//조준, 발사
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	float OriginSpringArmLength;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	float AimedSpringArmLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	bool bIsFiring;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	float CurrentRecoil;
	
	//재장전
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	bool bIsReloading;
	
	//HUD
	UPROPERTY(EditAnywhere,Category = "HUD")
	TSubclassOf<UHUDWidget> HUDClass;
	UPROPERTY()
	UHUDWidget* HUDInstance;
	UPROPERTY(EditAnywhere,Category = "HUD")
	TSubclassOf<UOnDiedWidget> GameOverHUDClass;
	UPROPERTY()
	UOnDiedWidget* GameOverHUDInstance;
	
	//크로스헤어용 UI컴포넌트.
	UPROPERTY(visibleAnywhere, Category = "Movement")
	UThreeFPSUIComponent* UIComponent;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> InventoryWidgetClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> InteractWidgetClass;
	UPROPERTY()
	UUserWidget* InteractWidget;
	UUserWidget* MissionWidget;

	// 아이템 베이스
	UPROPERTY(EditDefaultsOnly)
	UItemDatabase* ItemDatabase;
	
	//타이머 핸들 변수
	FTimerHandle UpdateStaminaTimer;
	FTimerHandle FireTimer;
	FTimerHandle DiedTimer;
	FTimerHandle JumpTimer;
	FTimerHandle DiveTimer;

	// 인터렉션 관련 변수
	FVector ViewVector;
	FRotator ViewRotation;
	FVector InteractVectorEnd;
	FHitResult InteractHitResult;
	FTimerHandle ReloadTimer;
	
protected:

	//인벤토리 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(AllowPrivateAccess=true))
	UWeaponInventoryComponent* WeaponInventory;
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AGunBase> RifleClass;

	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	//사망
	void Die();
	void GameOver();
	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;

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

	//점프 관련 함수
	virtual void Jump() override;
	void ResetJumpTimer();
	// void UpdateAimProgress(float Value);
	
	//발사 함수
	void StartFiring();
	void StopFiring();
	
	// 인터렉션 함수
	void Interact();
	void InteractCheck();
	
	// 인벤토리 함수
	void ToggleInventory();
	
	//재장전
	void StartReload();
	void OnReloaded();
	
	//회피
	void StartDive();
	void ResetDiveTimer();
	
	void EquipRifle();
	
public:
	FTimeline AimTimeLine;
	
	AThreeFPSCharacter();
	
	void GameStart();
	//업데이트 함수
	void UpdateStamina();
	void UpdateHP();
	void UpdateMutation();
	void UpdateAmmo();

	UFUNCTION(BlueprintCallable)
	void StopMutation();

	// 아이템 사용 함수
	void IncreaseHealth(int32 Amount);
	void DecreaseMutation(int32 Amount);
	void IncreaseAmmo(int32 Amount);

	//Getter 함수
	FORCEINLINE EPlayerMovementState GetCurrentMovementState() const {return CurrentMovementState;}
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE bool GetIsFiring() const { return bIsFiring; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsDive() const {return bIsDive;}
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsSprinting() const { return bIsSprinting; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; }
	FORCEINLINE UHUDWidget* GetHUDWidget() const {return HUDInstance;}
	
	TArray<FItemData> Inventory;
	UPROPERTY()
	UInventoryWidget* InventoryWidget;
	
	//몽타주
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Animation")
	UAnimMontage* HitMontage;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Animation")
	UAnimMontage* HipShotMontage;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Animation")
	UAnimMontage* AimShotMontage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Animation")
	TMap<EGunType, TSubclassOf<UAnimInstance>> Animations;

	bool bIsJumping;
	bool bCanJump;
	bool bIsDive;
	bool bIsCrouched;
};


