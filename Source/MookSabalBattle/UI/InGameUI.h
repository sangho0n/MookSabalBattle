// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "InGameUI.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void BindCharacterStat();

private:

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	UCanvasPanel* Equip;
	UCanvasPanel* Aim;

public:
	void SetEquipVisible();
	void SetEquipInvisible();

private:
	const float FadeOutTime = 0.6f;
	float AccTimeForFadeOut;
	bool bIsFadeOut;
};
