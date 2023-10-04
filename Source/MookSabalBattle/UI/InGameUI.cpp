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

	AccTimeForFadeOut_Equip = 0.0f;
	bIsFadeOut_Equip = false;
}

void UInGameUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateFadeOutEffect(Canvas_Equip, bIsFadeOut_Equip, AccTimeForFadeOut_Equip, InDeltaTime);
	UpdateFadeOutEffect(Canvas_DamageIndicator, bIsFadeOut_DamageIndicator,
		AccTimeForFadeOut_DamageIndicator, InDeltaTime, 1.0f);
}


void UInGameUI::UpdateFadeOutEffect(UCanvasPanel* Canvas, bool& bIsFading, float& AccTime, const float InDeltaTime, float Threshold)
{
	if(!bIsFading) return;
	if(Canvas == Canvas_DamageIndicator)
	{
		float Angle = CalcDamageIndicatorAngle();
		Canvas_DamageIndicator->SetRenderTransformAngle(Angle);
	}
	
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

/**
 * @brief Bind HP with HP Bar percentage, and init opacities.
 * Called on each client(or listen server), the actor who is locally controlled.
 * 
 * @param State 
 */
void UInGameUI::BindCharacterStat(ACharacterState* State)
{
	State->OnHPChanges.AddLambda([this, State](float HP)-> void
	{
		HPBar->SetPercent(HP / State->MaxHP);
	});
	State->OnHPChanges.AddUObject(this, &UInGameUI::ShowBleeding);

	this->Canvas_Bleeding->SetRenderOpacity(0.0f);
	this->Canvas_DamageIndicator->SetRenderOpacity(0.0f);
}

void UInGameUI::SetEquipVisible()
{
	bIsFadeOut_Equip = false;
	Canvas_Equip->SetRenderOpacity(1);
	Canvas_Equip->SetVisibility(ESlateVisibility::Visible);
}

void UInGameUI::SetEquipInvisible()
{
	AccTimeForFadeOut_Equip = 0.0f;
	bIsFadeOut_Equip = true;
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
	if(HP > Bleeding1Offset)
	{
		Image_Bleeding1->SetRenderOpacity(0);
		Image_Bleeding2->SetRenderOpacity(0);
		return;
	}

	Image_Bleeding1->SetRenderOpacity((Bleeding1Offset - HP)/Bleeding1Offset);
	if(HP < Bleeding2Offset)
		Image_Bleeding2->SetRenderOpacity((Bleeding2Offset - HP)/Bleeding2Offset * MaxBleeding2Opacity);
}

void UInGameUI::ShowDamageIndicator(AActor* Causer)
{
	AccTimeForFadeOut_DamageIndicator = 0.0f;
	Canvas_DamageIndicator->SetVisibility(ESlateVisibility::Visible);
	Canvas_DamageIndicator->SetRenderOpacity(1.0f);
	RecentDamageCauser = Causer;
	bIsFadeOut_DamageIndicator = true;
	float Angle = CalcDamageIndicatorAngle();

	Canvas_DamageIndicator->SetRenderTransformAngle(Angle);
}

float UInGameUI::CalcDamageIndicatorAngle()
{
	float Angle;
	auto ShotDirection = RecentDamageCauser->GetActorLocation() - GetOwningPlayerPawn()->GetActorLocation();
	auto OffsetVector = GetOwningPlayerCameraManager()->GetCameraRotation().Vector();
	ShotDirection = FVector(ShotDirection.X, ShotDirection.Y, 0.0f);
	OffsetVector = FVector(OffsetVector.X, OffsetVector.Y, 0.0f);
	ShotDirection.Normalize(); OffsetVector.Normalize();

	if(FVector::CrossProduct(OffsetVector, ShotDirection).Z > 0.0f)
	{
		Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(OffsetVector, ShotDirection)));
	}
	else
	{
		Angle = -FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(OffsetVector, ShotDirection)));
	}

	return Angle;
}

