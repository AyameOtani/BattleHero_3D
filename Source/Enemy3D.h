#pragma once
#include "DxLib.h"
#include "Unit.h"
#include <vector>
#include "Baria.h"
#include "HpBar.h"
#include "BoostBar.h"
#include "Trail3D.h"

// 前方宣言
class Model;
class WeaponManager;

class Enemy3D : public Unit
{
private:
	static constexpr float SHOT_FRAME = 50.0f; // 弾をうつ間隔
	static constexpr float ROTATE_SPEED = 0.2f; // 回転速度
	static constexpr float JUMP_POWER = 27.0f;  // ジャンプ力

public:
	Enemy3D(int type, std::string filename,
		VECTOR initPos, float hp, float boost, float speed, float maxSpeed);
	~Enemy3D();

	void Update() override;
	void Draw() override;
	void Move(); // 移動処理
	void Jump();    // ジャンプ処理
	void RotationByMove(); // 回転処理の関数

	// 当たり判定関係
	void FloorHit3D();
	void WallHit3D();

	// 敵同士が当たった時の処理の関数
	void EnemytoEnemyHit();

	// 敵のHPバーとかの描画 ミニマップの関係で分ける
	void DrawEnemyUI();

	void DrawDebug(); // デバックで死か表示しない文字立ち


	// 敵のタイプの追加
	enum EnemyType
	{
		Attacker, // 早い
		Shooter,    // 射撃
		Coward      // 逃げ
	};
	EnemyType mType;

private:
	float mRunDist = 1000.0f;    // 追跡距離
	float mShotDist = 2000.0f;   // 射撃距離
	float mJumpDist = 2900.0f;   // ジャンプ距離
	int mnType = 0; // タイプ決め


public:
	// -----------ゲッターセッター関係ー---
	// 攻撃クールタイムのゲッターセッター 追加 11/5
	float Get_ENeetTime() const { return mfNeetTime; }
	void Set_ENeetTime(float neet) { mfNeetTime = neet; }
	void Attack();  // 攻撃処理

	// ロックオンされているかのゲッターセッター
	bool GetRockEnemy() { return mbRockEnemy; }
	void SetRockEnemy(bool rock) { mbRockEnemy = rock; }

	// ゲッター関係
	bool IsJumping() const override { return mbJump; }// ジャンプ中かどうかのゲッター
	float GetDistance() const { return mfDistanceDebug; } // 距離の表示のため
	void DebugMode(); // playerのデバックモードを取得する関数 消すつもり

	// 角度をゲット　マップで使う
	float GetAngle() const { return mfAngle; }
	void BulletAttack(); //弾での攻撃

	// Unit から override して、上昇中でも回復しないようにした
	// プレイヤーはmbJumpだけだが、敵には沢山あるから調整必要
	bool IsFlying() const override { return mbFlyJump; } // 上昇中でも回復しないように


	// 上昇カウントのゲッター
	float GetFlyMaxCount() const { return mfFlyMaxCount; }
	float GetFlyCount() const { return mfFlyCount; }

	void FlyReset(); // 抽選をリセットするため　乱打ぬのやつ

	void Hit2D(); // 壁２Dと床２Dの当たり判定
	void DrawEnemyTrail3D(); // 軌跡の呼び出し


	// 今後必要xであればアタッチなどの関数を入れる

private:

	Model* mpModel; // モデルクラスのポインタ
	HpBar* mpHpBar; // HPばークラスのポインタ
	BoostBar* mpBoostBar; // ブーストバークラスのポインタ
	WeaponManager* mpWeapon; //武器を直接やってる

	float mfAngle = 0.0f;       // 現在の回転値
	float mfTargetAngle = 0.0f; // 目標の回転値
	VECTOR mvOldPosition = VGet(0, 0, 0); // 古いポジション
	float mfSpeed = 0.0f; // 走るスピード
	float mfMaxSpeed = 0.0f; // 最大の速さ


	// 壁フラグ 2Dの時にやっていた壁とのあたり判定のフラグ
	bool HitFlag;       // 壁と当たった時に方向を変える

	// ジャンプ関係
	bool mbJump = false;         // ジャンプ中かどうかのフラグ
	float mfJumpPower = 0.0f;   // ジャンプ力
	float mfTimeCount = 0.0f;  // 浮遊のタイマー
	bool mbJumpStart = false; // ジャンプを開始するか 0109



	// 敵の動き用のカウントとスイッチ
	int mnMoveCount = 0;
	bool mbAttackDistance = false; // プレイヤーと近いかの時のフラグ　攻撃を開始したいから
	float mfNeetTime = 0.0f; // クールタイム 攻撃が連続にならないように
	bool mbFlyJump = false; // 飛ぶ挙動にはいるかどうか
	bool mbShot = false; // 銃攻撃に入ってもよいか


	// あたり判定関係 両方に当たった時に使っている
	bool mbWallHit = false;  // 壁に当たったかの判定
	bool mbFloorHit = false; // 床に当たったかの判定
	float mfGroundY = 0.0f;
	bool mbFall = false; // 落ちるときのフラグ
	bool mbHitUp = false; // 天井に当たっているときに攻撃出来ないように

	// フラッシュ（ゲージ用）
	float mCurrentPhoto = 0.0f;
	bool mFlushTimer = false;

	// ロックオンで使う
	bool mbRockEnemy = false;

	// 弾関係
	float mfShotCounter = 0.0f; // 弾を出すカウンタ
	bool mbReload = false; // リロード中華のフラグ
	float mfAttackCount = 0.0f; // 連続で攻撃しないようにするため

	//  その他
	float mfDistanceDebug = 0.0f;// 距距離デバック用
	bool mbdebugMode = true; // デバックモードかどうか


	// 上昇したらカウントを回す
	float mfFlyCount = 0.0f; // ここが5000とかになったらもう一度入っても良いように
	bool mbOkFly = false; // 上昇しても良いかのフラグ

	float mfFlyMaxCount = 0.0f; // 最大のカウント数　ランダムにしている

	// メンバ変数
	float mJumpEffectDistance = 0.0f; // エフェクトを少し軽くしたいから出る量を決めている

	// 壁判定を分けたけど2Dので同じのを使いたいからめんばにした。
	VECTOR mvMoveVec = VGet(0, 0, 0);

	// 無敵中
	bool  isVisible = true; // 無敵時間でfalseにしてちかちか

	bool mbIsWallWeapon; // 壁に武器が当たったかどうか

	// 軌跡
	Trail3D g_Trail;

	// SEのフラグ
	bool mbBariaSE = false; // バリア
	bool mbAttackSE = false; // 攻撃フラグ
};