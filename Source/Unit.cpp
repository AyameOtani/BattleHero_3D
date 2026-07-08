#include "Unit.h"
#include "Effect.h"
#include "Object3D.h"
#include "InputManager.h"
#include "DeathEffect.h"
#include "Master.h"
#include "ObjectManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameManager.h"


// プレイヤー用のコンストラクタ Sttatusのポインタを渡している
Unit::Unit(VECTOR initPos, Status3D* status)
	:Object3D(initPos)
	, mfGodTime(0.0f)
{
	mpStatus = status;// 外部で new された Status3D を渡す場合

	// Unitはステータスを借りるだけだからデストラクタでdeleteしないようにfalse
	mbOwnStatus = false;

}

// 敵用のコンストラクタ  Hpとか固定にするため
Unit::Unit(VECTOR initPos, float hp, float boost, float speed, float maxSpeed)
	:Object3D(initPos)
	, mfGodTime(0.0f)
	//, mpStatus(new Status3D(hp, boost, speed, maxSpeed))  // Unit 内で new している場合
{

	Status3D::PlayerDate spec;

	spec.name = "Enemy";
	spec.hp = hp;
	spec.boost = boost;
	spec.speed = speed;
	spec.maxSpeed = maxSpeed;
	spec.memo = "";

	mpStatus = new Status3D(spec);


	// 敵が消えたらステータスも消えていいからtrueにしている
	mbOwnStatus = true;

}


Unit::~Unit()
{
	// 選択画面側で delete してはいけない
	//delete mpStatus;


	// 敵のだけdeleteしている
	// プレイヤーは unique_ptr が自動で delete する
	if (mbOwnStatus)
	{
		delete mpStatus;   // 自分でnewした敵だけdelete
	}

}

void Unit::Update()
{
	// 無敵時間が残っていればカウントダウン
	if (mfGodTime > 0.0f)
	{
		mfGodTime--;
	}

	// デバックモードの呼び出し
	//DebugMode();

	TimeEffect();
}

void Unit::Draw()
{

}


void Unit::OverHeat()
{
	// 時間あったら作る
}

void Unit::Heal() // デバックですぐ死ぬから追加
{
	mpStatus->ApplyHeal();
}


// HPを減らしてエフェクトや死亡処理までまとめる
void Unit::Damage(float damage, Unit* attacker)
{
	if (GetTime() > 0.0f)
	{
		return; // 無敵中なら処理しない
	}

	SetAttackFlag(true);    // 攻撃フラグON

	// HP減少を Status3D Damage(float damage)  ここのdamageをいれてる
	mpStatus->ApplyDamage(damage);


	// ０以下になったら
	if (mpStatus->Get_Base_Hp() <= 0)
	{
		Master::mpSoundManager->PlaySE(SoundManager::SE_DEATH, 220); // 死亡爆発SE

		// 自分が敵タグを持っていてかつ攻撃者がいる場合
		if (GetTag() == Object3D::T_Enemy3D && attacker != nullptr)
		{
			if (attacker->GetTag() == Object3D::T_Player3D)
			{
				GameManager::mnKillEnemyCount++;
			}

			auto pEnemyList = Master::mpSceneManager->GetCurrentScene()
				->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);
			int aliveCount = 0;
			for (auto obj : pEnemyList)
			{
				// 自分以外かつまだ削除フラグが立っていない敵をカウント
				if (obj != this && obj->IsDeleteFlag() == false)
				{
					aliveCount++;
				}
			}
			// 生き残りが自分だけだったならトドメを刺した相手を無敵にする
			if (aliveCount == 0)
			{
				attacker->SetTime(1000.0f); // 勝利の無敵
			}
		}
		// 自分がプレイヤーだった場合は攻撃者を無敵にする
		else if (GetTag() == Object3D::T_Player3D && attacker != nullptr)
		{
			attacker->SetTime(1000.0f); // 敗北の無敵
		}


		// 画面を揺らす
		// 引数：(揺らす時間, 揺れの強さ 揺れの速さ ステップ時間)
		Master::mpCamera->SetupShake(100.0f, 40.0f, 0.3f, 1.0f);
		// 死亡時エフェクト
		new DeathEffect(VAdd(GetPosition(), VGet(0.0f, 120.0f, 0.0f)), "Resource/2d/Death1.png");

		// 自分のタグがプレイヤーだったらカメラは敵をONする
		if (GetTag() == Object3D::T_Player3D)
		{
			// attacker が Damage 関数の引数に渡されている前提
			if (attacker)
			{
				Master::mpCamera->ActivateKillerCamera(attacker);
			}
		}

		SetDeleteFlag(true); // 消えておK
		mblive = true; // 生きているやつのフラグをONにする
	}
	else
	{
		if (GetTag() == Object3D::T_Player3D)
		{
			Master::mpSoundManager->PlaySE(SoundManager::SE_PUNCH, 230); // PlayerダメージSE
		}
		else if (GetTag() == Object3D::T_Enemy3D)
		{
			Master::mpSoundManager->PlaySE(SoundManager::SE_PUNCH, 170); // EnemyダメージSE
		}


		// 画面を揺らす
		// 引数：(揺らす時間, 揺れの強さ 揺れの速さ ステップ時間)
		Master::mpCamera->SetupShake(20.0f, 15.0f, 0.5f, 1.0f);
		// ダメージ時エフェクト
		new Effect(VAdd(GetPosition(), VGet(0.0f, 120.0f, 0.0f)), "Resource/2d/baria.png");
		// 無敵時間セット 60で一秒ぐらい
		SetTime(70.0f);
	}
}


// Booatを増やす処理
void Unit::AddBoost(float add)
{
	mpStatus->ApplyAddBoost(add);
}


// Boostを減らす処理
void Unit::SubBoost(float sub)
{
	if (IsOverHeat())
	{
		return;
	}
	mpStatus->ApplySubBoost(sub);
}

// スタミナが上限を超えないように
void Unit::LimitSpeed()
{
	mpStatus->Limit_Base_Speed();
}


//武器関係
void Unit::LoadWeapon()
{

}

//一応プレイヤーでSHILDとかで呼べるようにしたい
void Unit::SetWeapon(WeaponManager id)
{

}



void Unit::TimeEffect()
{
	// 無敵時間なら表示
	if (GetTime() > 0.0f)
	{
		//DrawFormatString(600, 600, GetColor(255, 255, 255), "無敵");

	} // 個別のCPPで解放がよいかも それか呼び出し
}