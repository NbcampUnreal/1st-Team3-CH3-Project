// Fill out your copyright notice in the Description page of Project Settings.


//#include "Monster/ZombieAnimInstance.h"
#include "Zombie.h"
#include "ZombieAnimInstance.h"

UZombieAnimInstance::UZombieAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAtt1(TEXT("AnimMontage'/Game/KSW/Animation/Zombie_Atk1_Montage.Zombie_Atk1_Montage'"));
	if (MontageAtt1.Succeeded())
		AttMontageArr.Add(MontageAtt1.Object);
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAtt2(TEXT("AnimMontage'/Game/KSW/Animation/Zombie_Atk2_Montage.Zombie_Atk2_Montage'"));
	if (MontageAtt2.Succeeded())
		AttMontageArr.Add(MontageAtt2.Object);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageDamage(TEXT("AnimMontage'/Game/KSW/Animation/Zombie_Damage_Montage.Zombie_Damage_Montage'"));
	if (MontageDamage.Succeeded())
	{
		DamageMontage = MontageDamage.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageDie(TEXT("AnimMontage'/Game/KSW/Animation/ZombieDieAnimMontage.ZombieDieAnimMontage'"));
	if (MontageDie.Succeeded())
		DieMontage = MontageDie.Object;
}

float UZombieAnimInstance::AnimationPlay(EZONBIE_ST Ani)
{
	float AniLength = 0.f;
	if (EZONBIE_ST::ATTACK == Ani)
	{
		int32 Rand = FMath::RandRange(0, 1);
		AniLength = AttMontageArr[Rand]->GetPlayLength();
		Montage_Play(AttMontageArr[Rand]);
	}
	else if (EZONBIE_ST::DAMAGE == Ani)
	{
		AniLength = DamageMontage->GetPlayLength();
		Montage_Play(DamageMontage);
	}
	else if (EZONBIE_ST::DIE == Ani)
	{
		int32 RandIdx = FMath::RandRange(1, 4);
		FString SectionName = "Die" + FString::FromInt(RandIdx);

		int32 SectionIdx = DieMontage->GetSectionIndex(FName(SectionName));
		AniLength = DieMontage->GetSectionLength(SectionIdx);
		
		Montage_Play(DieMontage);

		Montage_JumpToSection(FName(SectionName), DieMontage);
		UE_LOG(LogTemp, Warning, TEXT("AniSection:%s/Time:%f"), *SectionName, AniLength);
	}
	return AniLength;
}

void UZombieAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}





