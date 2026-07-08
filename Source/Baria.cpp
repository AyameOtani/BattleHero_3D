#include "Baria.h"
#include "Master.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Player3D.h"
#include "Utility.h"
#include "Model.h"
#include "Effect4.h"
#include <cmath>
#include "BreakableGlass.h"
#include "TutorialManager.h"


// 先生が作ってくれた割れるバリアをいれた
Baria::Baria(std::string filename, VECTOR initPos)
	:Object3D(initPos)
	, mfAlpha(0.0f)
	, mbBaria(false)
	// ★mWireGlassは残してもいいけど、今回は使わないので false のまま
	, mWireGlass(false)
	, mnModelHandle(-1)
	, mBarrierAlive(false)
	, mnHandle(-1)
{
	SetTag(Object3D::T_Baria3D); // タグ

	//バリアゲージみたいなとこに出ている画像
	mnGaugeHandle = LoadGraph("Resource/2d/BariaGauge.png");
	if (mnGaugeHandle == -1)
	{
		printfDx("画像の読み込みに失敗しました");
	}

	//mpModel = new Model("Resource/バリア/BariaModel2.mqo", initPos);

	mnFontBig = CreateFontToHandle(
		NULL, // 今使ってるのと同じだと思ふ
		40,              // 大きさだけ違う
		3                // 太さ
	);

}

Baria::~Baria()
{
	DeleteFontToHandle(mnFontBig);
	DeleteGraph(mnGaugeHandle);

	mGlass.Finalize();
}

// Player3Dのコンストラクタで呼んでいる
bool Baria::Initialize()
{
	//============================================================
	// バリア（割れるモデル）の初期化
	// intact : 割れていない状態のモデル
	// shards : 破片用モデル（フレーム単位で破片に分かれている必要あり）
	//============================================================
	if (!mGlass.Load("Resource/baria/model_baria_intact.mqo",
		"Resource/baria/model_baria_shards.mqo"))
	{
		return false;
	}

	// ★★★★★★★★ バリアの表示位置を設定
	mGlass.SetPosition(mvPosition);

	// ★★★★★★★★ バリアの大きさ（倍率）
	mGlass.SetScale(1.0f);

	// ★★★★★★★★ 割れていない時の透明度（0=透明,255=不透明）
	mGlass.SetIntactAlpha(140);

	// ★★★★★★★★ 割れた後（破片）の透明度
	mGlass.SetShardAlpha(200);

	// ★★★★★★★★ 地面で少し跳ねる設定
	// true  : 地面との衝突を有効
	// 0.0f  : 地面の高さ
	// 0.35f : 跳ね返り係数（0～1、小さいほど弱く跳ねる）
	mGlass.SetGroundBounce(true, 0.0f, 0.35f);

	return true;

}

void Baria::Update()
{
	// チュートリアルさん 許可されてないならやらん
	if (!TutorialManager::GetInstance()->IsAction(TutorialManager::Event_Baria)) return;


	auto obj = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DByTag(Object3D::T_Player3D);
	if (obj != nullptr)
	{
		Player3D* pPlayer = dynamic_cast<Player3D*>(obj);
		if (pPlayer != nullptr)
		{
			// ちょうどよい位置になるようにした　1211
			// 追記　1217 ポジションをちゃんと変えた
			mvPosition = VAdd(pPlayer->GetPosition(), VGet(0.0f, 85.0f, -0.0f));
			//mpModel->SetPosition(mvPosition);
		}
	}

	const float dt = FIXED_DT;
	mGlass.Update(dt);

	//  割れていない時だけ追従させる
	if (!mGlass.IsBroken())
	{
		mGlass.SetPosition(mvPosition);
	}



	if (mbBaria) // バリア張っていたら
	{
		mfFadetime -= 1.0f / 60.0f; // クールタイムを減らす
		if (mfFadetime <= 0.0f)
		{
			//mbGetCool = true;       // クールタイム取得フラグ
			//mfCoolTime = COOL_TIME;      // クールタイム開始
			mfFadetime = 0.0f;

			// バリアが時間切れ 割れる
			if (!mGlass.IsBroken())
			{
				mGlass.Break(mvPosition, BARRIER_BREAK_POWER);
			}
			mbBaria = false;        // バリア終了

			Master::mpSoundManager->PlaySE(SoundManager::SE_BARIA_BREAK, 160); // 破壊

			// チュートリアルさん
			TutorialManager::GetInstance()->Event(TutorialManager::Event_BariaOff);
		}
	}

	if (mbGetCool) // クールタイム中のフラグ
	{
		mfCoolTime -= 1.0f / 60.0f;

		if (mfCoolTime <= 0.0f)
		{
			mfCoolTime = 0.0f;
			mbGetCool = false; // 再使用可能

			mGlass.Reset();
			mGlass.SetPosition(mvPosition);
		}
	}
	else
	{
		//DrawFormatString(x, y, GetColor(255, 255, 255), "FT %f", mfFadetime);
		mfCoolTime = 0.0f;
	}


	// 点滅のタイマー
	mCurrentPhoto += 10.0f;
	if (mCurrentPhoto > 50.0f)
	{
		mFlushTimer = !mFlushTimer;
		mCurrentPhoto = 0;
	}

}

void Baria::DrawBaria()
{
	// バリア中かつ割れているときは描画するようにしないとでない7
	if (!mbBaria && !mGlass.IsBroken()) return;

	SetUseLighting(FALSE); // ライトの影響をオフ

	// ・割れていない時  intactモデルを描画
	// ・割れた後        破片モデルを描画
	mGlass.Draw();

	SetUseLighting(TRUE);


}

void Baria::DrawBar()
{
	// バリアの画像を出す　ゲージは、上から描画　1213
	//// 位置とサイズ
	const int x = Utility::SCREEN_WIDTH / 2 + 300; // 画面の半分の位置
	const int y = Utility::SCREEN_HEIGHT / 2 + 380; // 下の方

	{ // バリアの画像　丸みたいにやつ
		// 元画像のサイズを取得
		int width, height;
		GetGraphSize(mnGaugeHandle, &width, &height);

		// サイズを半分にする -1は、枠も入れたいから内側にしている
		int miniW = (int)(width / 2.5f - 5);
		int miniH = (int)(height / 2.5f - 5);

		if (mbGetCool) // coolタイム中だったら画像を暗めにする
		{
			SetDrawBright(128, 128, 128); // 0 255暗くする
			DrawExtendGraph(
				x, // 位置X
				y, // 位置Y
				x + miniW, // 右下X
				y + miniH, // 右下Y
				mnGaugeHandle,
				TRUE // 投下
			);
			SetDrawBright(255, 255, 255); // 必ず戻す！
		}
		else
		{
			DrawExtendGraph(
				x, // 位置X
				y, // 位置Y
				x + miniW, // 右下X
				y + miniH, // 右下Y
				mnGaugeHandle,
				TRUE // 投下
			);
		}

	}


	//// 位置とサイズ
	const float width1 = 63; // 横幅
	const float height1 = 84; // 高さ

	// ゲージの割合
	float rate = mfCoolTime / COOL_TIME; // 0から1の割合にしている
	// 値が０から１を超えたり下回ったりしないように
	if (rate < 0.0f)
	{
		rate = 0.0f;
	}
	if (rate > 1.0f)
	{
		rate = 1.0f;
	}

	// ここHPバーの色　警告バーはなくした　1212
	if (mbGetCool)
	{
		// barWidth...今のゲージの高さ
		float barheight = (height1 * (1.0f - rate));


		int col = GetColor(180, 180, 0);
		// 位置の微修正 1219
		float BarX = x + 17;
		float BarY = y + 6;
		float Yheight = BarY + height1;


		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150); // 透明度をさげた バリアの画像を見せるため

		DrawBox(
			(int)BarX,                       //  左上のX
			(int)Yheight - (int)barheight, // 左上のY　ゲージの一番下から今ゲージの高さをひいているから増えている
			(int)BarX + (int)width1,              // 右下X    左のXから＋横幅分移動させている
			(int)Yheight,             // 右下のY ここを固定にする
			col,
			TRUE);

		// 元に戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


		float FontX = BarX + width1 / 2;
		float FontY = BarY + height1 / 2;


		if (mbGetCool)
		{
			// ここだけ文字のおおきさを変えるためいつものじゃない!!!!!!
			DrawFormatStringToHandle(
				(int)FontX - 10,
				(int)FontY - 20,
				GetColor(255, 255, 255),
				mnFontBig, // ここのやつはコンストにいる
				"%d",
				(int)mfCoolTime
			);
		}


	}

}

void Baria::Start() // スタートが呼ばれたら
{
	// player情報の取得
	auto pPlayerList =
		Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(Object3D::T_Player3D);


	// クールタイム中は使えない
	if (mbGetCool) return;
	// すでにバリア中なら無視
	if (mbBaria) return;


	if (!pPlayerList.empty())
	{
		for (auto obj : pPlayerList)
		{
			Player3D* pPlayer = dynamic_cast<Player3D*>(obj);
			if (pPlayer)
			{
				if (!pPlayer->GetGameStart() && (Master::mpSceneManager->GetCurrentSceneType() != SceneManager::SCENE_TYPE::TUTORIAL_SCENE_3D))  // もしゲームスタートがfalseならはれないように
				{
					return;
				}
			}
		}
	}

	mbBaria = true; // ここでONにしている
	mfFadetime = FADE_TIME;
	mbGetCool = true;
	mfCoolTime = COOL_TIME;


	// カウント
	TutorialManager::GetInstance()->Event(TutorialManager::Event_Baria);

	Master::mpSoundManager->PlaySE(SoundManager::SE_BARIA, 140); // バリア発動

	// バリア開始時は必ず元の状態
	mGlass.Reset();
	mGlass.SetPosition(mvPosition);
}