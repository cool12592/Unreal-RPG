#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Player_State.h"
#include "SkillData.generated.h"


// 데이터 테이블에 사용할 스킬 데이터 구조체
USTRUCT(BlueprintType)
struct FSkillData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float StaminaCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float Cooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    bool IsOnGround;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float StunTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "EPlayerStateFlags"), Category = "Skill")
    uint8 RequiredState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "EPlayerStateFlags"), Category = "Skill")
    uint8 ClearedState;

    // 기본값 설정
    FSkillData()
        :
        StaminaCost(0.f),
        Cooldown(0.f),
        Damage(0.f),
        IsOnGround(true),
        StunTime(0.f),
        RequiredState(static_cast<uint8>(EPlayerStateFlags::None)),
        ClearedState(static_cast<uint8>(EPlayerStateFlags::None))
    {}
};
