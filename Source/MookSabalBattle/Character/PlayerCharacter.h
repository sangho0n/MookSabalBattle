// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "MookSabalBattle/Character/CharacterStateComponent.h"
#include "GameFramework/Character.h"
#include "MookSabalBattle/UI/InGameUI.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class MOOKSABALBATTLE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, Category=Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category=Weapon)
	AWeapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, Category=State)
	UCharacterStateComponent* CharacterState;

public:
	void ForwardBack(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);

	CharacterMode GetCurrentMode();
	
	AWeapon* OverlappedWeapon;
	
private:
	void ChangeCharacterMode(CharacterMode NewMode);

	TSubclassOf<UInGameUI> InGameUIClass;
	UInGameUI* InGameUI;

public:
	UFUNCTION(BlueprintCallable)
	UCharacterStateComponent* GetCharacterStateComponent();
	
	UFUNCTION(BlueprintCallable)
	bool EquipWeapon(AWeapon* NewWeapon);
	
	UFUNCTION(BlueprintCallable)
	void OnWeaponStartOverlap(AWeapon* OverlappedWeapon);
	
	UFUNCTION(BlueprintCallable)
	void OnWeaponEndOverlap();

private:
	const FVector CamPosWhenGunMode = FVector(0.0f, 50.0f, 50.0f);
	FVector CurrentCamPos;
	FVector DesiredCamPos;
	bool bInterpingCamPos;
	float accTime;
	const float MaxWalkSpeed = 600.0f;

public:
	virtual void Jump() override;

	UFUNCTION(BlueprintCallable)
	void AttackNonEquip();
	UFUNCTION(BlueprintCallable)
	void Shoot();
	UFUNCTION(BlueprintCallable)
	void StopShooting();
	UFUNCTION(BlueprintCallable)
	void SwingMelee();

	UFUNCTION(BlueprintCallable)
	void Hit(int32 CurrCombo);

private:
	UFUNCTION()
	void Punch();
	UFUNCTION()
	void Kick();
	UFUNCTION()
	void HitWithSword();
	UFUNCTION()
	void HitWithAxe();
	UFUNCTION()
	void HitWithPick();
	UFUNCTION()
	void HitWithGun();
	
	
	UFUNCTION()
	void OnCharacterBeginOverlapWithCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UCapsuleComponent* PunchKickCollider;
};
