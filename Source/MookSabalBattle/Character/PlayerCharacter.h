// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MookSabalBattle.h"
#include "MookSabalBattle/Character/CharacterState.h"
#include "GameFramework/Character.h"
#include "MookSabalBattle/UI/InGameUI.h"
#include "PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetDamage, FName, BoneName, FVector, HitFrom);
DECLARE_DELEGATE(FOnNewPlayerReplicationFinished);

UENUM()
enum OUT_LINE
{
	Ally = 1,
	Enemy,
	Weapon
};

UCLASS()
class MOOKSABALBATTLE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
private:
	UPROPERTY(VisibleAnywhere, Category=Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	UCameraComponent* Camera;

public:
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	TObjectPtr<AWeapon> CurrentWeapon;
	
protected:
	UPROPERTY(VisibleAnywhere, Category=State, Replicated)
	ACharacterState* CharacterState;

public:
	void ForwardBack(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);
	
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	AWeapon* OverlappedWeapon;
	
private:
	void ChangeCharacterMode(CharacterMode NewMode);

	TSubclassOf<UInGameUI> InGameUIClass;
	UInGameUI* InGameUI;
	
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TSubclassOf<UUserWidget> EndGameUIClass;
	UUserWidget* EndGameUI;

public:
	UFUNCTION(BlueprintCallable)
	ACharacterState* GetCharacterStateComponent();
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void EquipWeapon_Server(AWeapon* NewWeapon);
	UFUNCTION(NetMulticast, Reliable)
	void EquipWeapon_Multicast();
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OnWeaponStartOverlap_Server(AWeapon* OverlappedWeapon_);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void OnWeaponStartOverlap_Multicast(AWeapon* OverlappedWeapon_);
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OnWeaponEndOverlap_Server();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void OnWeaponEndOverlap_Multicast();

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

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void AttackNonEquip_Server();
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Shoot_Server();
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void StopShooting_Server();
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SwingMelee_Server();
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void AttackNonEquip_Multicast();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Shoot_Multicast();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void StopShooting_Multicast();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SwingMelee_Multicast();
	
	UFUNCTION(BlueprintCallable)
	/**
	* @brief Method for hit detection.
	* It is bound to the OnHitCheck delegate of the anim instance, and
	* the OnHitCheck delegate is broadcasted by AnimNotify_HitCheck.
	*
	*  Call the appropriate method with given param and CharacterState->CurrentMode
	* 
	* @param CurrCombo 
	*/
	void Hit(int32 CurrCombo);

private:
	UFUNCTION(Server, Reliable)
	void ApplyDamageEventsOnServer(APlayerCharacter* const &Causer, const TArray<FPointDamageEvent> &HitResults);
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHitEffect(FPointDamageEvent HitResult, APlayerCharacter* Causer);
	
	UFUNCTION()
	TArray<FPointDamageEvent> Punch();
	UFUNCTION()
	TArray<FPointDamageEvent> Kick();

	UPROPERTY(EditAnywhere)
	float AttackCapsuleColliderHalfHeight;
	UPROPERTY(EditAnywhere)
	float AttackCapsuleColliderRadius;
	float PunchDamage;
	float KickDamage;
	
	UFUNCTION()
	void OnCharacterBeginOverlapWithCharacter(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	void OnCharacterEndOverlapWithCharacter(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex);

	TArray<APlayerCharacter*> OverlappedAlly;
	
public:
	UFUNCTION(BlueprintCallable)
	bool IsSameTeam(APlayerCharacter* OtherPlayer);

private:
	UFUNCTION(Server, Reliable)
	void Die_Server();
	UFUNCTION(NetMulticast, Reliable)
	void Die_Multicast();

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

	UFUNCTION()
	void StopAttacking();
	
	UFUNCTION()
	void EndReloading();

public:
	FVector GetCameraLocation();
	FVector GetCameraDirection();

	UFUNCTION(Server, Reliable)
	void ReloadGun_Server();
	UFUNCTION(NetMulticast, Reliable)
	void ReloadGun_Multicast();

	FOnNewPlayerReplicationFinished OnNewPlayerReplicationFinished;
private:
	UFUNCTION()
	void AfterReplication();

	static int InitFinishedPlayer;
	static FCriticalSection CS_InitFinishedPlayer;

	/* static */ void SetPlayerOutline();

	UFUNCTION(BlueprintCallable)
	void Respawn();


	
public:
	UFUNCTION(NetMulticast, Reliable)
	void InitPlayer(const bool bIsRedTeam);
	UFUNCTION(BlueprintNativeEvent)
	void InitWidgets();
};
