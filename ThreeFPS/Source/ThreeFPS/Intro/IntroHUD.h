#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IntroHUD.generated.h"

class UButton;

UCLASS()
class THREEFPS_API UIntroHUD : public UUserWidget
{
	GENERATED_BODY()

public:
// ������ ȭ���� ������ �� �ڞ����� ����Ǵ� �ʱ�ȭ �Լ�
	virtual void NativeConstruct() override;

	
private:
	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;
	UPROPERTY(meta = (BindWidget))
	UButton* SettingButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UFUNCTION()
	void GameStart();

	UFUNCTION()
	void Setting();

	UFUNCTION()
	void GameExit();
};
