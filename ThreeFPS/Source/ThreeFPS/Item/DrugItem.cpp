#include "Item/DrugItem.h"
#include "Character/ThreeFPSCharacter.h"

void ADrugItem::Use(AThreeFPSCharacter* player)
{
	Super::Use(player);

	UE_LOG(LogTemp, Warning, TEXT("���� ���! �÷��̾� ü�� 30 ����"));
	player->DecreaseMutation(30);
}