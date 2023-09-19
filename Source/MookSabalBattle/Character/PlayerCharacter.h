// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "MookSabalBattle/Character/CharacterStateComponent.h"
#include "GameFramework/Character.h"
#include "MookSabalBattle/UI/InGameUI.h"
#include "PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetDamage, FName, BoneName, FVector, HitFrom);

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

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY(VisibleAnywhere, Category=Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	UCameraComponent* Camera;

public:
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	AWeapon* CurrentWeapon;
private:

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
	const FVector CamPosWhenFireGun = FVector(300.0f, 69.0f, 46.0f);
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

	UPROPERTY(EditAnywhere)
	float AttackCapsuleColliderHalfHeight;
	UPROPERTY(EditAnywhere)
	float AttackCapsuleColliderRadius;
	float PunchDamage;
	float KickDamage;
	
	UFUNCTION()
	void OnCharacterBeginOverlapWithCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UFUNCTION(BlueprintCallable)
	void SetCharacterAsAlly();
	UFUNCTION(BlueprintCallable)
	void SetCharacterAsEnemy();

private:
	UFUNCTION()
	void Die();

public:
	UPROPERTY()
	FOnGetDamage OnGetDamage;

	UFUNCTION(BlueprintImplementableEvent, Category=Event, meta=(DisplayName="On Hit"))
	void OnHit(FName BoneName, FVector HitFrom);

private:
	UPROPERTY(BlueprintReadWrite, Category="HitPhysAnim", meta=(AllowPrivateAccess=true))
	TArray<FName> HitBones;
	
	UPROPERTY(BlueprintReadWrite, Category="HitPhysAnim", meta=(AllowPrivateAccess=true))
	TArray<FVector> HitFromVector;
	
	UPROPERTY(BlueprintReadWrite, Category="HitPhysAnim", meta=(AllowPrivateAccess=true))
	TArray<float> HitBlendWeights;

	UParticleSystem* BleedingParticle;

	void ShowBleeding(FVector Location, FVector From, FVector Normal);

public:
	FVector GetCameraLocation();
	FVector GetCameraDirection();
	void ReloadGun();
};
