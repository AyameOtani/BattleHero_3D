#include "RankingScene3D.h"
#include "Utility.h" // 呼び出すと、SCREEN_WIDTHとかを使える
#include "DxLib.h"
#include "Master.h"
#include "inputManager.h"
#include "Player3D.h"


RankingScene3D::RankingScene3D()
	: Scene()     // 基底クラスのコンストラクタを呼び出しておく
{
	// おおきい文字の設定
	mnFontBig = CreateFontToHandle(
		NULL,
		45,
		3
	);

	// 普通の文字の設定
	mnFontNormal = CreateFontToHandle(
		NULL,
		42,
		3
	);

	// ちいさき文字の設定
	mnFontSmall = CreateFontToHandle(
		NULL,
		35,
		3
	);

	// 画像読み込み場所
	mnHandle_Bag = LoadGraph("Resource/Scene/sky.png");
	if (mnHandle_Bag == -1)
	{
		printfDx("画像の読み込みに失敗");
	}
	mnHandle_Waku = LoadGraph("Resource/Option/Graph1.png");
	if (mnHandle_Waku == -1)
	{
		printfDx("枠画像の読み込みに失敗");
	}

	SetFontSize(50); // 文字の大きさ
}

RankingScene3D::~RankingScene3D()
{

}

void RankingScene3D::Initialize()
{
	score.ScoreInitialize(); // 初期化して文字の大きさやってる
	// 過去ランキングを読み込む
	score.LoadRanking();

	// タイトルロゴのクラスの作成
	// プレイヤーの生成 などをここで行う
	// タイトル画面で必要なオブジェクトをここで生成する

	// BGM再生 ながさん
	// Master::mpSoundManager->PlayBGM(SoundManager::BGM_TITLE);
}


void RankingScene3D::Update()
{
	// intをboolに変換して揃えて押されたら絶対に返すようにする　1118
	bool KEY_BACK_Down = InputManager::CheckDownKey(KEY_INPUT_BACK) != 0;
	bool PAD_A_Down = InputManager::CheckDownPad(XINPUT_BUTTON_A) != 0;

	// 押されたらタイトル
	if (KEY_BACK_Down || PAD_A_Down)
	{
		Master::mpSoundManager->PlaySE(SoundManager::SE_ENTER, 255); // 決定音

		Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::TITLE_3D);
	}

	// ちかちかタイマー
	mCurrentPhoto += 10.0f;
	if (mCurrentPhoto > 600.0f)
	{
		mFlushTimer = !mFlushTimer;
		mCurrentPhoto = 0;
	}


	////// スティック関係追加
	//int StickX, StickY;
	//VECTOR movePad = VGet(0.0f, 0.0f, 0.0f);
	//GetJoypadAnalogInput(&StickX, &StickY, DX_INPUT_PAD1);
	//const int StickZone = 500;

	// 基底クラスの更新処理を呼びだす
	Scene::Update();
}


void RankingScene3D::Draw()
{
	// ２D用に設定
	SetUseZBufferFlag(FALSE);
	SetWriteZBufferFlag(FALSE);

	DrawGraph(0, 0, mnHandle_Bag, TRUE); // 空

	int DrawY_Up = 100; // 画像のの上
	int DrawY_Down = 950; // 画像の下
	int DrawL = 80; // 左
	int DrawR = 1900; // 右

	DrawModiGraph(
		DrawL, DrawY_Up, // 左上
		DrawR, DrawY_Up, // 右上
		DrawR, DrawY_Down, // 右下
		DrawL, DrawY_Down, // 左下
		mnHandle_Waku,
		TRUE);


	/*DrawRotaGraph(
		Utility::SCREEN_WIDTH / 2,
		Utility::SCREEN_HEIGHT / 2,
		1.0f,
		0.0f,
		mnHandle_Waku,
		TRUE
	);*/

	// 色を変えている
	unsigned int Cr2;
	Cr2 = GetColor(255, 255, 255); // siro


	// 点滅が有効なら
	if (mFlushTimer && !mbRoadFlag)
	{
		DrawString(Utility::SCREEN_WIDTH / 2 - 150, Utility::SCREEN_HEIGHT / 2 + 400, "BackSpace　or Aボタンでタイトルへ", Cr2);
	}
	if (mbRoadFlag)
	{
		DrawString(Utility::SCREEN_WIDTH / 2 - 150, Utility::SCREEN_HEIGHT / 2 + 400, "ロード中です", Cr2);

	}

	score.DrawRanking(600, 300);

	// 3D用に設定
	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);

	// 基底クラスの更新処理を呼びだす
	Scene::Draw();
}


void RankingScene3D::Finalize()
{
	// フォントの削除
	DeleteFontToHandle(mnFontBig);
	DeleteFontToHandle(mnFontNormal);
	DeleteFontToHandle(mnFontSmall);


	DeleteGraph(mnHandle_Bag);


	// BGM停止
	//Master::mpSoundManager->StopBGM();
}