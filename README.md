

<div align="center">
  <a href="https://github.com/othneildrew/Best-README-Template">
<p align="center">
  <img src="https://github.com/sangho0n/MookSabalBattle/assets/54069713/b0cd5403-aec0-419e-b115-3648b3c355c8">
</p>

  </a>


<h3 align="center">묵사발 배틀</h3>

  <p align="center">
    멀티플레이 TPS 게임
    <br />
    with listen server(미완성)
    <br />
    <br />
<!--     <a href="">데모 영상 보기(준비중)</a> ·
    <a href="">화면 설계</a> -->
  </p>
</div>

# About
UE 5.2 버전을 사용하여 만든 1인 프로젝트 멀티플레이 TPS 게임의 원격저장소입니다.

제작 기간은 2023. 8. 7 ~ 입니다. (진행중)

브랜치명 및 커밋명은 [다음](https://github.com/sangho0n/MookSabalBattle/issues/1)을 따라 작성하였습니다. 또한 각 브랜치별 구현 예정 사항을 깃헙 이슈를 이용하여 등록, 관리하였습니다.

# 구현 사항
## Loco Motion
c++ 애님인스턴스 클래스를 이용해 애니메이션 상태값를 관리하되, 효과적인 상태전이를 위해 애니메이션 블루프린트를 적극 활용함.

캐릭터의 모드(비무장, 한손무기, 총)에 따른 애니메이션과, 일반적인 애니메이션을 상/하체로 블랜딩하여 다양한 움직임을 보일 수 있도록 구현함.

![image](https://github.com/sangho0n/MookSabalBattle/assets/54069713/e8589323-388b-454b-a061-2e6096066fb9)

### Directional Move
총을 든 상태에서는 컨트롤러가 캐릭터를 회전시키지 않는다.(총구가 항상 전방을 향하도록 구현함). 이때 하체의 움직임을 블렌드스페이스를 이용하여 구현.

![dimove](https://github.com/sangho0n/MookSabalBattle/assets/54069713/5f3852d7-b40b-42ee-9352-a29fb9c3e48d)

(정지상태에서는 AimOffset을 활용해 총구가 전방을 향하도록 구현.)

![AimOffset](https://github.com/sangho0n/MookSabalBattle/assets/54069713/b91809ab-ad60-4904-a470-3ddb97ccc9b7)


## 공격
### 비무장 상태에서의 공격
비무장 상태에서는 두 손과 발을 이용한 공격을 한다.

공격 애니메이션은 애니메이션 몽타주를 통해 애니메이션 인스턴스로 전달되며, 컨트롤러에서 공격 요청이 여러번 들어올 경우 타이밍에 따라 몽타주 내 연속동작(콤보 공격)이 재생될 수 있다.

![비무장-1](https://github.com/sangho0n/MookSabalBattle/assets/54069713/f37b984e-782a-477c-bcb4-5583eaca134e)

### 한손무기 상태에서의 공격
한손무기의 종류에는 한손검, 도끼, 곡괭이가 있다. 무기의 종류마다 공격 사거리가 다르며, 공격 시 대미지 전달 메커니즘 역시 다르다.
무기가 훑고 간 경로를 따라 트레일 이펙트가 남는다.

![한손-2](https://github.com/sangho0n/MookSabalBattle/assets/54069713/525cde9c-4f3f-4138-900f-504434170057)

### 총을 든 상태에서의 공격
총을 습득하면 이전까지의 움직임과 달리 캐릭터의 시선방향과 카메라의 방향이 일치하게 된다. 화면 중앙에 조준점이 추가된다.

![총-3](https://github.com/sangho0n/MookSabalBattle/assets/54069713/dc1c34eb-166b-439b-a42e-15f2bac4a566)

## 피격

### Bleeding 파티클

피격 방향의 반대방향으로 파티클 생성

![Screenshot_20231014_213125_Samsung Notes](https://github.com/sangho0n/MookSabalBattle/assets/54069713/acb3f0f6-9665-432a-97ea-4849c95f7ba3)

### 피격 애니메이션

Locomotion 애니메이션과 물리기반 애니메이션을 블렌딩하여 구현. 타격점의 bone이 타격격방향대로 살짝 밀려났다가 돌아온다.

https://github.com/sangho0n/MookSabalBattle/assets/54069713/a59bdb54-ae48-4553-8028-57b4d16fea97

### Damage Indicator

피격시 나를 때린 적이 어디있는지 쉽게 알게하기 위해 각 유저의 화면에 피격방향 표시.

https://github.com/sangho0n/MookSabalBattle/assets/54069713/43aa3865-04fa-4b2e-8001-650cbc61c968

## 동기화

언리얼 클라이언트-서버 아키텍쳐에 맞춰, 모든 권한은 서버에서 관리. 움직임 Input이 들어오면 서버RPC를 통해 상태관리 및 검증 후
NetMulticast RPC를 통해 리슨서버와 각 클라이언트들을 동기화

```cpp
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SwingMelee_Server();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SwingMelee_Multicast();
```

다만 공격-피격판정의 경우 "보이는 것"을 기준으로 하기 위해 각 클라이언트(혹은 리슨서버)에서 Detection 후 서버RPC를 통해 대미지 적용
```cpp

void APlayerCharacter::Hit(int32 CurrCombo)
{
	if(!IsLocallyControlled()) return;

  // 현재 모드에 따라 대미지 이벤트 리스트 생성
  // ...
  
	ApplyDamageEventsOnServer(this, DamageEvents);
}
```

https://github.com/sangho0n/MookSabalBattle/assets/54069713/44bd5913-2643-4bc1-82d1-4099b67e1475

## 로비
소켓통신을 이용해 호스팅/조인 기능 구현(예정)

https://github.com/sangho0n/MookSabalBattle/assets/54069713/b545ab58-c027-4bfd-8b22-c3c786616b85

### 호스팅하기
### 참가하기
