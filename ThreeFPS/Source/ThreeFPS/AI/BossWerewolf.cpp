// Fill out your copyright notice in the Description page of Project Settings.


#include "BossWerewolf.h"
#include "Components/CapsuleComponent.h"
#include "AI/BossWerewolfAIController.h"
// Sets default values
ABossWerewolf::ABossWerewolf()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempSK(TEXT("/Game/SB/Model/Werebear/Mesh/SK_Werebear_red.SK_Werebear_red"));
	if (tempSK.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempSK.Object);
	}
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	AIControllerClass = ABossWerewolfAIController::StaticClass();
	// ��ġ�� NPC�� SpawnActor�� ���ؼ� ������Ų NPC �Ѵ� AI Controller�� ���� �����ϵ��� ����
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; 
}

// Called when the game starts or when spawned
void ABossWerewolf::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABossWerewolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABossWerewolf::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

