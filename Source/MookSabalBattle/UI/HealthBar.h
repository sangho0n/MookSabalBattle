// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "../Character/CharacterState.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class MOOKSABALBATTLE_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void BindCharacterStat(ACharacterState* State);

private:
	UCanvasPanel* Canvas_Health;
	UProgressBar* HealthBar;
	const float FadeOutTime = 0.6f;
	float AccTimeForFadeOut;
	bool bIsFadeOut;

	
	UFUNCTION()
	void UpdateFadeOutEffect(UCanvasPanel* Canvas,  bool& bIsFading, float& AccTime, const float InDeltaTime, float Threshold = 0.0f);

public:
	UFUNCTION()
	void ShowHealthBar(float HP);
};
