// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Components/ActorComponent.h"
#include "CharacterStateComponent.generated.h"

UENUM(BlueprintType)
enum class CharacterMode
{
	NON_EQUIPPED,
	GUN,
	MELEE
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOOKSABALBATTLE_API UCharacterStateComponent : public UActorComponent
{
private:
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool bIsEquipped;
	CharacterMode CurrentMode;
	bool bIsEquippable;


#pragma region getter_setter
public:
	void SetIsEquipped(bool bIsEquipped_)
	{
		this->bIsEquipped = bIsEquipped_;
	}

	void SetCurrentMode(CharacterMode CurrentMode_)
	{
		this->CurrentMode = CurrentMode_;
	}

	[[nodiscard]]
	bool IsEquipped() const
	{
		return bIsEquipped;
	}

	[[nodiscard]]
	CharacterMode GetCurrentMode() const
	{
		return CurrentMode;
	}
	
	[[nodiscard]]
	bool IsEquippable() const
	{
		return bIsEquippable;
	}

	void SetIsEquippable(bool bIsEquippable_)
	{
		this->bIsEquippable = bIsEquippable_;
	}
#pragma endregion getter_setter
};
