

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
    with listen server(개선중)
    <br />
    <br />
<a href="https://drive.google.com/file/d/1oEgK-jrPDGn802M5s4zCV0Jbppc5Ppim/view?usp=sharing">설명 영상 보기</a> 
  </p>
</div>

# About
UE 5.2 버전을 사용하여 만든 1인 프로젝트 멀티플레이 TPS 게임의 원격저장소입니다.

제작 기간은 2023. 8. 7 ~ 입니다.

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

## 로비
모든 플레이어는 게임 플레이가 이루어지는 세션을 생성하고, 여러 세션 중에서 자신이 원하는 세션에 참가 요청을 보낼 수 있다. (현재 LAN에서만 동작합니다.)

### 방 만들기
자신의 닉네임과 참가인원(2 ~ 6명; 짝수), 로컬 네트워크 이용 여부를 입력하고 Confirm 버튼을 누르면 세션을 열어둔 채 대기한다.
![image](https://github.com/sangho0n/MookSabalBattle/assets/54069713/19e28802-a83f-4405-bb11-4c6b147ffc04)

### 방 찾기
초기 로비 창에서 Join 버튼을 누르면 자동으로 검색이 이루어진다. 자신이 원하는 방을 선택할 수 있으며, 기존에 이미 선택했던 방이 있을 때 새로운 방을 선택하면 자동으로 기존 방에 대한 선택이 해제된다.
세션 정보 중 [호스트의 이름, 현재원/정원]이 화면에 노출된다.

Refresh 버튼을 통해 다시 검색할 수 있다.
![image](https://github.com/sangho0n/MookSabalBattle/assets/54069713/648a2df0-3f61-40ce-8094-6a27a677cf38)
(Null subsystem을 활용한 LAN 세션의 경우, 현재원 여부를 알 수 없기 때문에 null subsys 텍스트로 대체해두었습니다.)

### 방 참가하기
원하는 방을 선택한 후 닉네임을 입력하고 Confirm 버튼을 누르면 방에 참가 요청을 보낼 수 있다. 유효한 요청일 때 세션 참가가 이루어지며, 정원이 다 찼을 경우 게임이 시작된다.
- 유효하지 않은 요청 유형
	- 닉네임이 10자를 넘어가는 경우
	 - 중복된 닉네임을 가진 유저가 이미 세션에 참가하고 있는 경우
	 - 이미 시작된 세션인 경우
![image](https://github.com/sangho0n/MookSabalBattle/assets/54069713/39e8b8c0-99e2-4a97-9aab-92a145152436)

### 세션 종료
호스트가 설정한 방의 참가 인원이 채워지면 게임이 시작된다.
일정 시간 후 게임이 종료되며, 승패여부와 각 플레이어의 세부 정보가 화면에 표시된다.

게임 결과가 고지된 후 일정 시간이 지나면 로비화면으로 되돌아온다.
![GameResult](https://github.com/sangho0n/MookSabalBattle/assets/54069713/3b5ebdb8-e62c-45c1-82f4-d17ce61c9f7c)

