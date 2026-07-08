#pragma once
#include "Status3D.h"
#include "Master.h"
#include <string>


// グローバルをstructにしてやろうか悩んだ
// float Status3D::msSelectedHp = 10.0f; // デフォルト値

// 参照私
Status3D::Status3D(const PlayerDate& spec)
{
    // SelectSceneで選択したHpを代入して、初期HPとして扱っている
    msSelectPlayer = spec.name; // 名前も取得したいから入れた

    mfMaxHp = spec.hp; // 引数を使っている
    mfHp = mfMaxHp;

    mfMaxBoost = spec.boost;
    mfBoost = mfMaxBoost;

    // 速さの代入
    mfSpeed = spec.speed;
    mfMaxSpeed = spec.maxSpeed;
}

Status3D::~Status3D()
{

}

void Status3D::ApplyDamage(float applyDamage)
{
    mfHp -= applyDamage;
    if (mfHp <= 0.0f)
    {
        mfHp = 0.0f;
    }
}

void Status3D::ApplyHeal()
{
    mfHp = mfMaxHp;
}

void Status3D::ApplyAddBoost(float applyAddBoost)
{
    mfBoost += applyAddBoost;
    if (mfBoost >= mfMaxBoost)
    {
        mfBoost = mfMaxBoost;
    }
}

void Status3D::ApplySubBoost(float applySubBoost)
{
    mfBoost -= applySubBoost;
    if (mfBoost < 0.0f)
    {
        mfBoost = 0.0f;
    }
}

void Status3D::Limit_Base_Speed()
{
    // 最大を超えないように
    if (mfSpeed >= mfMaxSpeed)
    {
        mfSpeed = mfMaxSpeed;
    }
}


// キャラのデータを一括管理する 攻撃力とか返してくれる
// ここは、void とかの部分が WeaponManager::WeaponSpec これで、それのgetBaseSpacだから長くなっちゃった
Status3D::PlayerDate Status3D::GetPlayerSpec(int select)
{
    switch (select)
    {
    case 1:
        // 回転設定
        return { "一般人", 5105, 415, 10, 15, "バランス型\n彼はただの人間\n初期キャラクター" };

    case 2:
        return { "ゾンビ", 5632, 328, 7, 10, "タフネス型\n動きが遅いゾンビ\n体力が多い  これぞゾンビ" };

    case 3:
        return { "軍隊", 5854, 487, 12, 18, "エリート型\n走り方が全力な軍隊\n体力もスタミナも高め  一般人の上位互換" };

    case 4:
        return { "ボブ", 7671, 195, 8, 11, "タンク型\n一般人の友達\nやる気がないのでスピードが遅い" };

    case 5:
        return { "みゅん", 3684, 186, 14, 22, "スピード型\nスピードに全振りしている\nミュータントのみゅん" };

    default:
        return { "", 10, 10, 10, 10, "" };
    }
}


