// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterInterface.h"
#include "BaseMonster.generated.h"

class USphereComponent;

UCLASS()
class THREEFPS_API ABaseMonster : public ACharacter, public IMonsterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseMonster();
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> PatrolPoints;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	FName MonsterType;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	//USceneComponent* Scene;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	//UCapsuleComponent* Collision;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	//USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Effects")
	UParticleSystem* ParticleBlood;

	virtual void OnCapsuleOverlap(
		UPrimitiveComponent* OverlappedComp,/*자신*/
		AActor* OtherActor,/*충돌한 액터*/
		UPrimitiveComponent* OtherComp,/*충돌한 액터에 달린 충돌 컴포넌트(Collision)*/
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;



	virtual void OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComp,/*자신*/
		AActor* OtherActor,/*충돌한 액터*/
		UPrimitiveComponent* OtherComp,/*충돌한 액터에 달린 충돌 컴포넌트(Collision)*/
		int32 OtherBodyIndex) override;

	virtual void ApplyRagdoll(FVector HitDirection) override;
	// virtual void BeginPlay() override;
public:
	//virtual void Tick(float DeltaTime) override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
