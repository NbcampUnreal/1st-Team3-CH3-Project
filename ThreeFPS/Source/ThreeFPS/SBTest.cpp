// Fill out your copyright notice in the Description page of Project Settings.


#include "SBTest.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/TimelineComponent.h"
#include "Components/CapsuleComponent.h"
#include "BossWerewolf.h"

// Sets default values
ASBTest::ASBTest()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    FootComp= CreateDefaultSubobject<USceneComponent>(TEXT("FootComp"));
    FootComp->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void ASBTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASBTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsJumping)
	{
		CurrentJumpTime += DeltaTime;

		// ������ � ���� ����
		FVector NewLocation = StartLocation + (HorizontalVelocity * CurrentJumpTime);
		NewLocation.Z = StartLocation.Z + (InitialVerticalVelocity * CurrentJumpTime) - (0.5f * 980.0f * CurrentJumpTime * CurrentJumpTime);

		SetActorLocation(NewLocation);

		// ��ǥ ���� ���� ���� Ȯ�� (�Ÿ� ��� ����)
		float CurrentDistance = FVector::Dist(NewLocation, EndLocation);
		if (CurrentDistance <= 300.f)  // ��ǥ ������ �ſ� ��������� ����
		{
			bIsJumping = false;
			GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Purple, FString::Printf(TEXT("%s >>Attack!!"), *FDateTime::UtcNow().ToString(TEXT("%H:%M:%S"))), true, FVector2D(1.5f, 1.5f));
		}
	}
   
}
#pragma optimize("" , off)
// ���� ���� �Լ�
void ASBTest::JumpAttack(ABossWerewolf* Boss)
{
    //if (!Boss || !JumpMontage) return;

     // ���� ��ġ�� ��ǥ ��ġ ����
    StartLocation = FootComp->GetComponentLocation();
    DrawDebugSphere(GetWorld(), StartLocation, 40.0f, 32, FColor::Red, false, 0.1f);
    EndLocation = Boss->FootComp->GetComponentLocation();
    DrawDebugSphere(GetWorld(), EndLocation, 40.0f, 32, FColor::Blue, false, 0.1f);
    
    // Ÿ�ٰ��� 3D �Ÿ� ���
    float DistanceXY = FVector::Dist2D(StartLocation, EndLocation);
    float DeltaHeight = EndLocation.Z - StartLocation.Z;

    //  ������ ���� ���� �ð� ��� (���� ��Ģ ���)
    JumpDuration = FMath::Sqrt((2 * FMath::Abs(DeltaHeight) + 2 * DistanceXY) / Gravity);
    JumpDuration = FMath::Clamp(JumpDuration, 0.5f, 2.5f);  // �ּ�~�ִ� ����

    //  �ʱ� ���� �ӵ� (���� ���� �ݿ�)
    InitialVerticalVelocity = (Gravity * JumpDuration) / 2 + (DeltaHeight / JumpDuration);

    //  ���� �ӵ� ���
    Speed = DistanceXY / JumpDuration;
    HorizontalVelocity = (EndLocation - StartLocation).GetSafeNormal() * Speed;
    HorizontalVelocity.Z = 0.0f;

    // ���� ���� Ȱ��ȭ
    bIsJumping = true;
    CurrentJumpTime = 0.0f;

    // ���� �����
    CalculateJumpPath(20);
}
#pragma optimize("" , on) 
#pragma optimize("" , off)
void ASBTest::CalculateJumpPath(int32 NumPoints)
{
    JumpPathPoints.Empty();  // �迭 �ʱ�ȭ

    float TotalDistance = FVector::Dist(StartLocation, EndLocation);
    float StepDistance = TotalDistance / (NumPoints - 1);

    FVector Direction = (EndLocation - StartLocation).GetSafeNormal();

    for (int i = 0; i < NumPoints; i++)
    {
        FVector PointLocation = StartLocation + Direction * (StepDistance * i);
        float DistanceTraveled = FVector::Dist(StartLocation, PointLocation);

        //  �Ÿ� ��� Z ��ġ ��� (������ ���� ����)
        float TimeAtPoint = DistanceTraveled / HorizontalVelocity.Size();
        PointLocation.Z = StartLocation.Z + (InitialVerticalVelocity * TimeAtPoint) - (0.5f * Gravity * TimeAtPoint * TimeAtPoint);

        JumpPathPoints.Add(PointLocation);

        // ����� ��ü
        DrawDebugSphere(GetWorld(), PointLocation, 10.0f, 12, FColor::Red, false, 5.0f);
    }
}
#pragma optimize("" , on) 