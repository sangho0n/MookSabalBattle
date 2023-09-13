// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	this->Equip = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Equip_Canvas")));
	this->Aim = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Aim_Canvas")));
	this->HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HP_Bar")));

	Equip->SetVisibility(ESlateVisibility::Hidden);
	Aim->SetVisibility(ESlateVisibility::Hidden);

	AccTimeForFadeOut = 0.0f;
	bIsFadeOut = false;
}

void UInGameUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(bIsFadeOut)
	{
		AccTimeForFadeOut += InDeltaTime;
		auto opacity = (FadeOutTime - AccTimeForFadeOut) / FadeOutTime;
		if(opacity < KINDA_SMALL_NUMBER)
		{
			Equip->SetVisibility(ESlateVisibility::Hidden);
			bIsFadeOut = false;
			return;
		}
		Equip->SetRenderOpacity(opacity);
	}
}

void UInGameUI::BindCharacterStat(UCharacterStateComponent* State)
{
	MSB_LOG(Warning, TEXT("binding states"));
	MSB_LOG(Warning, TEXT("is HP bar valid or not %d"), HPBar->IsValidLowLevel());
	State->OnHPChanges.AddLambda([this, State](float HP)-> void
	{
		HPBar->SetPercent(HP / State->MaxHP);
	});
	State->OnHPChanges.Broadcast(200.0f);
}

void UInGameUI::SetEquipVisible()
{
	Equip->SetRenderOpacity(1);
	Equip->SetVisibility(ESlateVisibility::Visible);
}

void UInGameUI::SetEquipInvisible()
{
	AccTimeForFadeOut = 0.0f;
	bIsFadeOut = true;
}

void UInGameUI::SetAimVisible()
{
	Aim->SetVisibility(ESlateVisibility::Visible);
}

void UInGameUI::SetAimInvisible()
{
	Aim->SetVisibility(ESlateVisibility::Hidden);
}

