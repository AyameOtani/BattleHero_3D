#include "Player3D.h"
#include "Model.h"
#include "Master.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Wall.h"
#include "Effect.h"
#include "Effect2.h"
#include "Effect3.h"
#include "Enemy3D.h"
#include "Floor.h"
#include "Stage.h"
#include "Utility.h"
#include "Baria.h"
#include "Bullet3D.h"
#include "HpBar.h"
#include "BoostBar.h"
#include "WeaponManager.h"
#include "GameManager.h" // 武器選択のやつ
#include "Graph.h"
#include "TutorialManager.h" // チュートリアルのやつ
#include <algorithm>
#include "SandEffect.h"

// チュートリアル用の奴
Player3D::Player3D(std::string filename, VECTOR initPos, Status3D* status, bool isSeparateAnim)
	: Unit(initPos, status)
	, mfTimeCount(0.0f) // 浮遊時間のカウント
	, mfAngle(0.0f)
	, mfTargetAngle(0.0f)
	, mvOldPosition(initPos)
{
	SetTag(Object3D::T_Player3D); // タグ
	// 文字関係
	mnFontBig = CreateFontToHandle(
		NULL, // 今使ってるのと同じになるはず
		50,   // 大きさだけ違う
		3     // 太さらしいけど変えても意味なし
	);

	mnFontNormal = CreateFontToHandle(
		NULL,
		36,   // 今のフォントサイズ
		3
	);

	mnFontSmall = CreateFontToHandle(
		NULL,
		24,   // 小さめのフォントサイズ
		3
	);
	// モデルの作成
	mpModel = new Model(filename, initPos, isSeparateAnim);

	// 初期行列の設定
	// ここで設定する名称は、mixamoからダウンロードしたモデルによって変わる可能性が高いので、
	// ModelViewer などで確認すること（おそらく、***:Hips となっているフレームがあればそれ。）
	mpModel->SetupInitializeMatrix("mixamorig:Hips");

	// バリア作成
	mpBaria = new Baria("bariaModel.dx", GetPosition()); // DXライブラリの3Dモデルファイル 1112
	// Hpバーの作成
	mpHpBar = new HpBar(this);
	// Boostバーの作成
	mpBoostBar = new BoostBar(this);
	// 時間の作成
	mpTimer = new GameTimer();
	// モデルを作った直後にWeaponManagerを作る
	mpWeapon = new WeaponManager(mpModel->GetHandle());



	// チュートリアルは武器を固定にする
	GameManager::GetInstance()->WeaponSelect.mainWeapon = WeaponManager::WeaponID::Shield;
	GameManager::GetInstance()->WeaponSelect.subWeapon = WeaponManager::WeaponID::BlackGun;

	auto wp = GameManager::GetInstance()->WeaponSelect;
	mpWeapon->SelectLoadWeapon(wp.mainWeapon, false);
	mpWeapon->SelectLoadWeapon(wp.subWeapon, false);

	// なしじゃないなら選択したやつをセット
	if (wp.mainWeapon != WeaponManager::WeaponID::None)
	{
		mpWeapon->Set_Base_Weapon(wp.mainWeapon);
	}
	else // ない場合
	{
		//盾をロードして装備させる
		mpWeapon->SelectLoadWeapon(WeaponManager::WeaponID::Shield, false);
		mpWeapon->Set_Base_Weapon(WeaponManager::WeaponID::Shield);
	}




	mpBaria->Initialize(); // バリアの初期化を呼ぶ

	// ラグなったからこっちに移動
	SetFontSize(20); // 文字の大きさ


	mnHandle_RockOn = LoadGraph("Resource/Graph/RockOnUI.png");
	if (mnHandle_RockOn == -1)
	{
		printfDx("ロックオンの画像がない");
	}

	mnHandle_Cloud = LoadGraph("Resource/Graph/CloudUI.png");
	if (mnHandle_Cloud == -1)
	{
		printfDx("雲の画像がない");
	}


	mnDushHandle = LoadGraph("Resource/displayEffect/tex_effect_dash.png");
	if (mnDushHandle == -1)
	{
		printfDx("ダッシュの画像がない");
	}
	mnDushHandleSub = LoadGraph("Resource/displayEffect/tex_effect_dash_sub.png");
	if (mnDushHandleSub == -1)
	{
		printfDx("ダッシュサブの画像がない");
	}

	mnHandle_Black = LoadGraph("Resource/Graph/Graph3.png");
	if (mnHandle_Black == -1)
	{
		printfDx("黒い枠の画像がない");
	}
	mnHandle_mainGun = LoadGraph("Resource/Graph/Gun1.png");
	if (mnHandle_mainGun == -1)
	{
		printfDx("銃１の画像がない");
	}
	mnHandle_subGun = LoadGraph("Resource/Graph/Gun2.png");
	if (mnHandle_subGun == -1)
	{
		printfDx("銃２の画像がない");
	}
	mnHandle_GunBlack = LoadGraph("Resource/Graph/Graph4.png");
	if (mnHandle_GunBlack == -1)
	{
		printfDx("銃の黒枠の画像がない");
	}

	// オーバーヒート委関係  // ノイズの画像
	mnHandle_OverHeat = LoadGraph("Resource/Graph/DamageUI2.png");
	if (mnHandle_OverHeat == -1)
	{
		printfDx("オーバーヒートUI画像がない");
	}
	mnHandle_OverHeatRed = LoadGraph("Resource/Graph/DamageUI.png");
	if (mnHandle_OverHeatRed == -1) // 赤いぼかし
	{
		printfDx("オーバーヒートUI画像がない");
	}

	// ダメージ関係
	mnHandle_Damage = LoadGraph("Resource/Graph/OHD.png");
	if (mnHandle_Damage == -1) // 赤いぼかし
	{
		printfDx("ダメージ画像がない");
	}

	// バリアのUI
	mnHandle_BariaUI = LoadGraph("Resource/Graph/BariaUI1.png");
	if (mnHandle_BariaUI == -1)
	{
		printfDx("バリアUI画像がない");
	}
}




// 普通にゲームscene用の奴
Player3D::Player3D(int modelpath, VECTOR initPos, Status3D* status, bool isSeparateAnim)
	: Unit(initPos, status)
	, mfTimeCount(0.0f) // 浮遊時間のカウント
	, mfAngle(0.0f)
	, mfTargetAngle(0.0f)
	, mvOldPosition(initPos)
{
	SetTag(Object3D::T_Player3D); // タグ
	// 文字関係
	mnFontBig = CreateFontToHandle(
		NULL, // 今使ってるのと同じになるはず
		50,   // 大きさだけ違う
		3     // 太さらしいけど変えても意味なし
	);

	mnFontNormal = CreateFontToHandle(
		NULL,
		36,   // 今のフォントサイズ
		3
	);

	mnFontSmall = CreateFontToHandle(
		NULL,
		24,   // 小さめのフォントサイズ
		3
	);
	// モデルの作成
	mpModel = new Model(modelpath, initPos, isSeparateAnim);

	// 初期行列の設定
	// ここで設定する名称は、mixamoからダウンロードしたモデルによって変わる可能性が高いので、
	// ModelViewer などで確認すること（おそらく、***:Hips となっているフレームがあればそれ。）
	mpModel->SetupInitializeMatrix("mixamorig:Hips");

	// バリア作成
	mpBaria = new Baria("bariaModel.dx", GetPosition()); // DXライブラリの3Dモデルファイル 1112
	// Hpバーの作成
	mpHpBar = new HpBar(this);
	// Boostバーの作成
	mpBoostBar = new BoostBar(this);
	// 時間の作成
	mpTimer = new GameTimer();
	// モデルを作った直後にWeaponManagerを作る
	mpWeapon = new WeaponManager(mpModel->GetHandle());
	// 武器選択
	auto weapon = GameManager::GetInstance()->WeaponSelect;
	// セットに選択したものを入れる プレイヤーが使うからfalseにしている
	mpWeapon->SelectLoadWeapon(weapon.mainWeapon, false);
	mpWeapon->SelectLoadWeapon(weapon.subWeapon, false);

	// なしじゃないなら選択したやつをセット
	if (weapon.mainWeapon != WeaponManager::WeaponID::None)
	{
		mpWeapon->Set_Base_Weapon(weapon.mainWeapon);
	}
	else // ない場合
	{
		//盾をロードして装備させる
		mpWeapon->SelectLoadWeapon(WeaponManager::WeaponID::Shield, false);
		mpWeapon->Set_Base_Weapon(WeaponManager::WeaponID::Shield);
	}

	if (Master::msPlayerModelName == ("Resource/3d/Player/Hero.mv1"))
	{
		// アタッチしているもの
		mpModel->AddAttachment("Resource/PlayerBoost.mqo", "spineB"); // アタッチ先を wp というフレームモデルを作成
	}
	mpBaria->Initialize(); // バリアの初期化を呼ぶ

	// ラグなったからこっちに移動
	SetFontSize(20); // 文字の大きさ


	mnHandle_RockOn = LoadGraph("Resource/Graph/RockOnUI.png");
	if (mnHandle_RockOn == -1)
	{
		printfDx("ロックオンの画像がない");
	}

	mnHandle_Cloud = LoadGraph("Resource/Graph/CloudUI.png");
	if (mnHandle_Cloud == -1)
	{
		printfDx("雲の画像がない");
	}


	mnDushHandle = LoadGraph("Resource/displayEffect/tex_effect_dash.png");
	if (mnDushHandle == -1)
	{
		printfDx("ダッシュの画像がない");
	}
	mnDushHandleSub = LoadGraph("Resource/displayEffect/tex_effect_dash_sub.png");
	if (mnDushHandleSub == -1)
	{
		printfDx("ダッシュサブの画像がない");
	}

	mnHandle_Black = LoadGraph("Resource/Graph/Graph3.png");
	if (mnHandle_Black == -1)
	{
		printfDx("黒い枠の画像がない");
	}
	mnHandle_mainGun = LoadGraph("Resource/Graph/Gun1.png");
	if (mnHandle_mainGun == -1)
	{
		printfDx("銃１の画像がない");
	}
	mnHandle_subGun = LoadGraph("Resource/Graph/Gun2.png");
	if (mnHandle_subGun == -1)
	{
		printfDx("銃２の画像がない");
	}
	mnHandle_GunBlack = LoadGraph("Resource/Graph/Graph4.png");
	if (mnHandle_GunBlack == -1)
	{
		printfDx("銃の黒枠の画像がない");
	}

	// オーバーヒート委関係
	mnHandle_OverHeat = LoadGraph("Resource/Graph/DamageUI2.png");
	if (mnHandle_OverHeat == -1)
	{
		printfDx("オーバーヒートUI画像がない");
	}
	mnHandle_OverHeatRed = LoadGraph("Resource/Graph/OHD.png");
	if (mnHandle_OverHeatRed == -1)
	{
		printfDx("オーバーヒートUI画像がない");
	}

	// ダメージ関係
	mnHandle_Damage = LoadGraph("Resource/Graph/DamageUI.png");
	if (mnHandle_Damage == -1) // 赤いぼかし
	{
		printfDx("ダメージ画像がない");
	}

	// バリアのUI
	mnHandle_BariaUI = LoadGraph("Resource/Graph/BariaUI1.png");
	if (mnHandle_BariaUI == -1)
	{
		printfDx("バリアUI画像がない");
	}

	// これでなんか半透明になるけど使わん
	// MV1SetDifColorScale(modelpath, GetColorF(1.0f, 1.0f, 1.0f, 0.8f));
}

// Scene3Dで呼びだしている
void Player3D::Initialize()
{
	// プレイヤーの証明画像のやつ
	mnHandle_PlayerPhoto = Graph::PlayerPhoto(Master::msPlayerModelName);

	// 軌跡の初期化
	int TrailColor = GetColor(255, 255, 0);
	g_Trail.Initialize(0.35f, TrailColor); // 寿命と色
	// フレームの位置に軌跡を出す
	VECTOR BottomPos = mpWeapon->GetFrameWorldPosition("剣下");
	VECTOR TopPos = mpWeapon->GetFrameWorldPosition("銃口1");
	g_Trail.Reset(BottomPos, TopPos);

}

void Player3D::CloudUI()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)mfCloudAlpha);
	DrawRotaGraph(
		Utility::SCREEN_WIDTH / 2,
		Utility::SCREEN_HEIGHT / 2,
		1.0f,
		0.0f,
		mnHandle_Cloud,
		TRUE
	);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

// RockOnのUI
void Player3D::RockOnUI()
{
	if (Master::mpCamera->IsRockOn() || mfRockOnSize <= 1.5f)
	{
		DrawRotaGraph(
			Utility::SCREEN_WIDTH / 2,
			Utility::SCREEN_HEIGHT / 2,
			mfRockOnSize,
			0.0f,
			mnHandle_RockOn,
			TRUE
		);
	}
}

// ロックオンの画像の拡大率
void Player3D::UpdateRockOnUI()
{
	if (Master::mpCamera->IsRockOn())
	{
		if (!mbRockSE)
		{
			// ロック音
			Master::mpSoundManager->PlaySE(SoundManager::SE_ROCK, 210);
			mbRockSE = true;
		}

		mfRockOnSize -= 0.03f;
		if (mfRockOnSize <= 1.0f)
		{
			// チュートリアルさん ロックオンされたかの奴
			TutorialManager::GetInstance()->Event(TutorialManager::Event_RockOnGraph);

			mfRockOnSize = 1.0f;
			mbRockFlag = true; // されたらtrue
		}
	}
	else
	{

		if (mbRockSE)
		{
			// ロック音
			Master::mpSoundManager->PlaySE(SoundManager::SE_ROCK, 210);
			mbRockSE = false;
		}

		mfRockOnSize += 0.02f;
		if (mfRockOnSize >= 1.5f)
		{
			if (mbRockFlag) //１回ロックされたらはいる
			{
				// チュートリアルさん  ロックオン解除されたかの奴
				TutorialManager::GetInstance()->Event(TutorialManager::Event_RockOffGraph);
			}

			mfRockOnSize = 1.5f;
		}
	}
}



// OH UI
void Player3D::OverHeatUI()
{
	if (mfOverHeatAlpha > 0.0f)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)mfOverHeatAlpha);
		DrawGraph(0, 0, mnHandle_OverHeatRed, TRUE); // 赤いぼかし
		DrawRotaGraph( // ここがノイズ画像
			(Utility::SCREEN_WIDTH / 2) + (int)sin(GetNowCount() * 0.08f) * 10,
			(Utility::SCREEN_HEIGHT / 2) + (int)sin(GetNowCount() * 0.1f) * 5,
			1.1f,
			0.0f,
			mnHandle_OverHeat,
			TRUE
		);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		if (mbHeatSE)
		{
			Master::mpSoundManager->PlaySE(SoundManager::SE_OH, 130);
			mbHeatSE = false;
		}
	}
}

void Player3D::UpdateOverHeatUI()
{
	if (mbOverHeat)
	{
		// フェードイン
		mfOverHeatAlpha += 30.0f;   // 速度調整
		if (mfOverHeatAlpha > 220.0f)
			mfOverHeatAlpha = 220.0f;
	}
	else
	{
		// フェードアウト
		mfOverHeatAlpha -= 30.0f;
		if (mfOverHeatAlpha < 0.0f)
			mfOverHeatAlpha = 0.0f;
	}
}

// ダメージUI
void Player3D::UpdateDamage()
{
	if (mbPlayerDamage)
	{
		mfDamageAlpha -= 10.0f;
		if (mfDamageAlpha < 0.0f)
		{
			mfDamageAlpha = 0.0f;
		}
	}
}


void Player3D::DamageUI()
{
	if (mfDamageAlpha > 0.0f)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)mfDamageAlpha);
		DrawGraph(0, 0, mnHandle_Damage, TRUE); // 赤いぼかし
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}
// 透過をセット
void Player3D::SetAlpha()
{
	mfDamageAlpha = 255.0f; // 最大値にする
}

// バリアの描画を呼んでいる
void Player3D::BariaDraw()
{
	mpBaria->DrawBaria();
}

// アニメーション追加
void Player3D::AddAnimation(AnimationState state, std::string filename)
{
	mpModel->AddAnimation(state, filename);
}

void Player3D::P_TimerInitialize()
{
	mpTimer->Initialize(); // タイマーの初期化を呼びだす
}

Player3D::~Player3D()
{


	// モデルの削除
	delete mpModel;
	//delete mpBaria; // ここで消したら、二回消しちゃっている
	// Playerが消えたらここが通る->オブマネがここを通る->二十でやってクラッシュする
	delete mpHpBar;
	delete mpBoostBar;
	delete mpWeapon; // 追加
	delete mpTimer;
	delete mpGraph;

	DeleteGraph(mnDushHandle);
	DeleteGraph(mnHandle_RockOn);
	DeleteGraph(mnDushHandleSub);
	DeleteGraph(mnHandle_Black);
	DeleteGraph(mnHandle_mainGun);
	DeleteGraph(mnHandle_subGun);
	DeleteGraph(mnHandle_GunBlack);
	DeleteGraph(mnHandle_OverHeat);
	DeleteGraph(mnHandle_OverHeatRed);
	DeleteGraph(mnHandle_Damage);
	DeleteGraph(mnHandle_BariaUI);


	DeleteFontToHandle(mnFontBig);
	DeleteFontToHandle(mnFontNormal);
	DeleteFontToHandle(mnFontSmall);
}

void Player3D::WeaponSet()
{
	// 現在のアニメーション情報を取得
	AnimationState now = mpModel->GetNowState();
	// はしりモーションまたは待機だったら替えられるようにする
	if (now == ANIMATION_NEUTRAL || now == ANIMATION_RUN)
	{
		auto wp = GameManager::GetInstance();
		// リロード中かを取得
		bool MainRel = mpWeapon->GetReload(wp->WeaponSelect.mainWeapon);
		bool SubRel = mpWeapon->GetReload(wp->WeaponSelect.subWeapon);

		if (InputManager::CheckDownKey(KEY_INPUT_E) || (InputManager::CheckDownPad(XINPUT_BUTTON_RIGHT_SHOULDER)))
		{
			Master::mpSoundManager->PlaySE(SoundManager::SE_WEAPON, 220); // 武器変えたときの

			// 今持っている武器を取得
			auto nowWeapon = mpWeapon->Get_Base_WeaponID();

			// 今がメインならサブに
			if (nowWeapon == wp->WeaponSelect.mainWeapon)
			{
				if (wp->WeaponSelect.subWeapon != WeaponManager::WeaponID::None)
				{
					mpWeapon->Set_Base_Weapon(wp->WeaponSelect.subWeapon);
					WeaponEffect();

					// チュートリアルさん
					TutorialManager::GetInstance()->Event(TutorialManager::Event_Weapon);
				}
			}
			// 今がサブならメインに
			else
			{
				if (wp->WeaponSelect.mainWeapon != WeaponManager::WeaponID::None)
				{
					mpWeapon->Set_Base_Weapon(wp->WeaponSelect.mainWeapon);
					WeaponEffect();

					// チュートリアルさん
					TutorialManager::GetInstance()->Event(TutorialManager::Event_Weapon);
				}
			}
		}
	}
}

void Player3D::Update()
{
	mvOldPosition = mvPosition;

	// 現在のアニメーション情報を取得
	AnimationState now = mpModel->GetNowState();
	// 攻撃モーション中の速度制御
	if (now == ANIMATION_ATTACK)
	{
		// アニメーションの現在の時間と合計時間を取得
		float now = mpModel->GetAnimTime();
		float total = mpModel->GetTotalTiime();

		float startTime = total * (6.0f / 10.0f);

		if (now >= startTime)
		{
			// 終わり速度めちゃはやく
			mpModel->SetAnimSpeed(0.80f); // 速さを変える
		}
		else
		{
			mpModel->SetAnimSpeed(0.58f); // 速さを変える
		}
	}


	// ジャンプOUT
	if (now == ANIMATION_JUMP_OUT)
	{
		// アニメーションの現在の時間と合計時間を取得
		float now = mpModel->GetAnimTime();
		float total = mpModel->GetTotalTiime();

		float startTime = total * (4.0f / 10.0f);

		if (now >= startTime)
		{
			// 終わり速度めちゃはやく
			mpModel->SetAnimSpeed(0.88f); // 速さを変える
		}
		else
		{
			mpModel->SetAnimSpeed(0.60f); // 速さを変える
		}
	}




	if (IsOverHeat())
	{
		mbDushFly = false;
	}

	// 敵のオブジェクトを探している
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);
	if (pEnemyList.empty())
	{
		SetTime(1000); // 無敵時間をセット
	}

	// 移動処理
	MoveEx();

	// プレイヤーと敵の当たり判定
	PlayerToEnemyHit();

	// 回転処理の呼び出し
	RotationByMove();


	// 攻撃モーションでなければ
	if (mpModel->GetNowState() != ANIMATION_ATTACK)
	{
		// チュートリアルさん 攻撃アニメかをオフ
		TutorialManager::GetInstance()->Event(TutorialManager::Event_AttackOff);
	}

	// チュートリアルさん
	// ジャンプ処理が許可されているか
	if (TutorialManager::GetInstance()->IsAction(TutorialManager::Event_Jump))
	{
		Jump();	// ジャンプ処理の呼び出し
	}

	// 攻撃処理 が許可されているか
	if (TutorialManager::GetInstance()->IsAction(TutorialManager::Event_Attack))
	{
		Attack(); // 攻撃処理の呼び出し
	}



	// 銃攻撃 が許可されているか
	if (TutorialManager::GetInstance()->IsAction(TutorialManager::Event_Weapon))
	{
		BulletAttack(); // 攻撃銃の呼び出し
	}

	// バリアが許可されているか
	if (TutorialManager::GetInstance()->IsAction(TutorialManager::Event_Baria))
	{
		// バリア
		PlayerBariaAndCheck();
	}

	// 武器切り替え が許可されているか
	if (TutorialManager::GetInstance()->IsAction(TutorialManager::Event_Weapon))
	{
		WeaponSet(); // 武器の切り替えの呼び出し
	}


	// 今の武器がギターかどうか
	if (mpWeapon->Get_Base_WeaponID() == 6)
	{
		mbMusicWeapon = true;
	}
	else
	{
		mbMusicWeapon = false;
	}


	// 当たり判定の呼び出し
	FloorHit3D();
	WallHit3D();

	// デバック画面にするかの呼び出し
	DebugMode();


	// OHの透過の呼び出し
	UpdateOverHeatUI();

	// RONの画像のやつ
	UpdateRockOnUI();

	// 点滅のタイマー
	mCurrentPhoto += 10.0f;
	if (mCurrentPhoto > 200.0f)
	{
		mFlushTimer = !mFlushTimer;
		mCurrentPhoto = 0;
	}

	// 無敵時間中の点滅処理
	// 無敵かつまだゲームが終わってないなら
	if (GetTime() > 0.0f && GetTime() < 500.0f
		&& Master::mpSceneManager->GetCurrentSceneType()
		!= SceneManager::SCENE_TYPE::TUTORIAL_SCENE_3D)
	{
		mbPlayerDamage = true; // ダメージを受けたかのフラグ
		// 5フレームに一度 描画をスキップ
		if ((int)(GetTime() / 5) % 2 == 0)
		{
			isVisible = false;
		}
		else
		{
			isVisible = true;
		}
	}
	else if (GetTime() == 0.0f)
	{
		isVisible = true;
		// ダメージカウントの呼び出し
		UpdateDamage();
		// ダメージのUIをやる
		if (mfDamageAlpha <= 0.0f)
		{
			mbPlayerDamage = false; // ダメージUIをOFF
		}
	}

	// フレームの位置に軌跡を出す
	// なくても武器の位置から出るからおｋ
	VECTOR BottomPos = mpWeapon->GetFrameWorldPosition("剣下");
	VECTOR TopPos = mpWeapon->GetFrameWorldPosition("銃口1");
	// 軌跡の更新
	int TrailColor = GetColor(255, 255, 0);
	g_Trail.Update(BottomPos, TopPos);


	auto wp = GameManager::GetInstance();
	// リロード中かを取得
	bool MainRel = mpWeapon->GetReload(wp->WeaponSelect.mainWeapon);
	bool SubRel = mpWeapon->GetReload(wp->WeaponSelect.subWeapon);
	// どっちかがリロードちゅうだったら
	if (MainRel || SubRel)
	{
		mpWeapon->AddCount();
		if (mpWeapon->Get_Base_Bullet() >= mpWeapon->Get_Base_BulletMax())
		{
			mbReload = false;
		}
	}

	/*int pad = GetJoypadInputState(DX_INPUT_PAD1);
	printfDx("PAD:%d\n", pad);*/


	// ロックオン中だったら対象を見る
	Enemy3D* RockEnemy = nullptr;
	if (Master::mpCamera->IsRockOn())
	{
		RockEnemy = Master::mpCamera->GetTargetEnemy();

		if (RockEnemy) // ロックオン対象がいたら
		{
			VECTOR moveDir = VGet(0.0f, 0.0f, 0.0f); // 今フレームの移動方向
			VECTOR EnemyAngle = RockEnemy->GetPosition(); // 敵の位置にロックオン対象をいれる
			VECTOR toEnemy = VSub(EnemyAngle, mvPosition); // 対象からプレイヤーまでのベクトル

			float distance = VSize(toEnemy);
			VECTOR dir = VNorm(toEnemy); // 正規化

			if (distance <= 2000) // 敵との距離が2000以下なら、敵の方向を向くようにした　1125
			{
				moveDir = dir;
				// プレイヤーの方向がターゲットに向くようなY軸回転
				mfTargetAngle = atan2f(dir.x, dir.z); // TargetAngleのほうに入れる 0107
			}
		}
	}

	// 砂のエフェクト
	mfSandCount += 10.0f;
	if (mfSandCount > mfSandMax)
	{
		mbSandFlag = true; // フラグ反転
		mfSandCount = 0.0f;
	}
	else
	{
		mbSandFlag = false;
	}

	// 集中線で使うタイマー
	mfLineCount += 10.0f;
	if (mfLineCount > mfLineMax) // 調整必要
	{
		mbLineFlag = !mbLineFlag;
		mfLineCount = 0.0f;
	}


	// スタミナが０になったときのカウント
	if (mbOverHeat)
	{
		// スタミナがMAXになったら
		if (GetBoost() >= GetMaxBoost())
		{
			mbOverHeat = false;
		}
	}

	// 雲
	if (mvPosition.y >= 2800)
	{
		mfCloudAlpha += 5;
		if (mfCloudAlpha >= 255.0f) mfCloudAlpha = 255.0f;
	}
	else
	{
		mfCloudAlpha -= 5;
		if (mfCloudAlpha <= 0.0f) mfCloudAlpha = 0.0f;
	}


	// モデルの更新
	mpModel->SetPosition(mvPosition);
	mpModel->Update();

	mpHpBar->Update();
	mpBoostBar->Update();

	Unit::Update(); // これを呼ばないと、減らぬ
}


// タイマーは別で呼ぶ
void Player3D::P_Timer_Update()
{
	mpTimer->Update();
}

// タイマーのリセットを呼ぶ
void Player3D::P_Timer_Reset()
{
	// GetNowCountを取り直す
	mpTimer->Reset();
}

// 風っぽいのを出す
void Player3D::DrawPlayerAction()
{
	// 基底の速さよりはやかったら
	if (GetSpeed() < mfNowSpeed)
	{
		float maxSpeed = GetMaxSpeed();
		if (maxSpeed <= 0.0f) return; // バグって0になっても案心安心

		float wari = mfNowSpeed / maxSpeed; // 最大速度割る今速度
		if (wari > 1.0f) wari = 1.0f;
		if (wari < 0.0f) wari = 0.0f;
		float targetAlpha = wari * 130.0f; // 最大で130になる

		// なんてなめらか　差×0.06
		mfGraphAlpha += (targetAlpha - mfGraphAlpha) * 0.06f;
	}
	else
	{
		// なめらかに消えていく
		mfGraphAlpha *= 0.8f;
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)mfGraphAlpha);
	// 二枚をちらちらする
	if (mbLineFlag)
	{
		DrawGraph(0, 0, mnDushHandle, TRUE);
	}
	else
	{
		DrawGraph(0, 0, mnDushHandleSub, TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}



void Player3D::Draw()
{
	// デバック表示モードのとき
	if (GetDebugMode()) // Unitのやつ
	{
		// デバッグ表示 ★★★  drawPosの場所
		DrawFormatString(20, 220, GetColor(255, 255, 255), "X: %f, Y: %f, Z: %f ", mvPosition.x, mvPosition.y, mvPosition.z);
		DrawFormatString(20, 240, GetColor(255, 255, 255), "角度: %f  目標角度: %f", mfAngle, mfTargetAngle);
		DrawFormatString(20, 260, GetColor(255, 255, 255), "ジャンプ力: %f", mfJumpPower);
		// Unit版
		DrawFormatString(20, 360, GetColor(0, 255, 255), "プレイヤーのHP: %f    無敵: %f", GetHp(), GetTime());
		// Master::mTimeCount/ 60 をすることによって１秒になる
		//DrawFormatString(20, 280, GetColor(255, 255, 255), "浮遊:%f 秒  スタミナ:%f 秒", mfTimeCount / 60, GetBoost());
		//DrawFormatString(20, 300, GetColor(255, 255, 0), "弾数: %.0f, リロード: %.0f", mpWeapon->Get_Base_Bullet(), mpWeapon->GetNowCou());

		// 武器のIDの標示 0108 ★
		DrawFormatString(20, 400, GetColor(255, 255, 255), "所持している武器番号 %d", mpWeapon->Get_Base_WeaponID());
		DrawFormatString(20, 440, GetColor(255, 255, 255), "最大弾数 %.0f", mpWeapon->Get_Base_BulletMax());


		// 描画のどこかで
		DrawFormatString(20, 480, GetColor(255, 255, 255), "AnimTime: %.1f / Total %.1f",
			mpModel->GetAnimTime(), mpModel->GetTotalTiime());


		// デバック表示用 基本のカプセル　床用
		DrawCapsule3D(mvPosition, VAdd(mvPosition, VGet(0.0f, 80.0f, 0.0f)),
			50.0f,
			8,
			GetColor(0, 255, 0),
			GetColor(0, 255, 0), false);

		// 壁
		DrawCapsule3D(VAdd(mvPosition, VGet(0.0f, 100.0f, 0.0f)),
			VAdd(mvPosition, VGet(0.0f, 180, 0.0f)),
			35.0f,
			8,
			GetColor(0, 0, 255),
			GetColor(0, 0, 255), false);


		// 天井
		DrawCapsule3D(VAdd(mvPosition, VGet(0.0f, 220.0f, 0.0f)),
			VAdd(mvPosition, VGet(0.0f, 250.0f, 0.0f)),
			40.0f,
			8,
			GetColor(255, 0, 0),
			GetColor(255, 0, 0), false);


		// -------------------- スティック入力 --------------------
		int StickX, StickY;
		VECTOR moveVecPad = VGet(0.0f, 0.0f, 0.0f);  // スティック用
		GetJoypadAnalogInput(&StickX, &StickY, DX_INPUT_PAD1); // 左スティック
		const int stickDeadZone = 200;
		// デバッグ表示
		DrawFormatString(100, 100, GetColor(255, 255, 255), "LX:%d  LY:%d", StickX, StickY);




		// ながい武器でも当たるようにした
		// なくても武器の位置から出るからおｋ
		VECTOR muzzlePos = mpWeapon->GetFrameWorldPosition("銃口1");
		// ここで壁と武器の当たり判定のデバック
		VECTOR muzzlePos1 = mpWeapon->Get_Base_WeaponPosition();
		// 壁に当たっているかどうかで色を変える 黄だと攻撃OK　赤なら壁当たり
		unsigned int debugColor = mbIsWallWeapon ? GetColor(255, 0, 0) : GetColor(255, 255, 0);
		DrawSphere3D(muzzlePos1, 55.0f, 8, debugColor, GetColor(255, 255, 255), FALSE);
		DrawSphere3D(muzzlePos, 50.0f, 8, debugColor, GetColor(255, 255, 255), FALSE);

		// 武器を包み込むような球体
		DrawSphere3D(
			muzzlePos,
			30.0f,
			8,
			GetColor(255, 255, 255),
			GetColor(255, 255, 255),
			false
		);


		//// どのボタンが押されているか全スキャン
		//XINPUT_STATE input;
		//if (GetJoypadXInputState(DX_INPUT_PAD1, &input) == 0)
		//{
		//	for (int i = 0; i < 16; i++) // XInputのボタンは大体16個
		//	{
		//		// 押されていたら赤、押されていなかったら白で表示
		//		unsigned int color = (input.Buttons[i] != 0) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);
		//		// 縦に並べて表示
		//		DrawFormatString(800, 300 + (i * 20), color, "Button[%2d]: %d", i, input.Buttons[i]);
		//	}
		//}


		if (CheckHitKey(KEY_INPUT_R))
		{
			DrawFormatString(1000, 300, GetColor(255, 0, 0), "R押された");
		}
	}

	if (isVisible) // 点滅のときのフラぎ追加した
	{
		mpModel->Draw();
	}

	//DrawFormatString(500, 500, GetColor(255, 255, 255), "%f", mfRockOnSize);
	// 描画が終わったらブレンドモードを通常に戻す（これを忘れると全部透ける）
	//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	//mpModel->Draw();     // プレイヤーモデル
}

// 軌跡描画
void Player3D::DrawTrail3D()
{
	// アニメの時間とる
	float now = mpModel->GetAnimTime();
	float total = mpModel->GetTotalTiime();

	float startTime = total * (1.4f / 5.0f); // 全体の１４％
	float endTime = total * (3.2f / 5.0f); // 全体の６２％

	// 攻撃の特定の時間帯だけ描画する
	if (GetNearAttack() && now >= startTime && now <= endTime)
	{
		g_Trail.Draw();
	}
}


void Player3D::DrawUI()
{
	mpHpBar->PlayerHpBar(); // HP－の描画
	mpBoostBar->PlayerBoostBar(); // 追加1222
	DrawShotGauge();
	mpBaria->DrawBar();  // バリアゲージ（2D）
	SpeedBar(); // スピードバーを呼び出し
}
void Player3D::DrawTimer()
{
	mpTimer->Draw();   // 時間
}

void Player3D::FloorHit3D()
{
	// -----------------床の3Dモデルとのあたり判定 床★  ------追加 11/7----------------------------///////
	// 複数に 0121
	auto obj = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Stage3D);
	for (auto stage : obj)
	{
		Stage* pStage = dynamic_cast<Stage*>(stage);
		if (pStage != nullptr)
		{
			// ステージとプレイヤーのカプセルが当たっている場合
			if (pStage->CheckHit_Capsule(mvPosition, VAdd(mvPosition, VGet(0.0f, 80.0f, 0.0f)), 50.0f))
			{
				float LinePos = 33.0f; // プレイヤーの中心から離れている距離を設定
				// 五本の線の追加
				VECTOR LineSet[5]
				{
					VGet(0.0f, 0.0f, 0.0f), // 中央
					VGet(LinePos, 0.0f, 0.0f), // 右側
					VGet(-LinePos, 0.0f, 0.0f), // 左側
					VGet(0.0f, 0.0f, LinePos), // 前
					VGet(0.0f, 0.0f, -LinePos)  // 後
				};

				float MaxY = -FLT_MAX; // いちばん小さい数字入れている
				bool isHit = false; // 一本でも当たったかのフラグをON

				for (int i = 0; i < 5; i++)
				{
					VECTOR start = VAdd(mvPosition, VAdd(LineSet[i], VGet(0.0f, 80.0f, 0.0f)));
					VECTOR end = VAdd(mvPosition, VAdd(LineSet[i], VGet(0.0f, -40.0f, 0.0f)));

					VECTOR hitPos = pStage->CheckHit_Line(start, end);

					// デバック用
					DrawLine3D(start, end, GetColor(255, 0, 0));

					if (hitPos.x != 0.0f || hitPos.y != 0.0f || hitPos.z != 0.0f)
					{
						isHit = true;
						if (hitPos.y > MaxY)
						{
							MaxY = hitPos.y;
						}
					}
				}
				// 一本でも当たっていたらＴＲＵＥになる
				if (isHit)
				{
					mfGroundY = MaxY;
					if (!mbJump) // ジャンプしていなかったらＹを合わせる
					{
						mvPosition.y = mfGroundY;
					}
				}

				mbHitUp = false; // 天井に当たっていないフラグをＯＦＦ
			}
			// ステージとプレイヤーのカプセル頭上が当たっている場合
			if (pStage->CheckHit_Capsule(
				VAdd(mvPosition, VGet(0.0f, 220.0f, 0.0f)),
				VAdd(mvPosition, VGet(0.0f, 250.0f, 0.0f)), 35.0f))	 // 壁とかぶるから少し小さめ
			{
				float LinePos = 33.0f; // 頭の幅に合わせて少し狭め
				VECTOR LineSet[5]
				{
					VGet(0.0f, 0.0f, 0.0f),      // 中央
					VGet(LinePos, 0.0f, 0.0f),   // 右
					VGet(-LinePos, 0.0f, 0.0f),  // 左
					VGet(0.0f, 0.0f, LinePos),   // 前
					VGet(0.0f, 0.0f, -LinePos)   // 後
				};

				float MinY = FLT_MAX; // 天井なので一番低いのを探す
				bool isHitUp = false;

				for (int i = 0; i < 5; i++)
				{
					// 頭の位置から少し上に向かって線を引く
					VECTOR start = VAdd(mvPosition, VAdd(LineSet[i], VGet(0.0f, 180.0f, 0.0f)));
					VECTOR end = VAdd(mvPosition, VAdd(LineSet[i], VGet(0.0f, 260.0f, 0.0f)));

					VECTOR hitPos = pStage->CheckHit_Line(start, end);

					// デバック用
					// DrawLine3D(VSub(start ,VGet(0, 500.0f,0)), end, GetColor(255.0f, 200.0f, 100.0f));
					// 当たっている判定
					if (VSquareSize(hitPos) > 0.0f)
					{
						isHitUp = true;
						// 天井の中で一番低い場所を見つける
						if (hitPos.y < MinY)
						{
							MinY = hitPos.y;
						}
					}
				}

				if (isHitUp)
				{
					mbHitUp = true;
					// mvPosition（足元）を天井の高さから250下げた位置に固定
					mvPosition.y = MinY - 250.0f;
					// ジャンプ中なら勢いを殺す
					if (mfJumpPower > 0.0f)
					{
						mfJumpPower = -0.1f;
					}
				}
			}
			// モデルに当たっていない場合
			if ((!pStage->CheckHit_Capsule(
				VAdd(mvPosition, VGet(0.0f, 220.0f, 0.0f)),
				VAdd(mvPosition, VGet(0.0f, 250.0f, 0.0f)), 40.0f)) &&
				(!pStage->CheckHit_Capsule(mvPosition, VAdd(mvPosition, VGet(0.0f, 80.0f, 0.0f)), 50.0f)))
			{
				mbHitUp = false;

				// プレイヤーから下にＬＩＮＥやる
				VECTOR start = VAdd(mvPosition, VGet(0.0f, 60.0f, 0.0f));
				VECTOR end = VAdd(mvPosition, VGet(0.0f, -2000.0f, 0.0f));

				VECTOR hitPos = pStage->CheckHit_Line(start, end);
				// デバック
				// DrawLine3D(start, end, GetColor(0.0f, 255.0f, 180.0f));
				// 床が見つかったら更新
				mfGroundY = hitPos.y;

				// 床が段差じゃなかったら落下状態に入る
				const float GroundHight = 100;
				if (!mbJump)
				{
					if (mvPosition.y - mfGroundY > GroundHight)
					{
						mbJump = true;
						mbFall = true;
						mfJumpPower = 0.0f;
					}
				}
			}
		}
	}
}


void Player3D::WallHit3D()
{
	// 改良後の当たり判定　壁の３Dモデル 11/4
	// 本来の移動ベクトルから移動しようとしている所を引いている
	VECTOR moveVecWall = VSub(mvPosition, mvOldPosition);
	VECTOR newPos = mvPosition; // 仮の移動先を書いている
	auto wall3DList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Stage3D);

	std::vector<VECTOR> hitNormals; // ここにXZだけ保存したいので入れる

	// どの壁に当たっているか
	for (auto obj : wall3DList) // wall3DList の中のオブジェクトをひとつずつチェックする
	{
		Stage* wall = dynamic_cast<Stage*>(obj); // ダイナミックキャスト

		VECTOR hitPos, hitNormal; // 衝突した結果を受け取るための変数

		// 壁に当たっているかの判定
		if (wall->CheckHit_Capsule_Wall(VAdd(newPos, VGet(0, 90, 0)),
			VAdd(newPos, VGet(0, 180, 0)), 45.0f, hitPos, hitNormal))
		{
			// 当たっていたらXZだけ保存
			VECTOR mvXZ = VGet(hitNormal.x, 0.0f, hitNormal.z); // XとZの要素だけ取り出す
			mvXZ = VNorm(mvXZ); //正規化
			hitNormals.push_back(mvXZ); // 複数に当たっている可能性があるので末尾に追加していく

			// コンテナの外側もスライドするようにする
			VECTOR imvXZ = VScale(mvXZ, -1.0f);
			hitNormals.push_back(imvXZ);
		}
	}

	// 移動前に壁に当たっていた場合だけここに来る
	// 壁に当たっていたらスライド計算
	if (!hitNormals.empty())
	{
		// hitNormals に複数法線があるとき
		VECTOR slideVec = moveVecWall;

		for (auto osu : hitNormals) // 当たった壁ごとにループ
		{
			// めり込み方向の成分を取り除く
			float meri = VDot(slideVec, osu); // VDot　内積
			if (meri < 0.0f) // めり込んでいる方向のみ処理  壁に向かっている場合のみ
			{
				slideVec = VSub(slideVec, VScale(osu, meri)); // めりこみ成分を削る
			}
		}
		VECTOR KPos = VAdd(mvOldPosition, slideVec); // スライド後の仮の位置

		// スライド後も衝突するか
		bool slideHit = false; //初期化
		for (auto obj : wall3DList)// 配列やリストの中身を一個ずつ取り出して使うという繰り返し処理
		{
			Stage* wall = dynamic_cast<Stage*>(obj); // ダイナミックキャスト

			VECTOR hitPos, hitNormal; // 結果を受け取るための変数
			if (wall->CheckHit_Capsule_Wall(
				VAdd(KPos, VGet(0, 90, 0)),
				VAdd(KPos, VGet(0, 180, 0)), 40.0f, hitPos, hitNormal))
			{
				slideHit = true;
			}
		}

		// 衝突しなければ移動、していたら前座標で停止する
		if (!slideHit)
		{
			mvPosition.x = KPos.x;
			mvPosition.z = KPos.z;
		}
		else // ここが原因で角で止まる
		{
			VECTOR move = moveVecWall;
			VECTOR oldPos = mvOldPosition;
			float radius = 40.0f; // 半径

			// X方向
			VECTOR nextX = VAdd(oldPos, VGet(move.x, 0, 0));
			bool hitX = false;	// Xは通れるかのフラグ
			VECTOR normX = VGet(0, 0, 0); // X衝突時の法線を保存
			for (auto obj : wall3DList)
			{
				Stage* wall = dynamic_cast<Stage*>(obj);
				VECTOR hitPos, hitNormal;
				if (wall->CheckHit_Capsule_Wall(VAdd(nextX, VGet(0, 90, 0)),
					VAdd(nextX, VGet(0, 180, 0)), radius, hitPos, hitNormal))
				{
					hitX = true;	 // ああ立ったTRUE
					normX = hitNormal; // 法線を代入
					break;
				}
			}

			if (!hitX)
			{
				mvPosition.x = nextX.x;
			}
			else
			{
				// 壁の法線方向に押し出す
				mvPosition.x = oldPos.x + (normX.x * 0.15f);
			}



			// Z方向
			VECTOR basePos = mvPosition; // Xの押し戻しが反映された座標をベースにする
			VECTOR nextZ = VAdd(basePos, VGet(0, 0, move.z));
			bool hitZ = false;
			VECTOR normZ = VGet(0, 0, 0);
			for (auto obj : wall3DList)
			{
				Stage* wall = dynamic_cast<Stage*>(obj);
				VECTOR hp, hn;
				if (wall->CheckHit_Capsule_Wall(VAdd(nextZ, VGet(0, 90, 0)),
					VAdd(nextZ, VGet(0, 180, 0)), radius, hp, hn))
				{
					hitZ = true;
					normZ = hn; // 法線を拾う
					break;
				}
			}
			if (!hitZ)
			{
				mvPosition.z = nextZ.z;
			}
			else	 // こっちも押し出し
			{
				mvPosition.z = oldPos.z + (normZ.z * 0.15f);
			}
		}

		/*else // ここが原因で角で止まる
		{
			 // 斜めで移動したときにXとZが同時にあたり、VECが0,0,0になるから止まる
			 VECTOR move = moveVecWall;
			 VECTOR oldPos = mvOldPosition; // 壁に当たる前フレームの位置
			 // X方向
			 VECTOR nextX = VAdd(oldPos, VGet(move.x, 0, 0));
			 bool hitX = false; // Xは通れるかのフラグ
			 for (auto obj : wall3DList)
			 {
				  Stage* wall = dynamic_cast<Stage*>(obj);
				  VECTOR hitPos, hitNormal;
				  // X 動いたらカプセルは壁にあたるかの判定
				  if (wall->CheckHit_Capsule_Wall(
					  VAdd(nextX, VGet(0, 90, 0)),
					  VAdd(nextX, VGet(0, 160, 0)),
					  40.0f, hitPos, hitNormal))
				  {
					  hitX = true; // 当たったらON
					  break;
				  }
			 }
			 if (!hitX)
			 {
				  mvPosition.x = nextX.x;
			 }
			 else
			 {
				  mvPosition.x = oldPos.x;
			 }
			 // ---------- Z方向 ----------
			 VECTOR basePos = mvPosition; // 今のポジション Xにどう済み
			 // Xへの移動が終わった今の位置からZ方向だけ当たるか試す
			 VECTOR nextZ = VAdd(basePos, VGet(0, 0, move.z));
			 bool hitZ = false;
			 for (auto obj : wall3DList)
			 {
				  Stage* wall = dynamic_cast<Stage*>(obj);
				  VECTOR hp, hn;
				  if (wall->CheckHit_Capsule_Wall(
					  VAdd(nextZ, VGet(0, 90, 0)),
					  VAdd(nextZ, VGet(0, 160, 0)),
					  40.0f, hp, hn))
				  {
					  hitZ = true;
					  break;
				  }
			 }
			 if (!hitZ)
			 {
				  mvPosition.z = nextZ.z;
			 }
			 else
			 {
				  mvPosition.z = oldPos.z;
			 }
		} */
	}


	/*  移動ベクトルを計算
		前フレームの位置と今の位置から、「どこに移動しようとしているか」を求める

		どの壁に当たるか調べる
		壁リストをすべてチェックして、法線ベクトル（壁の向き）を取得

		スライド方向を計算
		壁の法線方向の成分を移動ベクトルから取り除く → 壁に沿った移動

		スライド後の位置候補を計算
		前の位置にスライドベクトルを足す

		衝突確認
		スライド後も壁にめり込んでいないか確認

		当たっていなければ移動、まだ当たるなら前位置で止める*/
}


// 移動処理（ステージとのあたり判定用）
void Player3D::MoveEx()
{
	AnimationState state = mpModel->GetNowState();
	if (state == ANIMATION_ATTACK || state == ANIMATION_JUMP_OUT)
	{
		return; // 特定のモーション中なら何もしない
	}


	// 移動方向をきめる -> 速度をかける
	VECTOR moveVec = VGet(0.0f, 0.0f, 0.0f); // 移動方向
	VECTOR upMoveVector = VGet(0.0f, 0.0f, 0.0f); // カメラの上方向（奥方向）ベクトル
	VECTOR leftMoveVector = VGet(0.0f, 0.0f, 0.0f); // カメラの左方向ベクトル

	// カメラの向きから移動ベクトルを求める
	{
		// 上方向への移動ベクトルは、カメラの視点方向からY成分を抜いたもの
		// 注視点からカメラ自身の座標を引いているだけ
		upMoveVector = VSub(Master::mpCamera->GetLookAtPosition(), Master::mpCamera->GetPosition());
		upMoveVector.y = 0.0f;

		// 左方向への移動ベクトルは、上方向への移動ベクトルと、ｙ軸のプラス方向へのベクトルに垂直な方向 (外積)
		// VCross: ベクトル同士の外積を計算してくれるもの ★★★
		leftMoveVector = VCross(upMoveVector, VGet(0.0f, 1.0f, 0.0f));
		leftMoveVector.y = 0.0f;

		// 移動ベクトルは移動量を加味しないので、正規化しておく（ベクトルの長さを１にすること）
		// VNorm: 正規化してくれる便利なもの
		upMoveVector = VNorm(upMoveVector);
		leftMoveVector = VNorm(leftMoveVector);
	}

	if (CheckHitKey(KEY_INPUT_A)) // 左方向
	{
		moveVec = VAdd(moveVec, leftMoveVector);
	}
	if (CheckHitKey(KEY_INPUT_D)) // 右方向
	{
		// moveVec = VSub(moveVec, leftMoveVector);
		// VScaleはVECTORに対して指定した数をかけてあげるもの
		moveVec = VAdd(moveVec, VScale(leftMoveVector, -1.0f));
	}
	if (CheckHitKey(KEY_INPUT_W)) // 奥方向
	{
		moveVec = VAdd(moveVec, upMoveVector);
	}
	if (CheckHitKey(KEY_INPUT_S)) // 手前方向
	{
		// moveVec = VSub(moveVec, upMoveVector);
		// VScaleはVECTORに対して指定した数をかけてあげるもの
		moveVec = VAdd(moveVec, VScale(upMoveVector, -1.0f));
	}

	// 上限突破したら、初期位置に戻す　11/11追加
	if (mvPosition.y >= 4500)
	{
		mvPosition.x = 0;
		mvPosition.y = 0;
		mvPosition.z = 0;
	}

	// -------------------- ゲームパッドのスティック入力 -------------------- ★★        
	int StickX, StickY; // XとYを入れる変数
	VECTOR moveVecPad = VGet(0.0f, 0.0f, 0.0f);  // スティック用

	bool DushFlag = false;

	// 傾きを-1000 から1000で返してくれる関数                                                                    -1000 上
	GetJoypadAnalogInput(&StickX, &StickY, DX_INPUT_PAD1); // 左スティック                                         |
	const int StickZone = 50; // ここで少し触れただけで動くのを制御する             -1000 左                   |0             右 1000
	//const int dushZone = 700; // ここでダッシュしたときの基準                       //---------------------------|---------------------
	// スティックの傾きが一定以上なら移動ベクトルを加算                                                            |
	if (StickX < -StickZone) // 左                                                                            1000　下
	{
		moveVecPad = VAdd(moveVecPad, leftMoveVector);
	}
	if (StickX > StickZone)  // 右
	{
		moveVecPad = VAdd(moveVecPad, VScale(leftMoveVector, -1.0f));
	}
	if (StickY < -StickZone) // 奥（上方向）
	{
		moveVecPad = VAdd(moveVecPad, upMoveVector);
	}
	if (StickY > StickZone)  // 手前（下方向）
	{
		moveVecPad = VAdd(moveVecPad, VScale(upMoveVector, -1.0f));
	}
	// -------------------- moveVec と moveVec2 の統合 --------------------
	// 両方の入力を合算してどっちの操作でも動くように
	moveVec = VAdd(moveVec, moveVecPad);

	// 両方同時入力しても速くならないように正規化
	if (VSize(moveVec) > 0.0f)  // ベクトルがゼロじゃないときだけ
	{
		moveVec = VNorm(moveVec); // ここで正規化して、1+1で２になったベクトルを１に戻している
	}


	// 移動している状態であれば
	bool isMove = (moveVec.x != 0.0f || moveVec.z != 0.0f);

	if (isMove)
	{
		// チュートリアルさん
		TutorialManager::GetInstance()->Event(TutorialManager::Event_Move);

		// 移動方向を正規化しておく
		moveVec = VNorm(moveVec);
		// 新しい回転をセット 目標の回転率を求める
		// 移動方向の二つの値をいれて回転の値を求める
		mfTargetAngle = atan2f(moveVec.x, moveVec.z);
	}


	// 地面にいて動いているとき // 追加
	if (isMove && (-5.0f <= mvPosition.y && mvPosition.y <= 10.0f) && mbSandFlag)
	{
		// 足音
		Master::mpSoundManager->PlaySE(SoundManager::SE_ASIOTO, 50);

		// 砂埃っぽいエフェクト 少し地面にめりこみしている
		new SandEffect(VAdd(GetPosition(), VGet(0.0f, -20.0f, 0.0f)), "Resource/2d/Damage.png");
	}

	// ジャンプ開始、ジャンプ中のアニメでなければ
	if (state != ANIMATION_JUMP_IN && state != ANIMATION_JUMP_LOOP)
	{
		if (isMove)
		{
			// 移動していたら走るモーション
			mpModel->ChangeAnimation(ANIMATION_RUN);
		}
		else
		{
			// 移動していないなら待機モーション
			mpModel->ChangeAnimation(ANIMATION_NEUTRAL);
		}
	}



	float speed = GetSpeed(); // スピードの初期値
	float DushSpeed = GetMaxSpeed(); // 走っている時の速さ
	float nowBoost = GetBoost(); // 今のブーストを参照している

	if (nowBoost <= 0) // スタミナが下限を下回った時に、ダッシュから戻す　11147
	{
		mbOverHeat = true;
		mbHeatSE = true;

		mfNowSpeed -= 2.0;
		if (mfNowSpeed <= speed) // 歩きの時にどんどん減速
		{
			mfNowSpeed = speed;
			mbDushFlag = false;
		}
	}
	// 左シフトが押されていたらダッシュ
	if (isMove && nowBoost > 0 && !mbOverHeat &&
		(CheckHitKey(KEY_INPUT_LSHIFT) || InputManager::CheckPressPad(XINPUT_BUTTON_LEFT_SHOULDER))) // ここでフラグがON(傾きが大きかったら)速度を変えている
	{
		mpModel->SetAnimSpeed(0.7f); // 速さを変える

		// SE関係
		{
			// まだ鳴らしていなければ
			if (!mbIsWindSoundPlayer)
			{
				mfWindSE = 0;
				// 最初に一回だけループ再生を開始
				Master::mpSoundManager->PlaySE_Loop(SoundManager::SE_WIND, (int)mfWindSE);
				Master::mpSoundManager->Change_SE_Volume(SoundManager::SE_WIND, 0);
				mbIsWindSoundPlayer = true;
			}
			// 音量上げていくー
			if (mfWindSE < mfWindSE_MAX)
			{
				mfWindSE += 5;
				if (mfWindSE > mfWindSE_MAX) mfWindSE = mfWindSE_MAX;
			}
		}


		mfNowSpeed += 1.5;
		if (mfNowSpeed >= DushSpeed) // ダッシュの時にどんどん加速
		{
			mfNowSpeed = DushSpeed;
		}
		if (!mbJump && isMove) // ジャンプ中じゃなかったら
		{
			//mfSkyGauge -= 1.0f;  // 通常減少
			SubBoost(1.0f); // 減らす処理を呼んでいる
			mbDushFlag = true; // 1114 追加
		}
	}
	else
	{
		// 押されてなかったらどんどん小さく
		if (mfWindSE > 0)
		{
			mfWindSE -= 5; // 消えるスピード
			if (mfWindSE < 0) mfWindSE = 0;
		}


		mfNowSpeed--;
		if (mfNowSpeed <= speed) // 歩きの時にどんどん減速
		{
			mfNowSpeed = speed;
			mbDushFlag = false;
		}
	}


	// SEカンケイ
	{
		// ジャンプ中じゃなくても入るように外にいる
		// 音量が残っているなら音量を更新
		if (mfWindSE > 0)
		{
			Master::mpSoundManager->Change_SE_Volume(SoundManager::SE_WIND, (int)mfWindSE);
		}

		// 音量が完全に 0 になりかつ再生中フラグが立っていたら完全に止める
		if (mfWindSE <= 0 && mbIsWindSoundPlayer)
		{
			Master::mpSoundManager->StopSE(SoundManager::SE_WIND);
			mbIsWindSoundPlayer = false;
		}
	}




	mvPosition = VAdd(mvPosition, VScale(moveVec, mfNowSpeed));



	// ３Dmodelとの当たり判定はもともとここ　1212 ジャンプ中にもやりたいので改変
	// -----------------壁の画像とのあたり判定 壁★  -------------------------------------------------------------------///////
	// 壁との当たり判定を行う
	auto walls = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Wall3D);
	if (!walls.empty())
	{
		std::vector<Wall*> hitCounter; // 当たっている壁を数える
		// 当たっている壁を調べる
		// ヒント： 現状の処理では１毎の壁しか最終的には判定されないので２枚（以上）に当たっていた処理を考える
		for (int i = 0; i < walls.size(); i++)
		{
			Wall* wall = dynamic_cast<Wall*>(walls.at(i)); // ダイナミックキャストする
			if (wall != nullptr)
			{
				std::vector<VERTEX3D> vertex = wall->GetVertex();

				bool hit = false;

				// プレイヤーを包み込むようなカプセル型の判定と、壁の三角形ポリゴンとの当たり判定を行う
				if (HitCheck_Capsule_Triangle(
					mvPosition,
					VAdd(mvPosition, VGet(0.0f, 200.0f, 0.0f)),
					80.0f,
					vertex.at(0).pos, vertex.at(1).pos, vertex.at(2).pos) ||

					HitCheck_Capsule_Triangle(
						mvPosition,
						VAdd(mvPosition, VGet(0.0f, 200.0f, 0.0f)),
						80.0f,
						vertex.at(3).pos, vertex.at(1).pos, vertex.at(2).pos)
					)
				{
					hit = true;// ★★★★
				}

				if (hit) // hitがtrueなら　　// ★★★★
				{
					hitCounter.push_back(wall); // 当たっている壁を記録する
				}
				else
				{
					mbWallHit = false;
				}
			}
		}

		// 当たっている壁の数が１つだったら壁沿いベクトルを計算  // ★★★★
		// 当たっている壁の数が２つある場合は前のポジションで固定してみる
		if (hitCounter.size() == 1)
		{

			mbWallHit = true;
			std::vector<VERTEX3D> vertex = hitCounter[0]->GetVertex();

			// 壁に沿って行くようなベクトル取得
			VECTOR slide = VGet(0.0f, 0.0f, 0.0f); // 壁沿いベクトル
			float a = VDot(VScale(moveVec, -1.0f), vertex.at(0).norm); // 移動ベクトルの反対のベクトルと、壁の法線との内積を求める
			slide = VAdd(moveVec, VScale(vertex.at(0).norm, a));       // 壁沿いベクトルを計算

			// 求めた内積と壁沿いベクトルは同じ方向
			// 一旦座標を戻してから壁に沿って歩くようにする

			// 追加11/4　壁方向に走った時にガタつくのを解消 ★
			mvPosition.x = mvOldPosition.x;
			mvPosition.z = mvOldPosition.z;
			mvPosition = VAdd(mvPosition, VScale(slide, 10.0f));

		}
		else if (hitCounter.size() >= 2)
		{
			mbWallHit = true;
			// 追加11/4　壁方向に走った時にガタつくのを解消 ★
			mvPosition.x = mvOldPosition.x;
			mvPosition.z = mvOldPosition.z;
		}
	}




	// 床との当たり判定 チャレンジ ★★★
	auto floors = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Floor3D);
	if (!floors.empty())
	{
		std::vector<Floor*> hitCounter; // 当たっている床を数える ★★★★

		// 当たっている壁を調べる
		// ヒント： 現状の処理では１毎の壁しか最終的には判定されないので２枚（以上）に当たっていた処理を考える
		for (int i = 0; i < floors.size(); i++)
		{
			Floor* floor = dynamic_cast<Floor*>(floors.at(i)); // ダイナミックキャストする
			if (floor != nullptr)
			{
				std::vector<VERTEX3D> vertex = floor->GetVertex();

				bool hit = false; // ★★★★

				// プレイヤーを包み込むようなカプセル型の判定と、壁の三角形ポリゴンとの当たり判定を行う
				if (HitCheck_Capsule_Triangle(
					VAdd(mvPosition, VGet(0.0f, 220.0f, 0.0f)),
					VAdd(mvPosition, VGet(0.0f, 250.0f, 0.0f)),
					40.0f,
					vertex.at(0).pos, vertex.at(1).pos, vertex.at(2).pos) ||

					HitCheck_Capsule_Triangle(
						VAdd(mvPosition, VGet(0.0f, 220.0f, 0.0f)),
						VAdd(mvPosition, VGet(0.0f, 250.0f, 0.0f)),
						40.0f,
						vertex.at(3).pos, vertex.at(1).pos, vertex.at(2).pos)
					)
				{
					hit = true;
					// 床に沿って行くようなベクトル取得                           人
					VECTOR slide = VGet(0.0f, 0.0f, 0.0f); // 床沿いベクトル   ========     
					float a = VDot(VScale(moveVec, -1.0f), vertex.at(0).norm); // 移動ベクトルの反対のベクトルと、壁の法線との内積を求める
					slide = VAdd(moveVec, VScale(vertex.at(0).norm, a));       // 壁沿いベクトルを計算

					// 求めた内積と壁沿いベクトルは同じ方向
					// 一旦座標を戻してから壁に沿って歩くようにする
					mvPosition = mvOldPosition;
					mvPosition = VSub(mvPosition, VScale(slide, -10.0f));
				}

				if (hit)
				{
					mbFloorHit = true;
				}
				else
				{
					mbFloorHit = false;
				}
			}
		}
	}

	if (mbFloorHit && mbWallHit) // 壁にも床にも当たっていたらとめる
	{
		mvPosition = mvOldPosition;
	}
}


// 移動による回転処理
void Player3D::RotationByMove()
{
	// 現在の回転値から目標の回転値の差分を求める
	float  subAngle = mfTargetAngle - mfAngle;

	// ある方向からある方向の差が180以上（以下）にならないはずなので
	// 差の値が180以上（以下）になっていたら矯正する
	if (subAngle < -DX_PI_F)
	{
		subAngle += DX_TWO_PI_F;
	}
	else if (subAngle > DX_PI_F)
	{
		subAngle -= DX_TWO_PI_F;
	}

	// 角度の差分を徐々に０に近づける
	if (subAngle > 0.0f)
	{
		subAngle -= ROTATE_SPEED;
		if (subAngle < 0.0f)
		{
			subAngle = 0.0f;
		}
	}
	// マイナス値だった場合
	else if (subAngle < 0.0f)
	{
		subAngle += ROTATE_SPEED;
		if (subAngle > 0.0f)
		{
			subAngle = 0.0f;
		}
	}

	// 今向いてほしい角度を算出
	mfAngle = mfTargetAngle - subAngle;
	// 回転値を設定
	// 行きたい方向＋円周率
	mvRotation.y = mfAngle + DX_PI_F;
	// モデルに伝える
	// これがないと変らない
	mpModel->SetRotation(mvRotation);
}



// ここに軌跡入れたい　あたってないときも出るように
// 攻撃処理
void Player3D::Attack()
{
	if (!mbHitUp)
	{
		// Lキーが押された瞬間のみをとる関数の呼び出し    // パッドのYも取っている
		if (((InputManager::CheckDownKey(KEY_INPUT_C)) || InputManager::CheckDownPad(XINPUT_BUTTON_Y)))
		{
			// チュートリアルさん
			TutorialManager::GetInstance()->Event(TutorialManager::Event_Attack);
			// 攻撃モーションの呼び出し
			mpModel->ChangeAnimation(ANIMATION_ATTACK, 0.55f);
			mpModel->SetLoop(false); // ループはさせない
			SetNearAttack(true); // 近接フラグをON

			if (!mbJump)
			{
				// モーション後は待機アニメーションに戻す
				mpModel->SetLoopFinishState(ANIMATION_NEUTRAL);
			}

		}

		// 現在のアニメーション情報を取得
		AnimationState now = mpModel->GetNowState();

		// 敵情報の取得（複数当たっても良いように）
		auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
			->GetObject3DListByTag(Object3D::T_Enemy3D);


		// 攻撃モーションでなければ
		if (now != ANIMATION_ATTACK)
		{
			if (!pEnemyList.empty())
			{
				// 敵のサイズを数えている
				for (int i = 0; i < pEnemyList.size(); i++)
				{
					auto pEnemy = pEnemyList[i];

					if (pEnemy != nullptr)
					{
						pEnemy->SetAttackFlag(false);
					}
				}
			}

			mbAttackSE = false; // SE OFF
			SetNearAttack(false);// 近接OFF
		}
		// 攻撃モーション中 かつ　近接がONなら
		else if (now == ANIMATION_ATTACK && GetNearAttack())
		{
			// もしSEがOFFなら鳴らす
			if (!mbAttackSE)
			{
				// SE再生
				Master::mpSoundManager->PlaySE(SoundManager::SE_ATTACK, 160); // シュってやつ
				mbAttackSE = true; // すぐtrueにしてる
			}

			if (!pEnemyList.empty())
			{
				// 敵のサイズを数えている
				for (int i = 0; i < pEnemyList.size(); i++)
				{
					bool WallWeaponHit = false;	// 武器と壁が当たっているかのフラグ
					auto pEnemy = pEnemyList[i];
					// ダイナミックキャストでも良い
					//Object3D* pObjs = dynamic_cast<Object3D*>(pObj.at(i));

					if (pEnemy != nullptr)
					{
						// ながい武器でも当たるようにした
						// なくても武器の位置から出るからおｋ
						VECTOR weaponBase = mpWeapon->Get_Base_WeaponPosition();
						VECTOR muzzlePos = mpWeapon->GetFrameWorldPosition("銃口1");
						// 当たり判定を行う
						bool isHit = HitCheck_Sphere_Capsule(
							muzzlePos,  // スフィアの中心座標
							40.0f,                             // スフィアの半径
							pEnemy->GetPosition(),               // カプセルの座標１（下側）
							VAdd(pEnemy->GetPosition(), VGet(0.0f, 150.0f, 0.0f)), // カプセルの座標２（上側）
							40.0f                              // カプセルの半径
						);

						// 敵のHPを取得
						// dynamic_cast... 動的変換
						Enemy3D* pTarget = dynamic_cast<Enemy3D*>(pEnemy);


						// 壁に武器がめり込んでいるかチェック
						auto wall3DList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Stage3D);
						for (auto obj : wall3DList)
						{
							Stage* wall = dynamic_cast<Stage*>(obj);
							VECTOR hitPos, hitNormal;
							// 手元か剣先がめり込んでたr
							if (wall->CheckHit_Capsule_Wall(weaponBase, weaponBase, 55.0f, hitPos, hitNormal)
								|| wall->CheckHit_Capsule_Wall(muzzlePos, muzzlePos, 50.0f, hitPos, hitNormal))
							{
								WallWeaponHit = true;
								break;
							}
						}
						// デバッグ表示用のメンバ変数に保存
						mbIsWallWeapon = WallWeaponHit;

						if (isHit)// isHit が true なら
						{
							// 壁に当たっていたら攻撃無効
							if (WallWeaponHit) continue;

							if (!pTarget->IsAttackFlag()) // 攻撃取得の関数を呼ぶ
							{
								pTarget->Damage(mpWeapon->Get_Base_Attack(), this); // Unitのダメージ処理呼ぶ
							}
						}
						else
						{
							pTarget->SetAttackFlag(false);
						}
					}
				}
			}
		}


	}
}





// ジャンプ処理
void Player3D::Jump()
{
	mbAttackStart = false;
	XINPUT_STATE state;
	GetJoypadXInputState(DX_INPUT_PAD1, &state);

	bool mbFlag = false;

	bool Trigger = state.LeftTrigger > 100;

	if (mbOverHeat && !mbJump) return; // オーバーヒート中だったらジャンプ出来ないようにする
	// スペースキーが押された かつ まだジャンプしていない場合
	if (InputManager::CheckDownKey(KEY_INPUT_SPACE) && !mbJump ||
		InputManager::CheckPressPad(XINPUT_BUTTON_B) && !mbJump || Trigger && !mbJump)
	{
		// チュートリアルさん
		TutorialManager::GetInstance()->Event(TutorialManager::Event_Jump);

		mbJump = true; // フラグをtrueにする
		mfJumpPower = JUMP_POWER; // ジャンプ力の初期設定
		mfTimeCount = 0.0f; // 時間カウントリセット

		// スタミナゲージ初期化（必要ならここは省略してもOK）
		// mfSkyGauge = GAUGE_POWER;

		// ジャンプ開始アニメーション
		mpModel->ChangeAnimation(ANIMATION_JUMP_IN, 0.55f);
		mpModel->SetLoop(false);
		mpModel->SetLoopFinishState(ANIMATION_MAX);

		if (Trigger)
		{
			mbFlag = false;
		}
	}

	// --- ジャンプ中の処理 ---
	if (mbJump)
	{
		mfTimeCount++;

		// --- スタミナ減少 ---
		if ((CheckHitKey(KEY_INPUT_LSHIFT) || InputManager::CheckPressPad(XINPUT_BUTTON_LEFT_SHOULDER)))
		{
			SubBoost(1.8f);  // ダッシュ時は多く減る
			mbDushFly = true;
		}
		else
		{
			SubBoost(1.0f);  // 通常減少
			mbDushFly = false;
		}


		// ここで落下したときに強制的にLOOPにしている
		if (mpModel->IsAnimationLoopFinish() || mbFall)
		{
			mpModel->ChangeAnimation(ANIMATION_JUMP_LOOP);
			mpModel->SetLoop(false);
			mpModel->SetLoopFinishState(ANIMATION_MAX);

			if (!mbFall) // 落下じゃないならブレンドせん　これやらんとINで死ぬ
			{
				mpModel->SetAnimationBlend(false);
			}
			mbFall = false;
		}


		// if (!mbFall) // こっちはふつうに入る
		{
			mvPosition = VAdd(mvPosition, VGet(0.0f, mfJumpPower, 0.0f));
			AnimationState now = mpModel->GetNowState();
			if (now != ANIMATION_ATTACK)
			{
				mfJumpPower -= 0.98f; // 重力
			}
			else
			{
				mfJumpPower -= 0.62f; // 重力を弱くした
			}

			FloorHit3D();

			if (mbFloorHit)
			{
				mbJump = false;
			}
		}


		// --- 地面到達 --- 
		if (mvPosition.y <= mfGroundY)  // ヒットポジションにするジャンプ中途徒歩で分けるO
		{
			mbJump = false;
			mbFall = false; // 当たっていない自動落下をfalseにする
			mbDushFly = false;
			mvPosition.y = mfGroundY;
			mfJumpPower = JUMP_POWER;
			mfTimeCount = 0.0f;

			// 着地アニメーション
			mpModel->ChangeAnimation(ANIMATION_JUMP_OUT);	   // 早くする
			mpModel->SetLoop(false);
			mpModel->SetLoopFinishState(ANIMATION_NEUTRAL);



			// 着地かつ土に着地したとき
			if (-5.0f <= mvPosition.y && mvPosition.y <= 10.0f)
			{
				new SandEffect(VAdd(GetPosition(), VGet(0.0f, -20.0f, 0.0f)), "Resource/2d/Damage.png");
				// 足音 砂
				Master::mpSoundManager->PlaySE(SoundManager::SE_ASIOTO, 150);
			}
			else if (mvPosition.y > 20.0f)
			{
				// 足音 建物
				Master::mpSoundManager->PlaySE(SoundManager::SE_ASIOTO2, 210);
			}
		}

		// 落下処理 天井のバグで、落ちてこなくても 強制的に落とすやつ
		if (GetBoost() <= 0.0f && mbJump && mbHitUp)
		{
			mvPosition = VAdd(mvPosition, VGet(0.0f, mfJumpPower, 0.0f));
			mfJumpPower -= 0.98f;
		}
	}

	// キーが押されたかの判定
	bool isBoosting = (((CheckHitKey(KEY_INPUT_SPACE) || InputManager::CheckPressPad(XINPUT_BUTTON_B)) || ((state.LeftTrigger > 100) && !mbFlag))
		&& mfTimeCount / 60.0f >= 0.2f && GetBoost() > 0.0f && !mbOverHeat);

	// 空中ブースト処理  オーバーヒート中じゃなかったら飛べるように
	if (isBoosting && mbJump)
	{
		// まだ鳴らしていなければ
		if (!mbIsBoostSoundPlaying)
		{
			// 最初に一回だけループ再生を開始
			Master::mpSoundManager->PlaySE_Loop(SoundManager::SE_BOOST, mnBoostVolume);
			mbIsBoostSoundPlaying = true;
		}
		// 音量上げていくー
		if (mnBoostVolume < mfBoostSE_Max)
		{
			mnBoostVolume += 15;
			if (mnBoostVolume > (int)mfBoostSE_Max) mnBoostVolume = (int)mfBoostSE_Max;
		}

		// チュートリアルさん
		TutorialManager::GetInstance()->Event(TutorialManager::Event_JumpUp);
		// エフェクト生成（キラキラなど）
		new Effect(VAdd(mpModel->GetPosition(), VGet(0.0f, 150.0f, 0.0f)), "Resource/2d/Damage2.png");
		mfJumpPower += 1.5f; // 上昇ブースト
	}
	else
	{
		if (mbJump)
		{
			//	押されてないが空中にいるなら
			if (mnBoostVolume > (int)mfBoostSE_Low && !mbOverHeat)
			{
				mnBoostVolume -= 5;
				if (mnBoostVolume < (int)mfBoostSE_Low) mnBoostVolume = (int)mfBoostSE_Low;
			}
			else if (mbOverHeat) // ジャンプ中かつOHだったら
			{
				if (mnBoostVolume > 0)
				{
					mnBoostVolume -= 10; // 消えるスピード
					if (mnBoostVolume < 0) mnBoostVolume = 0;
				}
			}
		}
		else   //いないなら
		{
			// 地面についたら 0 に向かって一気に下げる
			if (mnBoostVolume > 0)
			{
				mnBoostVolume -= 5; // 消えるスピード
				if (mnBoostVolume < 0) mnBoostVolume = 0;
			}
		}
	}


	// ジャンプ中じゃなくても入るように外にいる
	// 音量が残っているなら音量を更新
	if (mnBoostVolume > 0)
	{
		Master::mpSoundManager->Change_SE_Volume(SoundManager::SE_BOOST, mnBoostVolume);
	}


	// 音量が完全に 0 になりかつ再生中フラグが立っていたら完全に止める
	if (mnBoostVolume <= 0 && mbIsBoostSoundPlaying)
	{
		Master::mpSoundManager->StopSE(SoundManager::SE_BOOST);
		mbIsBoostSoundPlaying = false;
	}


	//// --- 落下処理（スタミナ切れなど） ---
	//else if (GetBoost() <= 0.0f && mbJump)
	//{
	//	mvPosition = VAdd(mvPosition, VGet(0.0f, mfJumpPower, 0.0f));
	//	mfJumpPower -= 0.98f;
	//}
}

// バリアのUIを表示
void Player3D::DrawBariaUI()
{
	// チュートリアルさん
	if (!TutorialManager::GetInstance()->IsAction(TutorialManager::Event_Baria)) return;

	if (PlayerBariaAndCheck())
	{
		if (mpBaria != nullptr)
		{
			// バリアから現在の時間と最大時間を取得
			float Fade = mpBaria->GetFadeTime();

			// 割合を計算
			float rate = (mpBaria->GetMaxFadeTime() > 0.0f) ? (Fade / mpBaria->GetMaxFadeTime()) : 0.0f;

			// 透過するために0,255に変換
			int alpha = (int)(rate * 255.0f);

			if (alpha < 0) alpha = 0;
			if (alpha > 255) alpha = 255;

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			DrawGraph(0, 0, mnHandle_BariaUI, TRUE); // 青いぼかし
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}
}


bool Player3D::PlayerBariaAndCheck() // バリア
{
	if (mpBaria && (InputManager::CheckDownKey(KEY_INPUT_V) || InputManager::CheckPressPad(XINPUT_BUTTON_A))) // Hが押されたら
	{
		mpBaria->Start(); // バリア発動
	}

	if (mpBaria != nullptr)
	{
		//バリアが現在発動中かどうかを取得
		return mpBaria->IsActive(); // はられているゲッターを呼びだす
	}
	else
	{
		return false;
	}
}

// 銃攻撃
void Player3D::BulletAttack()
{
	if (!mbHitUp)
	{
		mbAttackStart = false;
		XINPUT_STATE state;
		GetJoypadXInputState(DX_INPUT_PAD1, &state);

		// Lキーが押された瞬間のみをとる関数の呼び出し    // パッドのAも取っている // F
		if ((mpWeapon->Get_Base_Bullet() > 0 && ((CheckHitKey(KEY_INPUT_F)) || state.RightTrigger > 100)))
		{
			// 攻撃モーションの呼び出し
			mpModel->ChangeAnimation(ANIMATION_ATTACK, 0.78f);   // はやくしている
			// ループはさせない
			mpModel->SetLoop(false);

			if (!mbJump)
			{
				// モーション後は待機アニメーションに戻す
				mpModel->SetLoopFinishState(ANIMATION_NEUTRAL);
			}
			mbAttackStart = true;

		}

		// 現在のアニメーション情報を取得
		AnimationState now = mpModel->GetNowState();
		bool WallWeaponHit = false;	// 武器と壁が当たっているかのフラグ

		// 敵情報の取得（複数当たっても良いように）
		auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
			->GetObject3DListByTag(Object3D::T_Enemy3D);

		// 攻撃モーションでなければ
		if (now != ANIMATION_ATTACK && mbAttackStart)
		{
			if (!pEnemyList.empty())
			{
				// 敵のサイズを数えている
				for (int i = 0; i < pEnemyList.size(); i++)
				{
					auto pEnemy = pEnemyList[i];

					if (pEnemy != nullptr)
					{
						pEnemy->SetAttackFlag(false);
					}
				}
			}
		}

		// 攻撃モーション中なら
		else
		{
			// ながい武器でも当たるようにした
			// なくても武器の位置から出るからおｋ
			VECTOR weaponBase = mpWeapon->Get_Base_WeaponPosition();
			VECTOR muzzlePos = mpWeapon->GetFrameWorldPosition("銃口1");
			// 壁に武器がめり込んでいるかチェック
			auto wall3DList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Stage3D);
			for (auto obj : wall3DList)
			{
				Stage* wall = dynamic_cast<Stage*>(obj);
				VECTOR hitPos, hitNormal;
				// 手元か剣先がめり込んでたr
				if (wall->CheckHit_Capsule_Wall(weaponBase, weaponBase, 55.0f, hitPos, hitNormal)
					|| wall->CheckHit_Capsule_Wall(muzzlePos, muzzlePos, 50.0f, hitPos, hitNormal))
				{
					WallWeaponHit = true;
					break;
				}
			}
			mbIsWallWeapon = WallWeaponHit; // フラグ代入


			mfShotCounter++;
			// 壁に当たっていないかる弾が０以上かつ攻撃カウンタOKなら
			if (mbAttackStart && mfShotCounter >= SHOT_FRAME && mpWeapon->Get_Base_Bullet() > 0)
			{
				mfShotCounter = 0;
				mpWeapon->SubBullet(1); // 弾を減らす 1



				// プレイヤーの向きの取得
				VECTOR dir = VGet(sin(mfAngle), 0.0f, cos(mfAngle)); // プレイヤーの向いている方向
				//VECTOR Pos = VGet(mvPosition.x + 20.0f, mvPosition.y + 130.0f, mvPosition.z + 10.0f);


				if (Master::mpCamera->IsRockOn())
				{
					Target = Master::mpCamera->GetTargetEnemy(); // ターゲットにカメラからの敵を入れる
				}


				if (!WallWeaponHit)
				{
					TutorialManager::GetInstance()->Event(TutorialManager::Event_Bullet);

					// 黒い銃だとframe返してくれる
					// なくても武器の位置から出るからおｋ
					VECTOR muzzlePos = mpWeapon->GetFrameWorldPosition("銃口1");


					std::string bullet = "Resource/Weapon/弾/弾1.mqo";
					std::string effect = "Resource/2d/BulletEffect.png";
					if (mbMusicWeapon) // ギターなら
					{
						bullet = "Resource/Weapon/弾/Music.mqo"; // 音符ちゃん
						effect = "Resource/2d/Music.png";	   // ぴんく
						// SE再生
						Master::mpSoundManager->PlaySE(SoundManager::SE_GUITER, 140); // ギター音
					}
					else
					{
						// SE再生
						Master::mpSoundManager->PlaySE(SoundManager::SE_PAN, 190); // 銃声
					}


					new Bullet3D(
						Object3D::T_Player3D,
						this,       // 誰が打ったかの特定
						Target,
						muzzlePos,
						bullet,
						dir,
						mpWeapon->Get_Base_Attack(), // 武器に応じたダメージ
						effect
					);


				}

				mbAttackStart = false;

			}
		}
	}
}


//弾丸ゲージ　バー
void Player3D::DrawShotGauge()
{
	auto wp = GameManager::GetInstance();

	float mainNow = mpWeapon->Get_Sub_Bullet(wp->WeaponSelect.mainWeapon);
	float mainMax = mpWeapon->Get_Sub_BulletMax(wp->WeaponSelect.mainWeapon);
	float subNow = mpWeapon->Get_Sub_Bullet(wp->WeaponSelect.subWeapon);
	float subMax = mpWeapon->Get_Sub_BulletMax(wp->WeaponSelect.subWeapon);

	// リロード中かを取得
	bool MainRel = mpWeapon->GetReload(wp->WeaponSelect.mainWeapon);
	bool SubRel = mpWeapon->GetReload(wp->WeaponSelect.subWeapon);

	// 位置とか
	int mainX; // 位置X
	int subX; // 位置X


	const int y = 800; // 位置Y


	// 選択している武器のもろもろ変えてる
	bool isMain = (mpWeapon->Get_Base_WeaponID() == wp->WeaponSelect.mainWeapon);
	bool isSub = (mpWeapon->Get_Base_WeaponID() == wp->WeaponSelect.subWeapon);
	mainX = isMain ? 1600 : 1670; // ボックス位置
	subX = isMain ? 1670 : 1600;
	int Width = 170; // 幅
	int Height = 25; // 高さ
	int FontSize = mnFontSmall;
	int y2 = 950; // 位置Y

	// ゲージ　文字色
	int color1 = isMain ? GetColor(75, 174, 188) : GetColor(25, 124, 138);  // ゲージの色1
	int color2 = isMain ? GetColor(25, 124, 138) : GetColor(75, 174, 188);  // ゲージの色2
	int RelColor1 = isMain ? GetColor(203, 37, 37) : GetColor(153, 3, 3); // 赤黒1
	int RelColor2 = isMain ? GetColor(153, 3, 3) : GetColor(203, 37, 37); // 赤黒2
	int StringColor1 = isMain ? GetColor(255, 255, 255) : GetColor(130, 130, 130); // 文字色1
	int StringColor2 = isMain ? GetColor(130, 130, 130) : GetColor(255, 255, 255); // 文字色2



	// 黒い背景
	int Black;
	int Black2;
	Black = isMain ? 1750 : 1820;
	Black2 = isMain ? 1820 : 1750;
	int posY = 780;

	int GunX = mainX + 150;
	int Gun2X = subX + 150;


	auto mainSpec = WeaponManager::GetBaseSpec((WeaponManager::WeaponID)wp->WeaponSelect.mainWeapon);
	auto subSpac = WeaponManager::GetBaseSpec((WeaponManager::WeaponID)wp->WeaponSelect.subWeapon);

	// ひとつめのゲージ
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 210); // 透明度をさげた
		// 銃の黒枠 大きさOK
		DrawRotaGraph(
			Black,
			posY,
			0.5f,
			0.0f,
			mnHandle_GunBlack,
			TRUE
		);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawFormatStringToHandle(
			mainX - 5,
			y - 61,
			StringColor1,
			FontSize,
			"MAIN\n%s",
			mainSpec.name.c_str());

		float rate = mainNow / mainMax;
		if (rate < 0.0f)
		{
			rate = 0.0f;
		}
		if (rate > 1.0f)
		{
			rate = 1.0f;
		}
		float Reload = (float)mpWeapon->GetReloadCount(wp->WeaponSelect.mainWeapon) / mpWeapon->GetMaxRel();
		if (Reload < 0.0f)
		{
			Reload = 0.0f;
		}
		if (Reload > 1.0f)
		{
			Reload = 1.0f;
		}

		// 背景黒
		DrawBox(mainX, y, mainX + Width, y + Height, GetColor(80, 80, 80), TRUE);

		if (MainRel) // リロード中だったら
		{
			DrawBox(mainX, y, mainX + (int)(Width * Reload), y + Height, RelColor1, TRUE); // リロードゲージ
			// 数値  リロード
			DrawFormatStringToHandle(
				mainX + 50,
				y + 1,
				StringColor1,
				FontSize,
				"Reload");
		}
		else
		{
			DrawBox(mainX, y, mainX + (int)(Width * rate), y + Height, color1, TRUE); // 弾丸ゲージ
			// 数値  弾丸数
			DrawFormatStringToHandle(
				mainX + 50,
				y + 1,
				StringColor1,
				FontSize,
				"%.0f / %.0f",
				mainNow, mainMax
			);
		}
		// 枠線 白
		DrawBox(mainX, y, mainX + Width, y + Height, StringColor1, FALSE);


		if (!isMain)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 70); // 透明度をさげた
		}
		// 銃画像
		DrawRotaGraph(
			GunX,
			y - 20,
			0.8f,
			0.0f,
			mnHandle_mainGun,
			TRUE
		);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}


	// 二つ目のゲージ
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 210); // 透明度をさげた
		// 銃の黒枠 大きさOK
		DrawRotaGraph(
			Black2,
			posY + 150,
			0.5f,
			0.0f,
			mnHandle_GunBlack,
			TRUE
		);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawFormatStringToHandle(
			subX,
			y2 - 61,
			StringColor2,
			FontSize,
			"SUB\n%s",
			subSpac.name.c_str());


		float rate = subNow / subMax;
		if (rate < 0.0f)
		{
			rate = 0.0f;
		}
		if (rate > 1.0f)
		{
			rate = 1.0f;
		}

		float Reload = (float)mpWeapon->GetReloadCount(wp->WeaponSelect.subWeapon) / mpWeapon->GetMaxRel();;
		if (Reload < 0.0f)
		{
			Reload = 0.0f;
		}
		if (Reload > 1.0f)
		{
			Reload = 1.0f;
		}

		// 背景黒
		DrawBox(subX, y2, subX + Width, y2 + Height, GetColor(80, 80, 80), TRUE);


		if (SubRel) // リロード中だったら
		{
			DrawBox(subX, y2, subX + (int)(Width * Reload), y2 + Height, RelColor2, TRUE); // リロードゲージ
			// 数値  リロード
			DrawFormatStringToHandle(
				subX + 50,
				y2 + 1,
				StringColor2,
				FontSize,
				"Reload");
		}
		else
		{
			DrawBox(subX, y2, subX + (int)(Width * rate), y2 + Height, color2, TRUE); // 弾丸ゲージ
			// 数値  弾丸数
			DrawFormatStringToHandle(
				subX + 50,
				y2 + 1,
				StringColor2,
				FontSize,
				"%.0f / %.0f",
				subNow, subMax
			);
		}
		// 枠線 白
		DrawBox(subX, y2, subX + Width, y2 + Height, StringColor2, FALSE);

		if (!isSub)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 70); // 透明度をさげた
		}
		// 銃画像
		DrawRotaGraph(
			Gun2X,
			y2 - 20,
			0.8f,
			0.0f,
			mnHandle_subGun,
			TRUE
		);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void Player3D::SetEnemy(Enemy3D* enemy) // 1125
{
	mpEnemy = enemy; // 敵を代入
}

void Player3D::WeaponEffect()
{
	// 同じ武器に変えようとしていないなら出る
	if (!mpWeapon->GetSameWeapon())
	{
		// ここで壁と武器の当たり判定のデバック
		VECTOR muzzlePos1 = mpWeapon->Get_Base_WeaponPosition();
		// ゲームパッドのAボタンが押されている または、Kが押されている(押し続けられても処理が出来る)
		new Effect2(muzzlePos1, "Resource/2d/Weapon.png");
	}
}

void Player3D::DebugMode()
{
	if (InputManager::CheckDownKey(KEY_INPUT_F3))
	{
		// mbDebugMode = !mbDebugMode;
	}
}

// バーの描画
void Player3D::SpeedBar()
{
	// 表示する黒い画像　透過度低め
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 210); // 透明度をさげた
	// Boxの表示
	DrawRotaGraph(
		80,
		920,
		0.8f,
		0.0f,
		mnHandle_Black,
		TRUE
	);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//スケール設定描画
	// しょうめいしゃしんの後ろの四角
	DrawBox(28, 848, 133, 988, GetColor(200, 200, 200), TRUE);

	// 選択しているプレイヤーの証明写真
	DrawRotaGraph(
		80,
		917,
		0.7f,
		0.0f,
		mnHandle_PlayerPhoto,
		TRUE
	);

	const int x = 180;
	const int y = 960;
	const int width = 170;
	const int height = 25;

	// Max分の今の速さ
	float rate = mfNowSpeed / GetMaxSpeed();
	if (rate < 0.0f)
	{
		rate = 0.0f;
	}
	if (rate > 1.0f)
	{
		rate = 1.0f;
	}

	DrawBox(x, y, x + width, y + height, GetColor(40, 40, 40), TRUE); // 黒背景
	int color = GetColor(90, 132, 185); // ゲージの色

	// ゲージ
	DrawBox(x, y, x + (int)(width * rate), y + height, color, TRUE);

	// 枠線 白
	DrawBox(x, y, x + width, y + height, GetColor(255, 255, 255), FALSE);


	// 選択したプレイヤーの名前
	DrawFormatStringToHandle(
		x - 40,
		y - 90,
		GetColor(200, 200, 200),
		mnFontSmall,
		"Player: %s",
		Master::mpNextStatus->GetName().c_str() // c_strつけないと文字化けする
	);

	// 入力した名前
	DrawFormatStringToHandle(
		x - 40,
		y - 50,
		GetColor(200, 200, 200),
		mnFontSmall,
		"Name  : %s",
		GameManager::Players.name.c_str()
	);

	DrawFormatString(x + 3, y + 3, GetColor(255, 255, 255), "SPEED");
}


// プレイヤーと敵がぶつかった時の処理
void Player3D::PlayerToEnemyHit()
{
	// 敵情報の取得 当たった時の判定を敵同士でやるから
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Enemy3D);

	for (auto obj : pEnemyList)
	{
		Enemy3D* pEnemy = dynamic_cast<Enemy3D*>(obj);
		if (pEnemy == nullptr) return;

		// プレイヤーのカプセルと敵のカプセルの当たり判定
		bool isHit = HitCheck_Capsule_Capsule(
			GetPosition(),                                   // 敵下
			VAdd(GetPosition(), VGet(0.0f, 150.0f, 0.0f)),   // 敵上
			40.0f,                                           // 敵半径

			pEnemy->GetPosition(),                          // プレイヤー下
			VAdd(pEnemy->GetPosition(), VGet(0.0f, 150.0f, 0.0f)), // プレイヤー上
			40.0f                                            // プレイヤー半径
		);

		if (isHit)
		{
			// 敵から自分へのヴェクトルを求める
			VECTOR dir = VSub(pEnemy->GetPosition(), this->GetPosition());
			dir.y = 0.0f; // Yは0
			float dist = VSize(dir);
			float radiusSum = 40.0f + 40.0f; // カプセル文で判定している

			if (dist < radiusSum && dist > 0.0f)
			{
				float pushPower = radiusSum - dist; // 重なっている分だけやる
				dir = VNorm(dir);
				VECTOR enemyPos = pEnemy->GetPosition();
				enemyPos = VAdd(enemyPos, VScale(dir, pushPower));
				pEnemy->SetPosition(enemyPos);
				pEnemy->WallHit3D(); // あたり判定一応
			}
		}
	}
}