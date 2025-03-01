// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie.h"
#include "MonsterAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstance.h"
#include "MonsterAIController.h"
#include "DrawDebugHelpers.h"
#include "ZombieAnimInstance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
AZombie::AZombie()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCharacterMovement()->bUseRVOAvoidance = true; //true가 적용된 몬스터들은  AvoidanceConsiderationRadius 가중치에 따라 들러붙지않으려고 한다.
    GetCharacterMovement()->AvoidanceConsiderationRadius = 100.f;
    GetCapsuleComponent()->InitCapsuleSize(86.42f, 24.64f);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);

    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));//DetectionSphere = NewObject<USphereComponent>(this);
    if (DetectionSphere)
    {
        DetectionSphere->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
        DetectionSphere->SetSphereRadius(60.f);
        DetectionSphere->SetRelativeLocation(FVector(40.f, 0.f, 20.f));
        DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        DetectionSphere->SetCollisionObjectType(ECC_WorldDynamic);
        DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
        DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
        //DetectionSphere->RegisterComponent();
        DetectionSphere->SetCollisionProfileName(TEXT("Trigger"));
    }

    bool bSelectFeMale = 0;
    FString BodyMeshFilePath = "";
    if (bSelectFeMale)
        BodyMeshFilePath = TEXT("SkeletalMesh'/Game/KSW/Resouces/zombie/mesh/Female/SK_Zombie_F02_01.SK_Zombie_F02_01'");
    else
        BodyMeshFilePath = TEXT("SkeletalMesh'/Game/KSW/Resouces/zombie/mesh/male/AA_SK_Zombie_M02_01.AA_SK_Zombie_M02_01'");

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> BodyMeshAsset(*BodyMeshFilePath);
    if (BodyMeshAsset.Succeeded())
        GetMesh()->SetSkeletalMesh(BodyMeshAsset.Object);

    GetMesh()->MarkRenderStateDirty();
    GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90.f), FRotator(0.f, -90.f, 0.f));
    GetMesh()->SetRelativeScale3D(FVector(1.f));
    
    PantsMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PantsMesh"));
    PantsMeshComp->SetupAttachment(GetMesh()); 
    PantsMeshComp->SetLeaderPoseComponent(GetMesh()); // 본을 공유

    
    ShirtMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShirtMesh"));
    ShirtMeshComp->SetupAttachment(GetMesh());
    ShirtMeshComp->SetLeaderPoseComponent(GetMesh());

    HairMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HairMesh"));
    HairMeshComp->SetupAttachment(GetMesh());
    HairMeshComp->SetLeaderPoseComponent(GetMesh());

    HairStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HairStaticMesh"));
    FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
    // HairStaticMeshComp->AttachToComponent(GetMesh(), Rules, FName("headSocket"));
    HairStaticMeshComp->SetupAttachment(GetMesh(), FName("headSocket"));
    //HairStaticMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("head"));
    FQuat NewRotation = FQuat(FRotator(90.f, -90.f, -90.f));
    HairStaticMeshComp->SetRelativeRotation(NewRotation);
    HairStaticMeshComp->SetRelativeLocation(FVector(-164.7f, 0.f, 0.f));
    HairStaticMeshComp->SetRelativeScale3D(FVector(1.f, 1.f, -1.f));

    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AZombie::OnCapsuleOverlap);

    DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AZombie::OnDetectionOverlap);
    DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AZombie::OnDetectionEndOverlap);
    DisableDetection();
    Hp = 10;
}

void AZombie::SetClothingMeshs(USkeletalMesh* Pants, USkeletalMesh* Shirt, USkeletalMesh* Hair , UStaticMesh* HairStatic )
{
    if(Pants)
        PantsMeshComp->SetSkeletalMesh(Pants);
    if(Shirt)
        ShirtMeshComp->SetSkeletalMesh(Shirt);
    if(Hair)
        HairMeshComp->SetSkeletalMesh(Hair);
    if (HairStatic)
        HairStaticMeshComp->SetStaticMesh(HairStatic);
}

void AZombie::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnCapsuleOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    if (OtherActor && OtherActor->ActorHasTag("Projectile"))
    {
        FVector AttackDirection = (GetActorLocation() - OtherActor->GetActorLocation()).GetSafeNormal();
        ApplyRagdoll(AttackDirection);
        //UE_LOG(LogTemp, Warning, TEXT("몬스터가 %s와 오버랩 발생!"), *OtherActor->GetName());
    }
}
void AZombie::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnCapsuleEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}
void AZombie::OnDetectionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnDetectionEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
    if (OtherActor == TargetActor)
    {
        bDetectionOverlapping = false;
        bAttackTimming = false;
        TargetActor = nullptr;
       // UE_LOG(LogTemp, Warning, TEXT("몬스터가 %s와 End오버랩 !"), *OtherActor->GetName());
    }
}
void AZombie::OnDetectionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnDetectionOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
   
    if (OtherActor && OtherActor->ActorHasTag("Player") )
    {
        bDetectionOverlapping = true;
        TargetActor = Cast<ACharacter>(OtherActor);
       // UE_LOG(LogTemp, Warning, TEXT("몬스터가 %s와 오버랩 발생!"), *OtherActor->GetName());
       // FVector CurPos = GetActorLocation() + DetectionSphere->GetRelativeLocation();
       // float Distance = FVector::DistXY(CurPos, TargetActor->GetActorLocation());
       // UE_LOG(LogTemp, Warning, TEXT("Distance:%f/ SphereRadius:"), Distance, DetectionSphere->GetScaledSphereRadius());
    }
}
void AZombie::EnableDetection()
{
    Super::EnableDetection();
   // UE_LOG(LogTemp, Warning, TEXT("EnableDetection()!!!"));

    if (DetectionSphere && bDebugDetectionSphere)
    {
        DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        DrawDebugSphere(GetWorld(), DetectionSphere->GetComponentLocation(),
        DetectionSphere->GetScaledSphereRadius(), 12, FColor::Red, false, 3.f, 0, 2.f);
    }
}

void AZombie::DisableDetection()
{
    Super::DisableDetection();
    if (DetectionSphere)
        DetectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    bDetectionOverlapping = false;
    bAttackTimming = false;
    TargetActor = nullptr;
}
float AZombie::Attack()
{
    Super::Attack();

    ZombieState = EZONBIE_ST::ATTACK;
    UZombieAnimInstance* Anim = Cast < UZombieAnimInstance>(GetMesh()->GetAnimInstance());
    float AniPlayLength = -1.f;
    if (Anim )
        AniPlayLength = Anim->AnimationPlay(ZombieState);
    return AniPlayLength;
}
void AZombie::BeginPlay()
{
    Super::BeginPlay();

    VariousJombie();//Random Custom Jombie
}

void AZombie::AttackTimming(int AttType)
{
    Super::AttackTimming(AttType);

    Power = AttackPowerArr[AttType];
    bAttackTimming = true;
}

void AZombie::Tick(float DeltaTime)
{
    if ( bDetectionOverlapping )
    {
        if (bAttackTimming)
        {
            if (TargetActor&& TargetActor->ActorHasTag("Player"))
            {
                //FVector CurPos = GetActorLocation() +  DetectionSphere->GetRelativeLocation();
                //float Distance = FVector::DistXY(CurPos, TargetActor->GetActorLocation());
                //UE_LOG(LogTemp, Warning, TEXT("Distance:%f/ SphereRadius:"), Distance, DetectionSphere->GetScaledSphereRadius());
                //if (150.f >= Distance)
                    float  ReceiveApplyDamage = UGameplayStatics::ApplyDamage(TargetActor, Power, TargetActor->GetInstigatorController(), TargetActor, NULL);
                    UE_LOG(LogTemp, Warning, TEXT("ATT:%f"),Power);

            }
            bAttackTimming = false;
        }
    }
}
float AZombie::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (Hp <= 0)
        return 0.f;

    float ReceiveADamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    Hp -= DamageAmount;

    UZombieAnimInstance* animInst = Cast< UZombieAnimInstance>(GetMesh()->GetAnimInstance());
    if (!animInst)
        return 0.f;

    if (Hp <= 0)
    {
        ZombieState = EZONBIE_ST::DIE;
        float aniLength = animInst->AnimationPlay(ZombieState)-0.3f;

        GetCharacterMovement()->StopMovementImmediately();
        GetCharacterMovement()->DisableMovement();
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
        GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        DisableDetection();
        GetController()->Destroy();

        FTimerHandle TimerHandle;
        //GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AZombie::Die, aniLength, false);
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle, ( [this, animInst]() {
                animInst->Montage_Pause(animInst->DieMontage);
            // animInst->Montage_SetPlayRate(animInst->DieMontage, 0.0f);
             Die();
        }),  aniLength, false);
    }
    else
    {
        ZombieState = EZONBIE_ST::DAMAGE;
        float DamageAniTime = animInst->AnimationPlay(ZombieState);
        PauseMoveForDamage(DamageAniTime);
    }
    return ReceiveADamage;
}
void AZombie::Die()
{
    Super::Die();
  /*  GetMesh()->bPauseAnims = true;
    GetMesh()->bNoSkeletonUpdate = true;*/
   // Destroy();
    UE_LOG(LogTemp, Warning, TEXT("Die"));
}
void AZombie::PauseMoveForDamage(float PauseTime)
{
    AAIController* MAIController = Cast<AAIController>(GetController());//StopMovement();
    UBehaviorTreeComponent* BT = nullptr;
    if (MAIController)
    {
        if (MAIController)
        {
            BT = Cast<UBehaviorTreeComponent>(MAIController->GetBrainComponent());//(TEXT("TakeDamage"));
            if (IsValid(BT))
            {
                BT->PauseLogic(TEXT("TakeDamage"));
                MAIController->StopMovement();
            }
        }
    }
    GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandler);

    GetWorld()->GetTimerManager().SetTimer(
        DamageTimerHandler, ([this, MAIController, BT]() {
            BT->ResumeLogic(TEXT("TakeDamage"));
            }), PauseTime+1.f, false
    );
   
}
#pragma region //VariousJombie
void AZombie::VariousJombie()
{
    TMap<FString, FString> CostumeMap =  // Shirt 4개, Pants 2개 , hair 5개,
    {
        {TEXT("Shirt0"), TEXT("SkeletalMesh'/Game/KSW/Resouces/zombie/mesh/male/Clothing/Singlet/SK_Clth_Singlet_M02.SK_Clth_Singlet_M02'")},
        {TEXT("Shirt1"), TEXT("SkeletalMesh'/Game/KSW/Resouces/zombie/mesh/male/Clothing/Tshirt/SK_Clth_Tshirt_M02.SK_Clth_Tshirt_M02'")},
        {TEXT("Shirt2"), TEXT("SkeletalMesh'/Game/KSW/Resouces/zombie/mesh/Female/Clothing/OutfitCheerleader/SK_Clth_Singlet_Cheerleader_F02.SK_Clth_Singlet_Cheerleader_F02'")},
        {TEXT("Shirt3"), TEXT("SkeletalMesh'/Game/KSW/Resouces/zombie/mesh/Female/Clothing/OutfitLingerie/SK_Clth_Bra_Lingerie_F02.SK_Clth_Bra_Lingerie_F02'")},

        {TEXT("Pants0"),TEXT("SkeletalMesh'/Game/KSW/Resouces/zombie/mesh/male/Clothing/LongJeans/SK_Clth_LongPants_M02.SK_Clth_LongPants_M02'")},
        {TEXT("Pants1"),TEXT("SkeletalMesh'/Game/KSW/Resouces/zombie/mesh/Female/Clothing/OutfitCheerleader/SK_Clth_Skirt_Cheerleader_F02.SK_Clth_Skirt_Cheerleader_F02'")},
        {TEXT("Hair0"), TEXT("StaticMesh'/Game/KSW/Resouces/zombie/mesh/male/Hair/SM_Hair_M02.SM_Hair_M02'")},
        {TEXT("Hair1"), TEXT("StaticMesh'/Game/KSW/Resouces/zombie/mesh/male/Hair/SM_HairCap_M02.SM_HairCap_M02'")},
        {TEXT("Hair2"), TEXT("StaticMesh'/Game/KSW/Resouces/zombie/mesh/male/Hair/SM_HairCombined_M02.SM_HairCombined_M02'")},
        {TEXT("Hair3"), TEXT("SkeletalMesh'/Game/KSW/Resouces/zombie/mesh/Female/Hair/SK_Hair_F02.SK_Hair_F02'")},
        {TEXT("Hair4"), TEXT("SkeletalMesh'/Game/KSW/Resouces/zombie/mesh/Female/Hair/SK_HairCap_F02.SK_HairCap_F02'")},
    };

    bool bSelectFeMale = FMath::RandRange(0, 1) == 0 ? true : false;
    FString BodyMeshFilePath = "";
    if (bSelectFeMale)
        BodyMeshFilePath = TEXT("SkeletalMesh'/Game/KSW/Resouces/zombie/mesh/Female/SK_Zombie_F02_01.SK_Zombie_F02_01'");
    else
        BodyMeshFilePath = TEXT("SkeletalMesh'/Game/KSW/Resouces/zombie/mesh/male/AA_SK_Zombie_M02_01.AA_SK_Zombie_M02_01'");

    USkeletalMesh* BodyMesh = LoadObject<USkeletalMesh>(nullptr, *BodyMeshFilePath);
    if (BodyMesh)
        GetMesh()->SetSkeletalMesh(BodyMesh);
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Error! Create Body "));
        return;
    }

    bool bNude = FMath::RandRange(0, 1) == 0 ? true : false;  //누드 바디면 기본 바디로만 진행(속웃상태) 
    if (bNude)
    {
        USkeletalMesh* BraMesh = nullptr;
        USkeletalMesh* FhairMesh = nullptr;
        UStaticMesh* MhairMesh = nullptr;
        FString HairKey = "";
        FString Mhair = "";
        int randHairType = FMath::RandRange(0, 2);
        if (bSelectFeMale)//여성이면 브라자 입힘.
        {
            BraMesh = LoadObject<USkeletalMesh>(nullptr, *CostumeMap["Shirt3"]);
            if (randHairType != 0)
            {
                HairKey = "Hair" + FString::FromInt(FMath::RandRange(3, 4));
                FhairMesh = LoadObject<USkeletalMesh>(nullptr, *CostumeMap[HairKey]);
            }
        }
        else
        {
            if (randHairType != 0)
            {
                HairKey = "Hair" + FString::FromInt(FMath::RandRange(0, 2));
                MhairMesh = LoadObject<UStaticMesh>(nullptr, *CostumeMap[HairKey]);
            }
        }
        SetClothingMeshs(nullptr, BraMesh, FhairMesh, MhairMesh);
        return;
    }

    int32 HairIdx = -1;
    int32 ShirtIdx = -1;
    int32 PantsIdx = -1;
    if (bSelectFeMale)
    {
        HairIdx = FMath::RandRange(3, 4);
        ShirtIdx = FMath::RandRange(2, 3);
        PantsIdx = 1;
    }
    else
    {
        HairIdx = FMath::RandRange(0, 2);
        ShirtIdx = FMath::RandRange(0, 1);
        PantsIdx = 0;
    }

    FString FileLoadPathHair = "Hair" + FString::FromInt(HairIdx);
    FString FileLoadPathShirt = "Shirt" + FString::FromInt(ShirtIdx);
    FString FileLoadPathPants = "Pants" + FString::FromInt(PantsIdx);

    UStaticMesh* HairStaticMesh = nullptr;
    USkeletalMesh* HairMesh = nullptr;
    USkeletalMesh* ShirtMesh = LoadObject<USkeletalMesh>(nullptr, *CostumeMap[FileLoadPathShirt]);
    USkeletalMesh* PantsMesh = LoadObject<USkeletalMesh>(nullptr, *CostumeMap[FileLoadPathPants]);

    if (!bSelectFeMale)
    {
        if (FMath::RandRange(0, 1) == 1)
        {
            //일단 남자 청바지만 맵이 두종류임
            UMaterialInstance* NewMaterial = LoadObject<UMaterialInstance>(nullptr, TEXT("MaterialInstanceConstant'/Game/KSW/Resouces/zombie/Materials/ZombieM02/Clothing/LongJeans/02BlueDirty/MI_Clth_LongJeans_M02_02.MI_Clth_LongJeans_M02_02'"));
            if (NewMaterial)
                PantsMeshComp->SetMaterial(0, NewMaterial);
        }
    }

    if (bSelectFeMale)
        HairMesh = LoadObject<USkeletalMesh>(nullptr, *CostumeMap[FileLoadPathHair]);
    else
        HairStaticMesh = LoadObject<UStaticMesh>(nullptr, *CostumeMap[FileLoadPathHair]);

    SetClothingMeshs(PantsMesh, ShirtMesh, HairMesh, HairStaticMesh);

    //FString LogString = bSelectFeMale ? "True" : "False";
    //UE_LOG(LogTemp, Warning, TEXT("bSelectFeMale : %s"), *LogString);
#pragma region //생성자에서 사용하기
/* static ConstructorHelpers::FObjectFinder<USkeletalMesh> ShirtMeshAsset(*CostumeMap[FileLoadPathShirt]);
   static ConstructorHelpers::FObjectFinder<USkeletalMesh> PantsMeshAsset(*CostumeMap[FileLoadPathPants]);
   static ConstructorHelpers::FObjectFinder<USkeletalMesh> HairMeshAsset = nullptr;
   static ConstructorHelpers::FObjectFinder<UStaticMesh> HairStaticMeshAsset = nullptr;

   if (!bSelectFeMale)
   {
       HairStaticMeshAsset = *CostumeMap[FileLoadPathHair];
       SetClothingMeshs(PantsMeshAsset.Object, ShirtMeshAsset.Object, nullptr, HairStaticMeshAsset.Object);
   }
   else
   {
       HairMeshAsset = *CostumeMap[FileLoadPathHair];
       SetClothingMeshs(PantsMeshAsset.Object, ShirtMeshAsset.Object, HairMeshAsset.Object, nullptr);
   }*/
#pragma endregion
}
#pragma endregion
#pragma region //ApplyRagdoll
void AZombie::ApplyRagdoll(FVector HitDirection)
{
    Super::ApplyRagdoll(HitDirection);
    ZombieState = EZONBIE_ST::DIE;

    //  캡슐 콜리전 비활성화 (레그돌이 제대로 작동하도록 설정)
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->SetSimulatePhysics(true);

    //  이동 불가능하도록 설정 
    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();

    //  스켈레탈 메시를 물리(Ragdoll) 상태로 변경
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

    //  HitDirection 방향으로 힘을 가함 (살짝 뒤로 날아가도록 설정)
    FVector ImpulseForce = HitDirection * 4000.0f + FVector(0, 0.f, 2000.0f); // Y축으로 밀고 위로 튕기기// + FVector(1, 0.f, 0.0f)
    GetMesh()->AddImpulse(ImpulseForce, NAME_None, true);

    // FTimerHandle DestroyTimerHandle;
    // GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AMonster::Destroy, 3.0f, false);
}

#pragma endregion
#pragma region //EndPlay
void AZombie::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}
#pragma endregion