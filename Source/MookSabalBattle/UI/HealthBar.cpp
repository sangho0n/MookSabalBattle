// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBar.h"

#include "MookSabalBattle/Character/CharacterStateComponent.h"

void UHealthBar::NativeConstruct()
{
	Super::NativeConstruct();

	this->Canvas_Health = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Health_Canvas")));
	this->HealthBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HealthBar")));

	Canvas_Health->SetVisibility(ESlateVisibility::Hidden);

	AccTimeForFadeOut = 0.0f;
	bIsFadeOut = false;
}

void UHealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateFadeOutEffect(Canvas_Health, bIsFadeOut, AccTimeForFadeOut, InDeltaTime, 2.0f);
}

void UHealthBar::BindCharacterStat(UCharacterStateComponent* State)
{
	State->OnHPChanges.AddLambda([this, State](float HP) -> void
	{
		HealthBar->SetPercent(HP / State->MaxHP);
	});
	State->OnHPChanges.AddUObject(this, &UHealthBar::ShowHealthBar);
}


void UHealthBar::UpdateFadeOutEffect(UCanvasPanel* Canvas, bool& bIsFading, float& AccTime, const float InDeltaTime, float Threshold)
{
	if(!bIsFading) return;
	
	AccTime += InDeltaTime;
	float Opacity = (FadeOutTime + Threshold - AccTime) / FadeOutTime;

	if (Opacity < KINDA_SMALL_NUMBER)
	{
		Canvas->SetVisibility(ESlateVisibility::Hidden);
		bIsFading = false;
		Opacity = 0.0f;
	}

	Canvas->SetRenderOpacity(Opacity);
}


void UHealthBar::ShowHealthBar(float HP)
{
	MSB_LOG(Warning, TEXT("show 헬스바"));
	bIsFadeOut = true;
	AccTimeForFadeOut = 0.0f;
	
	Canvas_Health->SetVisibility(ESlateVisibility::Visible);
	Canvas_Health->SetRenderOpacity(1.0f);
}

