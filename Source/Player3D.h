#pragma once

#include "DxLib.h"
#include <string>
#include "Unit.h"
#include "GameTimer.h"
#include "Model.h"
#include "Graph.h"
#include "Trail3D.h"

// 前方宣言
class Model;
class Baria;
class Enemy3D;
class HpBar;
class BoostBar;
class WeaponManager;

class Player3D : public Unit // 継承する
{
private:
	static constexpr float SHOT_FRAME = 20.0f; // 弾をうつ間隔
	static constexpr float ROTATE_SPEED = 0.3f; // 回転速度
	static constexpr float JUMP_POWER = 25.0f;  // ジャンプ力
	//static const int MAX_SHOT = 45; // 弾を撃てる最大数　ここ変えたい
	//static constexpr float MAX_RELOAD = 300; // リロード時間の最大数 ここ変えたい


public:
	void BariaDraw();

public:

	// ちゅーとりあるで使うやつ
	Player3D(std::string filename, VECTOR initPos, Status3D* status, bool isSeparateAnim = false);

	// ロードやったからintになった
	Player3D(int modelpath, VECTOR initPos, Status3D* status, bool isSeparateAnim = false);
	~Player3D(); // デストラクタ

	void Initialize();
	void Update() override; // 更新  引き継ぐから override
	void Draw() override;   // 描画
	void MoveEx(); // 移動処理（ステージとのあたり判定用）
	void RotationByMove(); // 回転処理の関数
	void Jump();    // ジャンプ処理

	// ★New★
	// アニメーション追加
	// Modelクラスへの橋渡し関数
	void AddAnimation(AnimationState state, std::string filename);


	// 当たり判定関係
	void FloorHit3D();
	void WallHit3D();

	// ゲージ関係
	void DrawShotGauge(); // 弾丸ゲージ

	// 攻撃関係
	void Attack();  // 通常攻撃
	void BulletAttack(); //弾での攻撃
	void SetEnemy(Enemy3D* enemy); // ホーミングのため

	// シールド関係
	bool PlayerBariaAndCheck(); // バリアのための

	// ゲッター関係
	//これらはUnitのにも影響している Ovverride
	bool IsJumping() const override { return mbJump; }// ジャンプ中かどうかのゲッター
	bool IsDashing() const override { return mbDushFlag; }// ダッシュ中かどうかのゲッター
	bool IsOverHeat() const override { return mbOverHeat; } // カウントのゲッター
	bool IsDushFlying() const override { return mbDushFly; } // 飛ぶダッシュかどうかのゲッター



	// 武器を変えたときのエフェクト
	void WeaponEffect();
	// 武器の付け替え
	void WeaponSet();
	// Scene3DでUIが隠れないようにする呼びだすための関数
	void DrawUI();

	// デバックモードにするかのセッターとゲッター
	void DebugMode();
	bool GetDebugMode() const { return mbDebugMode; }
	void SetDebug(bool debug) { mbDebugMode = debug; }

	// 角度を取得
	float GetAngle() const { return mfAngle; }

	// playerのアクションに対して出す画像関数
	void DrawPlayerAction();

	// スピードのゲージ 今の速さはプレイヤーだけ知ってるからこっちに描いてる
	void SpeedBar();


	// タイマー関係
	void P_TimerInitialize(); // タイマーの初期化
	int P_GetTimer() const { return mpTimer->GetTime(); } // ここでタイマーをゲットしている
	void P_Timer_Update(); // タイマーのアップデートを呼ぶ
	void P_Timer_Reset(); // タイマーのリセットを呼ぶ
	void DrawTimer();	// タイマーの描画

	// UI系
	void UpdateOverHeatUI(); // OHのときに透過をかえるやつ
	void OverHeatUI(); // OHのときの画像
	void DamageUI(); // ダメージ受けたときに出すUI


	// ダメージを受けたかのゲッターセッター
	//bool IsDamage() const { return mbPlayerDamage; }
	//void SetPlayerDamage(bool pd) { mbPlayerDamage = pd; }
	void SetAlpha(); // 攻撃受けたときにセッターを呼ぶ
	void UpdateDamage(); // ダメージを減らすやつupdで呼ぶ

	void DrawBariaUI(); // バリアのUI画像
	void DrawTrail3D(); // 軌跡の呼び出し
	void RockOnUI();
	void UpdateRockOnUI();  // ロックオンのカウントのやつ
	void CloudUI(); // 雲UI


	void PlayerToEnemyHit(); // プレイヤーと敵の当たり判定　普通にめり込まんように


	// ゲームを初めてよいかのフラグ
	bool GetGameStart() const { return mbGameStart; }
	void SetGameStart(bool st) { mbGameStart = st; }


	// 今の武器がギターかどうか
	bool GetMusicWeapon() const { return mbMusicWeapon; }
	void SetMusicWeapon(bool mu) { mbMusicWeapon = mu; }

private:
	// ポインタ関係
	Model* mpModel;    // モデルクラスのポインタ
	Baria* mpBaria;    // バリアクラスのポインタ
	HpBar* mpHpBar;    // Hpバークラスのポインタ
	BoostBar* mpBoostBar; // Boostクラスのポインタ
	GameTimer* mpTimer;
	WeaponManager* mpWeapon; //武器を直接やってる


	float mfAngle;       // 現在の回転値
	float mfTargetAngle; // 目標の回転値
	VECTOR mvOldPosition; // 古いポジション
	float mfNowSpeed = 0.0f; // 走るスピード


	// ジャンプ関係
	bool mbJump = false;         // ジャンプ中かどうかのフラグ
	float mfJumpPower = 0.0f;    // ジャンプ力
	float mfTimeCount = 0.0f;    // 浮遊のタイマー
	bool mbDushFlag = false;     // ダッシュしているときに回復しないように
	bool mbOverHeat = 0.0f;    // ブーストが０になったときにtrueにする

	bool mbDushFly = false; // 空中でブーストしているか


	// あたり判定関係 両方に当たった時に使っている
	bool mbWallHit = false;  // 壁に当たったかの判定
	bool mbFloorHit = false; // 床に当たったかの判定
	float mfGroundY = 0.0f;
	bool mbFall = false; // 落ちるときのフラグ
	bool mbHitUp = false; // 天井に当たっているときに攻撃出来ないように


	// 弾関係
	float mfShotCounter = 0.0f; // 弾を出すカウンタ
	bool mbReload = false; // リロード中華のフラグ
	bool mbAttackStart = false;   // 攻撃開始したか


	// フラッシュ関係
	float mCurrentPhoto = 0.0f;
	bool mFlushTimer = false;

	Enemy3D* mpEnemy = nullptr; // ホーミングのため
	Object3D* Target = nullptr; // もとはEnemy3D

	bool mbDebugMode = false; // デバックモードのフラグ はじめは表示させるためtrue

	// 画像表示
	int mnDushHandle; //集中線１
	int mnDushHandleSub; // ２
	bool mbLineFlag = false; // 集中線の描画
	float mfLineCount = 0.0f; // 集中線で使うカウント
	const float mfLineMax = 40.0f;

	// UI関係
	int mnHandle_Black; // 黒い枠
	Graph* mpGraph = nullptr; // playerの見た目のグラフを見るため

	// 無敵中
	bool  isVisible = true; // 無敵時間でfalseにしてちかちか

	// 文字関係
	int mnFontBig; // 文字をおおきく表示するため
	int mnFontNormal; // いつものフォントサイズ
	int mnFontSmall; // 小さいやつ

	// 弾数ＵＩ
	int mnHandle_mainGun;
	int mnHandle_subGun;
	int mnHandle_GunBlack; // 黒い背景

	// オーバーヒートUI
	int mnHandle_OverHeat;
	int mnHandle_OverHeatRed; // 赤いぼかし画像
	float mfOverHeatAlpha = 0.0f;   // 透過度
	bool mbHeatSE = false;

	// ダメージを受けたかのフラグ
	int mnHandle_Damage;
	bool mbPlayerDamage = false; // 受けたらtrueにしてダメージがぞうを出したい
	float mfDamageAlpha = 0.0f; // ダメージの画像をゆっくり消してく
	// バリアのUI
	int mnHandle_BariaUI;
	// プレイヤーのUI
	int mnHandle_PlayerPhoto = -1;

	// 軌跡
	Trail3D g_Trail;
	bool mbIsWallWeapon = false; // 壁に武器が当たったかどうか
	// スピードUIのやつ
	float mfGraphAlpha = 0.0f;

	// ロックオンUI
	int mnHandle_RockOn;
	float mfRockOnSize = 1.5f; // ロックオン画像の大きさ
	bool mbRockFlag = false; // 一回でもロックオンされたか

	// 雲の透過
	int mnHandle_Cloud;
	float mfCloudAlpha = 0.0f;

	// 砂エフェクト
	float mfSandCount = 0.0f; // 砂を出すかのやつ
	const float mfSandMax = 200.0f; // 砂の最大
	bool mbSandFlag = false; // エフェクトを出すか

	bool mbGameStart = true; // ゲームを初めてよいかのフラグ

	// SE関係
	bool mbAttackSE = false; // 攻撃SE鳴らすやつ
	bool mbRockSE = false; // ロックオンSE

	//上昇エフェクトの奴
	float mfBoostSE_Max = 90.0f; // 音の最大
	float mfBoostSE_Low = 0.0f; // さいてい
	bool mbIsBoostSoundPlaying = false; // 再生中かどうかのフラグ
	int mnBoostVolume = 30;  // BOOST大きさSE

	// 弾を音符にする運動
	bool mbMusicWeapon = false; // ギターかどうか

	// 風エフェクトの音
	float mfWindSE_MAX = 90.0f; // 最大
	float mfWindSE = 30.0f;
	bool mbIsWindSoundPlayer = false; // 風の再生中かどうか
};
