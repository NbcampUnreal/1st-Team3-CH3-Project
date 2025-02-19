#include "Intro/IntroHUD.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UIntroHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->OnClicked.Clear();
		StartButton->OnClicked.AddDynamic(this, &UIntroHUD::GameStart);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.Clear();
		ExitButton->OnClicked.AddDynamic(this, &UIntroHUD::GameExit);
	}
}

void UIntroHUD::GameStart()
{
	UE_LOG(LogTemp, Warning, TEXT("Click GameStart!!!"));
}

void UIntroHUD::GameExit()
{
	UKismetSystemLibrary::QuitGame(this, 0, EQuitPreference::Quit, false);
}
