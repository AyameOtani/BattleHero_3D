#include "LoseResult3D.h"
#include "Utility.h" // 呼び出すと、SCREEN_WIDTHとかを使える
#include "DxLib.h"
#include "Master.h"
#include "inputManager.h"
#include "GameManager.h"
#include "Score3D.h"
#include "Slide3D.h"


LoseResult3D::LoseResult3D()
	: Scene()     // 基底クラスのコンストラクタを呼び出しておく,
{
	// 画像読み込み場所 蒼い四角
	mnHandle_Bag = LoadGraph("Resource/Option/Graph.png");
	if (mnHandle_Bag == -1)
	{
		printfDx("画像の読み込みに失敗");
	}
	mnHandle_Sky = LoadGraph("Resource/Scene/sky2.png");
	if (mnHandle_Sky == -1)
	{
		printfDx("画像の読み込みに失敗");
	}


	// 文字の大きさ関係
	mnFontBig = CreateFontToHandle(
		NULL,
		150, // すごく大きく
		3
	);

	mnFontNormal = CreateFontToHandle(
		NULL,
		100,   // 今のフォントサイズ
		3
	);

	mnFontSmall = CreateFontToHandle(
		NULL,
		50,   // 小さめのフォントサイズ
		3
	);



	SetFontSize(40); // 文字の大きさ
}

LoseResult3D::~LoseResult3D()
{
	
}

void LoseResult3D::Initialize()
{
	Master::mpCamera->Reset();
	mnHandle_D = Graph::DefeatPhoto(Master::msPlayerModelName);

	m_Slide.Reset();



    // 過去ランキングを読み込む
	m_Score.LoadRanking();
	// スコアクラスにデータを渡して計算
	m_Score.ScoreGet(GameManager::mnLastTime, GameManager::mnLastPlayerHp, GameManager::mnKillEnemyCount);
	//  ランキングに保存
	m_Score.SaveRanking();


	// タイトルロゴのクラスの作成
	// プレイヤーの生成 などをここで行う
	// タイトル画面で必要なオブジェクトをここで生成する

	// BGM再生 選択画面と同じだけどtag分けた
	Master::mpSoundManager->PlayBGM(SoundManager::BGM_RESULT, true, 190);
	// SE再生
	Master::mpSoundManager->PlaySE(SoundManager::SE_GAMWSET); // ウィーンみたいな
}


void LoseResult3D::Update()
{
	m_Slide.Update();

	// intをboolに変換して揃えて押されたら絶対に返すようにする　1118
	bool KEY_ENTER_Down = InputManager::CheckDownKey(KEY_INPUT_RETURN) != 0;
	bool PAD_B_Down = InputManager::CheckDownPad(XINPUT_BUTTON_B) != 0;

	if (m_Slide.GetSlideEnd())
	{
		// エンターキーが押されたらゲーム起動画面へ移動
		if (KEY_ENTER_Down || PAD_B_Down)
		{
			// SE再生
			Master::mpSoundManager->PlaySE(SoundManager::SE_ENTER); // 決定音

			Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::TITLE_3D);
		}


		mCurrentPhoto += 10.0f;
		if (mCurrentPhoto > 300.0f)
		{
			mFlushTimer = !mFlushTimer;
			mCurrentPhoto = 0;
		}
	}
	
	// 基底クラスの更新処理を呼びだす
	Scene::Update();
}


void LoseResult3D::Draw()
{
	// ２D用に設定
	SetUseZBufferFlag(FALSE);
	SetWriteZBufferFlag(FALSE);

	// 背景空
	DrawGraph(0, 0, mnHandle_Sky, TRUE);
	// プレイヤーの画像
	DrawGraph(0, 0, mnHandle_D, TRUE);

	//スケール設定描画 青い枠のやつ
	DrawRotaGraph(1350, 470, 1.0f, 0.0f, mnHandle_Bag, TRUE);



	// 色を変えている
	unsigned int Cr2;
	Cr2 = GetColor(255, 255, 255); // 白

	// 点滅が有効なら
	if (mFlushTimer && (m_Slide.GetSlideEnd()))
	{
		DrawString(Utility::SCREEN_WIDTH / 2 + 300, Utility::SCREEN_HEIGHT / 2 + 400, "Enterでタイトル", Cr2);
	}


	// 位置X
	float X = (float)Utility::SCREEN_WIDTH / 2;
	// スコアの表示
	int resultTime = GameManager::mnLastTime;
	int resultPlayerHp = GameManager::mnLastPlayerHp;
	int resultEnemyCount = GameManager::mnKillEnemyCount;
	int totalNumber = (resultEnemyCount + resultPlayerHp + resultTime);

	// スコアの代入
	m_Score.ScoreGet(resultTime, resultPlayerHp, resultEnemyCount);

	// DEFEAT
	DrawFormatStringToHandle((int)X, 200, Cr2, mnFontBig, "DEFEAT");
	DrawFormatStringToHandle((int)X, 420, Cr2, mnFontNormal, "評価");
	DrawFormatStringToHandle((int)X, 550, Cr2, mnFontSmall, "合計得点\n""生存時間\n""残りHP\n""撃破数\n");


	// スライドの基準位置
	float startX = (float)Utility::SCREEN_WIDTH + 100.0f;
	float endX = (float)Utility::SCREEN_WIDTH / 2.0f + 400.0f;

	Slide3D::SlideData slides[] =
	{
		// 動き出し時間 停止時間 Y フォント だしたいやつ
		// ループで使うときに%sでやっているからバグるからto_stringってのにした
		{ 10,  40, 600, mnFontSmall,  std::to_string((int)resultTime) },     // 時間	    1
		{ 30,  60, 650, mnFontSmall,  std::to_string((int)resultPlayerHp) }, // 残りHP	    2
		{ 50,  80, 700, mnFontSmall,  std::to_string(resultEnemyCount) },	 // げきは数    3
		{ 125, 160, 420, mnFontNormal, m_Score.TotalGameScore() },            // 評価　英語 5
		{ 80, 110, 550, mnFontSmall, std::to_string((int)totalNumber) }      // 合計得点    4
	};
	// ＆つけないとなんか点々でる
	for (const auto& i : slides)
	{
		if (m_Slide.GetTimer() > i.SlideStart)
		{
			float x = m_Slide.SlideDirection((int)i.SlideStart, (int)i.SlideEnd, startX, endX);
			DrawFormatStringToHandle((int)x, (int)i.SlideY, Cr2, i.SlideFont, "%s", i.moji.c_str());
		}
	}


	if (m_Slide.GetSlideEnd())
	{
		m_Score.DrawMemo(); // らんくいんとか
		// m_Score.DrawRanking(100,200);
	}

	// 3D用に設定
	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);

	// 基底クラスの更新処理を呼びだす
	Scene::Draw();
}


void LoseResult3D::Finalize()
{
	DeleteGraph(mnHandle_Bag);
	DeleteGraph(mnHandle_Sky);
	DeleteGraph(mnHandle_D);
	//  フォントハンドルの削除
	DeleteFontToHandle(mnFontBig);
	DeleteFontToHandle(mnFontNormal);
	DeleteFontToHandle(mnFontSmall);


	// BGM停止
	Master::mpSoundManager->StopBGM();
}