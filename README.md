

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
    Only client side code repository(미완성)
    <br />
    <br />
<!--     <a href="">데모 영상 보기(준비중)</a> ·
    <a href="">화면 설계</a> -->
  </p>
</div>

# About
UE 5.2 버전을 사용하여 만든 1인 프로젝트 멀티플레이 TPS 게임의 원격저장소입니다.

제작 기간은 2023. 8. 7 ~ 입니다. (진행중)

에셋의 저작권 이슈로 인해 Content 폴더를 모두 ignore해둔 상태이나, 완성 후 문제가 있는 에셋을 제외한 모든 내용을 공개할 예정입니다.(블루프린트, 블렌드스페이스 등)

브랜치명 및 커밋명은 [다음](https://github.com/sangho0n/MookSabalBattle/issues/1)을 따라 작성하였습니다. 또한 각 브랜치별 구현 예정 사항을 깃헙 이슈를 이용하여 등록, 관리하였습니다.

# 구현 사항
### 비무장 상태에서의 공격
비무장 상태에서는 두 손과 발을 이용한 공격을 한다.

공격 애니메이션은 애니메이션 몽타주를 통해 애니메이션 인스턴스로 전달되며, 컨트롤러에서 공격 요청이 여러번 들어올 경우 타이밍에 따라 몽타주 내 연속동작이 재생될 수 있다.

![비무장-1](https://github.com/sangho0n/MookSabalBattle/assets/54069713/f37b984e-782a-477c-bcb4-5583eaca134e)

### 한손무기 상태에서의 공격
한손무기의 종류에는 한손검, 도끼, 곡괭이가 있다. 무기의 종류마다 공격 사거리가 다르며, 공격 시 대미지 전달 메커니즘 역시 다르다.(대미지 부분 구현 예정)

![한손-2](https://github.com/sangho0n/MookSabalBattle/assets/54069713/525cde9c-4f3f-4138-900f-504434170057)

### 총을 든 상태에서의 공격
총을 습득하면 이전까지의 움직임과 달리 캐릭터의 시선방향과 카메라의 방향이 일치하게 된다. 화면 중앙에 조준점이 추가된다.

![총-3](https://github.com/sangho0n/MookSabalBattle/assets/54069713/dc1c34eb-166b-439b-a42e-15f2bac4a566)
