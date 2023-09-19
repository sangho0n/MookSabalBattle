// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "MookSabalBattle/Character/CharacterStateComponent.h"
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
	
	void BindCharacterStat(UCharacterStateComponent* State);

private:

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	UCanvasPanel* Canvas_Equip;
	UCanvasPanel* Canvas_Aim;
	UCanvasPanel* Canvas_Bleeding;
	UWidget* Image_Bleeding1;
	UWidget* Image_Bleeding2;
	UCanvasPanel* Canvas_DamageIndicator;
	UProgressBar* HPBar;

public:
	void SetEquipVisible();
	void SetEquipInvisible();
	void SetAimVisible();
	void SetAimInvisible();

private:
	const float FadeOutTime = 0.6f;
	float AccTimeForFadeOut_Equip;
	bool bIsFadeOut_Equip;
	float AccTimeForFadeOut_DamageIndicator;
	bool bIsFadeOut_DamageIndicator;
	const float Bleeding1Offset = 100.0f;
	const float Bleeding2Offset = 65.0f;
	const float MaxBleeding2Opacity = 0.384f;
	UFUNCTION()
	void ShowBleeding(float HP);

	UFUNCTION()
	void UpdateFadeOutEffect(UCanvasPanel* Canvas,  bool& bIsFading, float& AccTime, const float InDeltaTime, float Threshold = 0.0f);

public:
	UFUNCTION()
	void ShowDamageIndicator(AActor* Causer);
private:
	AActor* RecentDamageCauser;
	float CalcDamageIndicatorAngle();
};
