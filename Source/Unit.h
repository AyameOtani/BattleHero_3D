#pragma once
#include <string>
#include "Object3D.h"
#include "Status3D.h"
#include "WeaponManager.h"


class Object3D;
class Unit : public Object3D
{
public:
	Unit(VECTOR initPos, Status3D* status); // プレイヤー用
	Unit(VECTOR initPos, float hp, float boost, float speed, float maxSpeed); // 敵耀　（固定 今は1226）★
	virtual ~Unit();

	virtual void Update();
	virtual void Draw();
	void Heal(); // 回復（デバックの時だけ）

	// ステータス関係
	//----------- Hp関係---------------------
	// 攻撃されたときの処理をまとめたもの 1120
	virtual void Damage(float damage, Unit* attacker = nullptr);
	// HPのゲッター
	float GetHp() { return mpStatus->Get_Base_Hp(); }
	// MaxHpのゲッターセッター
	float GetMaxHp() { return mpStatus->Get_Base_MaxHp(); } // StatusのMaxHpを返す関数

	//----------- Boooooost関係---------------------
	// MaxBoostのゲッター
	float GetBoost() { return mpStatus->Get_Base_Boost(); }
	// Boostのゲッターセッター
	float GetMaxBoost() { return mpStatus->Get_Base_MaxBoost(); }


	//-----------スピード関係-----------------------
	float GetSpeed() { return mpStatus->Get_Base_Speed(); }
	float GetMaxSpeed() { return mpStatus->Get_Base_MaxSpeed(); }
	void LimitSpeed(); // 速さの上限をステータスから呼びだす

	// 無敵時間のときに出したい
	void TimeEffect();


	// 武器関係
	// 武器の登録
	void LoadWeapon();
	void SetWeapon(WeaponManager id);

	//VECTOR GetWeaponPosition();

	void OverHeat();


	// ダッシュの時に減らすブースト 英語わからんからAdd
	virtual void AddBoost(float add);
	// 徒歩の時に増やすブースト 英語わからんからSub
	virtual void SubBoost(float sub);


	// 無敵時間のゲッターセッター
	float GetTime() { return mfGodTime; }
	void SetTime(float time) { mfGodTime = time; }

	// 生きているかのゲッターセッター
	bool GetLive() { return mblive; }
	void SetLive(bool li) { mblive = li; }

	// 派生で上書き可能なげったーを作る 初めはfalse
	// プレイヤーで上書きわれるので注意 1222
	virtual bool IsJumping() const { return false; }
	virtual bool IsDashing() const { return false; }
	virtual bool IsFlying() const { return false; }
	virtual bool IsDushFlying() const { return false; }

	// スタミナが０になったあとのタイマー
	virtual bool IsOverHeat() const { return false; }

private:
	// 無敵時間関係
	float mfGodTime; // 無敵タイム
	bool mblive; // 生きているフラグ

private:
	Status3D* mpStatus = nullptr;
	bool mbOwnStatus = false;  //  自分で new したかどうかのフラグ

	WeaponManager* mpWeapon = nullptr; // 0129 追加

	//bool mbDebugMode = true; // デバックモードのフラグ はじめは表示させるためtrue


private:
	// 攻撃と弾攻撃によって分ける
	// 今までアニメーションでしか見てなかったからフラグで分ける
	bool mbNearAttack = false; // 近接か

public:
	//上のゲッターセッター
	void SetNearAttack(bool NA) { mbNearAttack = NA; } // 近接攻撃をセット
	bool GetNearAttack() const { return mbNearAttack; } // 近接攻撃かゲット
};