// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "Components/ActorComponent.h"
#include "CharacterStateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHPIsZero);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChanges, float);

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
	UPROPERTY(EditAnywhere, Category=Status, meta=(AllowPrivateAccess=true))
	CharacterMode CurrentMode;
	bool bIsEquippable;
	bool bIsAttacking;
	UPROPERTY(EditAnywhere, Category=Status, meta=(AllowPrivateAccess=true))
	float HP;
public:
	const float MaxHP = 200.0f;
private:
	FString NickName;
	bool bIsFriendly;

public:
	FOnHPIsZero OnHPIsZero;
	FOnHPChanges OnHPChanges;

public:
	void ApplyDamage(float damage);
	
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
	
	[[nodiscard]]
	bool IsAttacking() const
	{
		return bIsAttacking;
	}

	void SetIsAttacking(bool bIsAttacking_)
	{
		this->bIsAttacking = bIsAttacking_;
	}
	[[nodiscard]]
	float GetHP() const
	{
		return HP;
	}

	void SetHP(float HP_)
	{
		this->HP = HP_;
	}

	[[nodiscard]]
	FString GetNickName() const
	{
		return NickName;
	}

	void SetNickName(const FString& NickName_)
	{
		this->NickName = NickName_;
	}

	[[nodiscard]]
	bool IsFriendly() const
	{
		return bIsFriendly;
	}

	void SetFriendly(bool flag)
	{
		bIsFriendly = flag;
	}
#pragma endregion getter_setter
};
