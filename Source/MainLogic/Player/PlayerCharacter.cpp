// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine.h"
#include "MyAnimInstance.h"
#include "DrawDebugHelpers.h"
#include "MyMatineeCameraShake.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "MyPlayerController.h"
#include "Player_Skill.h"
#include "Player_Health.h"
#include "BasicEnemy.h"
#include "Components/WidgetComponent.h"
#include "PlayerStatus.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Player_State.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	/// <summary>
	/// Base
	/// </summary>
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 20.0f;
	CameraBoom->bUsePawnControlRotation = true;


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	Tags.Add("RealCharacter");


	Player_Skill = CreateDefaultSubobject<UPlayer_Skill>(TEXT("Player Skill"));
	Player_Skill->SetIsReplicated(true);

	Player_State = CreateDefaultSubobject<UPlayer_State>(TEXT("Player State"));
	Player_Health = CreateDefaultSubobject<UPlayer_Health>(TEXT("Player Health"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	MyAnim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	MyAnim->OnHitedPause.AddUObject(Player_Health, &UPlayer_Health::StartHitedPause);

	MyController = Cast<AMyPlayerController>(GetOwner());
	if (MyController)
	{
		MyController->Myplayer = this;
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForWard", this, &APlayerCharacter::MoveForWard);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);

	InputComponent->BindAction<TDelegate<void(FName)>>("Attack", IE_Pressed, Player_Skill, &UPlayer_Skill::StartSkill, FName("AttackMelee"));
	InputComponent->BindAction<TDelegate<void(FName)>>("DashAttack", IE_Pressed, Player_Skill, &UPlayer_Skill::StartSkill, FName("DashAttack"));
	InputComponent->BindAction<TDelegate<void(FName)>>("UpperWind", IE_Pressed, Player_Skill, &UPlayer_Skill::StartSkill, FName("AirLaunch"));
	InputComponent->BindAction<TDelegate<void(FName)>>("AirCombo", IE_Pressed, Player_Skill, &UPlayer_Skill::StartSkill, FName("AirCombo"));
	InputComponent->BindAction<TDelegate<void(FName)>>("SwordWave", IE_Pressed, Player_Skill, &UPlayer_Skill::StartSkill, FName("SlashWave"));
	InputComponent->BindAction<TDelegate<void(FName)>>("SwordWave", IE_Released, Player_Skill, &UPlayer_Skill::StartSkill, FName("SlashWave"));
	InputComponent->BindAction<TDelegate<void(FName)>>("DashCombo_Input", IE_Released, Player_Skill, &UPlayer_Skill::StartSkill, FName("ChainLightningDash"));
	InputComponent->BindAction<TDelegate<void(FName)>>("MagicSword", IE_Released, Player_Skill, &UPlayer_Skill::StartSkill, FName("GreatSword"));
	InputComponent->BindAction<TDelegate<void(FName)>>("Ultimate1", IE_Released, Player_Skill, &UPlayer_Skill::StartSkill, FName("BlossomTempest"));


	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::StartJump);
	InputComponent->BindAction("Roll", IE_Pressed, this, &APlayerCharacter::Roll);


	InputComponent->BindAction("Defence", IE_Pressed, this, &APlayerCharacter::StartDefence);
	InputComponent->BindAction("Defence", IE_Released, this, &APlayerCharacter::EndDefence); // 아IE_Released구나 pressed가아니라
}

void APlayerCharacter::LookUp(float AxisValue)
{
	if (MyController->bShowMouseCursor) return;
	AddControllerPitchInput(AxisValue);
}

void APlayerCharacter::Turn(float AxisValue)
{
	if (MyController->bShowMouseCursor) return;
	AddControllerYawInput(AxisValue);
}
void APlayerCharacter::MoveForWard(float value)
{
	SetForwardDirValue(value);
	if (!Controller) 
		return;
	if (!Player_State->HasState(static_cast<uint8>(EPlayerStateFlags::CanMove)))
		return;
	
	// 어느 쪽이 전방인지 알아내어 플레이어가 그 방향으로 이동하고자 한다고 기록합니다.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, value);
}

void APlayerCharacter::MoveRight(float value)
{
	SetRightDirValue(value);
	if (!Controller)
		return;
	if (!Player_State->HasState(static_cast<uint8>(EPlayerStateFlags::CanMove)))
		return;
	// 어느 쪽이 오른쪽인지 알아내어 플레이어가 그 방향으로 이동하고자 한다고 기록합니다.
	
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, value);
}

void APlayerCharacter::SetForwardDirValue_Implementation(float value)
{
	ForwardDirValue = value;
}

void APlayerCharacter::SetRightDirValue_Implementation(float value)
{
	RightDirValue = value;
}


void APlayerCharacter::StartJump()
{
	if (Player_State->HasState(static_cast<uint8>(EPlayerStateFlags::CanAttack)) == false)
		return;

	PlayAnimMontage(JumpAnim, 1.f);
	bPressedJump = true;
}

void APlayerCharacter::StopJump()
{
}

int APlayerCharacter::DirToIndex(float Forward, float Right)
{
	if (Forward > 0)       return (Right > 0) ? 2 : (Right < 0 ? 8 : 1);
	else if (Forward < 0)  return (Right > 0) ? 4 : (Right < 0 ? 6 : 5);
	else                   return (Right > 0) ? 3 : (Right < 0 ? 7 : 0);
}

void APlayerCharacter::StartDefence()
{
	if (!GetCharacterMovement()->IsMovingOnGround()) return;
	if (Player_State->HasState(static_cast<uint8>(EPlayerStateFlags::CanAttack)) == false)
		return;

	Player_State->SetAllStatesChecked(false);
	Player_State->UseStamina(BasicStaminaCost);

	PlayAnimMontage(DefenceAnim, 1.f);
}
void APlayerCharacter::EndDefence()
{
	if (isDefence)
	{
		isDefence = false;
		MyAnim->Montage_Resume(GetCurrentMontage());
	}
}
void APlayerCharacter::ResetGravity()
{
	GetCharacterMovement()->GravityScale = 1.f;
}

void APlayerCharacter::StartResetGravityTimer(float time)
{
	GetWorldTimerManager().ClearTimer(LadingTH);
	GetWorldTimerManager().SetTimer(LadingTH, this, &APlayerCharacter::ResetGravity, time, false);

}

void APlayerCharacter::ClearGravityTimer()
{
	GetWorldTimerManager().ClearTimer(LadingTH);
}
void APlayerCharacter::Roll_Implementation()
{
	Roll_Multicast();
}
void APlayerCharacter::Roll_Multicast_Implementation()
{
	if (!GetCharacterMovement()->IsMovingOnGround()) return;
	if (Player_State->HasState(static_cast<uint8>(EPlayerStateFlags::CanRoll)) == false)
		return;

	Player_State->RemoveState(static_cast<uint8>(EPlayerStateFlags::CanRoll));
	Player_State->UseStamina(BasicStaminaCost);
	

	Player_Health->ClearHitedTime(); //만약 맞아서 hited타임돌고있었을수 잇으니까 ///그럴경우 setState(true)는 roll애니메이션 mt_end에서 작동

	const int dir = DirToIndex(ForwardDirValue, RightDirValue);

	// Yaw 각도 매핑 테이블 (인덱스 = dir)
	const float DirectionAngles[9] = {
		0.f,    // 0: 제자리
		0.f,    // 1: 정면
		45.f,   // 2: 앞오른쪽
		90.f,   // 3: 오른쪽
		135.f,  // 4: 뒤오른쪽
		180.f,  // 5: 뒤
		225.f,  // 6: 뒤왼쪽
		270.f,  // 7: 왼쪽
		315.f   // 8: 앞왼쪽
	};

	const FRotator RollRotation = FRotator(0.0f, GetControlRotation().Yaw + DirectionAngles[dir], 0.0f);
	SetActorRotation(RollRotation);
	
	PlayAnimMontage(RollAnim, 1.f); //string을 name으로 형변환 (원래 이함수가 name을 받아서) 참고로 string으로 먼저 쓴이유는 문자조작되는게 string이라서
	//LaunchCharacter(GetActorForwardVector() * 8000.f, true, true);
}


void APlayerCharacter::ResetCount()
{
	isDefence = false;
	GetCharacterMovement()->GravityScale = 1.f;
	Offinvincibility();

	Player_Skill->ResetCount();
}

void APlayerCharacter::Oninvincibility()
{
	Player_State->Oninvincibility();
}

void APlayerCharacter::Offinvincibility()
{
	Player_State->Offinvincibility();
}

void APlayerCharacter::MyTakeDamage2(float damage, HitedState hit, float hitedTime_, FVector launchVec)
{
	Player_Health->MyTakeDamage2(damage, hit,hitedTime_,launchVec);
}
void APlayerCharacter::KnockBack(float damage, float hitedTime_, FVector launchVec)
{
	MyTakeDamage2(damage, HitedState::Block, hitedTime_, launchVec);
}

void APlayerCharacter::ReSpawn_Implementation()
{
	Player_Health->ReSpawn();
}

void APlayerCharacter::HP_HEAL_Implementation(float add)
{
	Player_Health->HP_HEAL(add);
}
void APlayerCharacter::STAMINA_HEAL_Implementation(float add)
{
	Player_Health->STAMINA_HEAL(add);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, ForwardDirValue);
	DOREPLIFETIME(APlayerCharacter, RightDirValue);
	DOREPLIFETIME(APlayerCharacter, Player_Skill);
}

bool APlayerCharacter::CheckCanUseItem()
{
	return Player_State->HasState(static_cast<uint8>(EPlayerStateFlags::CanAttack));
}

void APlayerCharacter::BlockadeSkill()
{
	Player_Skill->BlockadeSkill();
}

