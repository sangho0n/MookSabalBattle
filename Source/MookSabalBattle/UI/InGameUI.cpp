// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI.h"
#include "SlateCore/Public/Widgets/Images/SImage.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	this->Canvas_Equip = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Equip_Canvas")));
	this->Canvas_Aim = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Aim_Canvas")));
	this->Canvas_Bleeding = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Bleeding_Canvas")));
	this->Image_Bleeding1 = Cast<UWidget>(GetWidgetFromName(TEXT("Bleeding_1")));
	this->Image_Bleeding2 = Cast<UWidget>(GetWidgetFromName(TEXT("Bleeding_2")));
	this->Canvas_DamageIndicator = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("DamageIndicator_Canvas")));
	this->HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HP_Bar")));

	Canvas_Equip->SetVisibility(ESlateVisibility::Hidden);
	Canvas_Aim->SetVisibility(ESlateVisibility::Hidden);
	Canvas_Bleeding->SetVisibility(ESlateVisibility::Visible);

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
			Canvas_Equip->SetVisibility(ESlateVisibility::Hidden);
			bIsFadeOut = false;
			return;
		}
		Canvas_Equip->SetRenderOpacity(opacity);
	}
}

void UInGameUI::BindCharacterStat(UCharacterStateComponent* State)
{
	State->OnHPChanges.AddLambda([this, State](float HP)-> void
	{
		HPBar->SetPercent(HP / State->MaxHP);
	});
	State->OnHPChanges.AddUObject(this, &UInGameUI::ShowBleeding);
	State->OnHPChanges.Broadcast(200.0f);

	this->Canvas_Bleeding->SetRenderOpacity(1.0f);
	this->Canvas_DamageIndicator->SetRenderOpacity(0.0f);
}

void UInGameUI::SetEquipVisible()
{
	Canvas_Equip->SetRenderOpacity(1);
	Canvas_Equip->SetVisibility(ESlateVisibility::Visible);
}

void UInGameUI::SetEquipInvisible()
{
	AccTimeForFadeOut = 0.0f;
	bIsFadeOut = true;
}

void UInGameUI::SetAimVisible()
{
	Canvas_Aim->SetVisibility(ESlateVisibility::Visible);
}

void UInGameUI::SetAimInvisible()
{
	Canvas_Aim->SetVisibility(ESlateVisibility::Hidden);
}

void UInGameUI::ShowBleeding(float HP)
{
	MSB_LOG(Warning, TEXT("asdfasdfasdfasdf"));
	if(HP > Bleeding1Offset)
	{
		Image_Bleeding1->SetRenderOpacity(0);
		Image_Bleeding2->SetRenderOpacity(0);
		return;
	}

	MSB_LOG(Warning, TEXT("asdfasdfasdfasdf"));
	Image_Bleeding1->SetRenderOpacity((Bleeding1Offset - HP)/Bleeding1Offset);
	if(HP < Bleeding2Offset)
		Image_Bleeding2->SetRenderOpacity((Bleeding2Offset - HP)/Bleeding2Offset * MaxBleeding2Opacity);
}

