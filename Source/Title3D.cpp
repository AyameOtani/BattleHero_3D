#include "Title3D.h"
#include "Utility.h" // 呼び出すと、SCREEN_WIDTHとかを使える
#include "DxLib.h"
#include "Master.h"
#include "inputManager.h"
#include "Player3D.h"
#include "SkyBox.h"
#include "Stage.h"
#include "GameManager.h"
#include "PlayerAnim.h"


Title3D::Title3D()
	: Scene()     // 基底クラスのコンストラクタを呼び出しておく
	, mbSelectGame(false)
{
	// おおきい文字の設定
	mnFontBig = CreateFontToHandle(
		NULL,
		80,
		3
	);

	// 普通の文字の設定
	mnFontNormal = CreateFontToHandle(
		NULL,
		80,
		3
	);

	// 小さいの文字の設定
	mnFontSmall = CreateFontToHandle(
		NULL,
		50,
		3
	);
	// 小さいの文字の設定
	mnFontSmall2 = CreateFontToHandle(
		NULL,
		40,
		3
	);


	// 画像読み込み用１
	mnHandel_Tutorial = LoadGraph("Resource/Option/Graph.png");
	if (mnHandel_Tutorial == -1)
	{
		printfDx("画像の読み込みに失敗");
	}

	// 画像読み込み場所
	mnHandle_Bag = LoadGraph("Resource/Scene/Logo2.png");
	if (mnHandle_Bag == -1)
	{
		printfDx("画像の読み込みに失敗");
	}
	mnHandle_Sky = LoadGraph("Resource/Scene/sky.png");
	if (mnHandle_Sky == -1)
	{
		printfDx("画像の読み込みに失敗");
	}


	SetFontSize(60); // 文字の大きさ
	mnSelect = 1; // 初めは１
	mnGameSelect = 1; // １
	mnSelectChenge = 1;
	mnGameSelectChenge = 1;
}

Title3D::~Title3D()
{

}

void Title3D::Initialize()
{
	// ここでカメラを元の位置に戻している
	Master::mpCamera->Reset();

	if (GameManager::Resources.skyBoxModel == -1)
	{
		// まだ誰もロードしていないなら、普通にロードして GameManager に預ける
		GameManager::Resources.skyBoxModel = MV1LoadModel("Resource/3d/SkyBox/SkyBox.x");
	}
	// あとは GameManager にあるハンドルを使って SkyBox を生成
	SkyBox* pSkyBox = new SkyBox(GameManager::Resources.skyBoxModel);
	pSkyBox->SetScale(9.0f);

	if (GameManager::Resources.titleStage == -1)
	{
		// まだ誰もロードしていないなら、普通にロードして GameManager に預ける
		GameManager::Resources.titleStage = MV1LoadModel("Resource/mapModel/model_map_title.mqo");
	}
	new Stage(GameManager::Resources.titleStage, -1);



	// BGM再生 最初から再生しないからPlayBGMのreturnにはいるから神
	Master::mpSoundManager->PlayBGM(SoundManager::BGM_TITLE, false, 150);
}



void Title3D::Update()
{
	// メニューの選択が変わったかチェック
	if (!mbSelectGame && mnSelect != mnSelectChenge)
	{
		Master::mpSoundManager->PlaySE(SoundManager::SE_SELECT, 110); // 選択音 ちいさけ
		mnSelectChenge = mnSelect; // 更新
	}
	// ゲーム開始の選択が変わったかチェック
	if (mbSelectGame && mnGameSelect != mnGameSelectChenge)
	{
		Master::mpSoundManager->PlaySE(SoundManager::SE_SELECT, 110); // 選択音  ちいさめ
		mnGameSelectChenge = mnGameSelect; // 更新
	}


	// intをboolに変換して揃えて押されたら絶対に返すようにする　1118
	bool KEY_ENTER_Down = InputManager::CheckDownKey(KEY_INPUT_RETURN) != 0;
	bool PAD_B_Down = InputManager::CheckDownPad(XINPUT_BUTTON_B) != 0;

	// 名前入力モード中なら
	if (mbInputName)
	{
		// キー入力ハンドルの入力が終了しているか取得する
		// 打っている間は0　でENTERが長荒れたら1を返すやつ
		if (CheckKeyInput(mNameInputHandle) == 1 || PAD_B_Down)
		{
			Master::mpSoundManager->PlaySE(SoundManager::SE_ENTER, 110); // 決定音

			Master::mpSoundManager->PlaySE(SoundManager::SE_OPEN, 255); // 選択音


			char temp[11];  // 最大10文字
			GetKeyInputString(temp, mNameInputHandle);	// 撃ち込まれたやつをtempにコピー
			mName = temp;  // 代入

			// なにも入力されなかったら(仮)やる
			if (mName == "" || mName.empty())
			{
				mName = "Player";
			}

			GameManager::Players.name = mName; // 文字を代入
			GameManager::Players.isEntryFinished = true;  //入力したかフラグをON

			DeleteKeyInput(mNameInputHandle); // 四角い枠を消す
			mNameInputHandle = -1;
			mbInputName = false;// 名前入力をfalse
			mbSelectGame = true; // チュートリアルフラグ
		}
	}
	// エンターキーが押されたらゲーム起動画面へ移動
	else if ((KEY_ENTER_Down || PAD_B_Down))
	{
		Master::mpSoundManager->PlaySE(SoundManager::SE_ENTER); // 決定音

		if (!mbSelectGame && !mbInputName)
		{
			switch (mnSelect)
			{
			case 1: // キャラ選択
				mbSelectGame = true;
				mbInputName = true; // 名前入力したいからtrue

				// 日本語の入力をFALSEに運動
				SetUseIMEFlag(FALSE);
				// 最大文字数  キャンセル機能の有無 半角文字のみの入力か 数値文字のみの入力か
				mNameInputHandle = MakeKeyInput(10, FALSE, TRUE, FALSE);
				SetActiveKeyInput(mNameInputHandle);  // 打った文字を送るやつ

				// もし入力したことがあればその名前を初期値にする
				if (GameManager::Players.isEntryFinished)
				{
					SetKeyInputString(GameManager::Players.name.c_str(), mNameInputHandle);
				}

				break;


			case 2: // 説明画面
				Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::OPERATE_SCENE_3D);
				break;


			case 3: // ランキング
				Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::RANKING_SCENE_3D);
				break;


			default: // えらーなら説明画面に飛ぶ
				Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::OPERATE_SCENE_3D);
				break;
			}
		}
		else
		{
			// チュートリアルやるなら１　やらぬなら２
			switch (mnGameSelect)
			{
			case 1: // チュートリアル画面
				Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::TUTORIAL_SCENE_3D);
				break;

			case 2: // 選択画面
				Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::SELECT_SCENE_3D);
				break;

			default: // elseはチュートリアルにした
				Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::TUTORIAL_SCENE_3D);
				break;
			}
			mbRoadFlag = true; // ロード中かのフラグをONにする 説明画面に出すと辺になるからこっちだけ
		}
	}



	// ちかちかタイマー
	mCurrentPhoto += 10.0f;
	if (mCurrentPhoto > 600.0f)
	{
		mFlushTimer = !mFlushTimer;
		mCurrentPhoto = 0;
	}


	//// スティック関係追加
	int StickX, StickY;
	VECTOR movePad = VGet(0.0f, 0.0f, 0.0f);

	GetJoypadAnalogInput(&StickX, &StickY, DX_INPUT_PAD1);
	const int StickZone = 500;

	// 選択するやつ // Lスティック下
	if (!mbStickInput && (InputManager::CheckDownKey(KEY_INPUT_S) || (StickY > StickZone)) && !mbInputName)
	{
		if (mbSelectGame) // フラグ経っていたらGameelectを動かす
		{
			mnGameSelect++;
			if (mnGameSelect > mngameSelectMax)
			{
				mnGameSelect = 1;
			}
		}
		else // たってないなら普通に
		{
			mnSelect++;
			if (mnSelect > mnSelectMax)
			{
				mnSelect = 1;
			}
		}
		mbStickInput = true;
	}
	// Lスティック上
	if (!mbStickInput && (InputManager::CheckDownKey(KEY_INPUT_W) || (StickY < -StickZone)) && !mbInputName)
	{
		if (mbSelectGame) // フラグ経っていたらGameelectを動かす
		{
			mnGameSelect--;
			if (mnGameSelect < 1)
			{
				mnGameSelect = mngameSelectMax;
			}
		}
		else
		{
			mnSelect--;
			if (mnSelect < 1)
			{
				mnSelect = mnSelectMax;
			}
		}
		mbStickInput = true;
	}

	// スティックが中央に戻ったらフラグのリセット
	if (StickY < StickZone && StickY > -StickZone)
	{
		mbStickInput = false; // ここでfalseにしてまた入力出来るように
	}


	// 基底クラスの更新処理を呼びだす
	Scene::Update();
}


void Title3D::Draw()
{
	// 基底クラスの更新処理を呼びだす
	Scene::Draw();


	// 色を変えている
	unsigned int Cr2;
	Cr2 = GetColor(255, 255, 255); // siro

	// ２D用に設定
	SetUseZBufferFlag(FALSE);
	SetWriteZBufferFlag(FALSE);

	{
		std::string hpList[] =
		{
			"",
			"NEW　GAME",
			"  MANUAL",
			" RANKING"
		};

		// 参考演算子で選択中のを見やすく表示
		float x = Utility::SCREEN_WIDTH / 2 - 150;
		float y = Utility::SCREEN_HEIGHT / 2 + 70;
		int SelectNow = mnSelect; // 1, 2, 3 のどれを強調するか
		for (int i = 1; i <= mnSelectMax; i++) // 表示するため
		{
			int posY = (int)y + (i - 1) * 75; // 毎回100ずつずらす
			if (i == SelectNow)
			{	//ここで枠
				DrawBox((int)x - 5, (int)posY - 5, (int)x + 385, (int)posY + 75, GetColor(120, 255, 233), TRUE);
			}

			// 選択中か、それ以外かの処理を三項演算子でやってる
			int Color = (i == SelectNow) ? GetColor(255, 255, 255) : GetColor(150, 150, 150); // 文字色
			int Font = (i == SelectNow) ? mnFontBig : mnFontNormal; // サイズ　今は同じ

			int BoxColor = (i == SelectNow) ? GetColor(115, 200, 210) : GetColor(25, 25, 25); // 背景ボックス
			DrawBox((int)x, posY, (int)x + 380, posY + 70, BoxColor, TRUE);
			DrawFormatStringToHandle((int)x, posY, Color, Font, "%s", hpList[i].c_str());

		}

		// 点滅が有効なら
		if (mFlushTimer && !mbRoadFlag)
		{
			DrawFormatStringToHandle((int)x - 100, Utility::SCREEN_HEIGHT / 2 + 350,
				GetColor(255, 255, 255),
				mnFontSmall,
				"W/S or Lスティックで選択\n"
				" Enter or Bボタンで決定", Cr2);

		}
		else if (mbRoadFlag)
		{
			DrawString((int)x, Utility::SCREEN_HEIGHT / 2 + 350, "ロード中です", Cr2);
		}

		// ロゴ
		DrawGraph((int)x - 330, (int)y - 700, mnHandle_Bag, TRUE);
	}


	// 名前入力中なら
	if (mbInputName)
	{
		// 画面中央の座標
		float x = Utility::SCREEN_WIDTH / 2;
		float y = Utility::SCREEN_HEIGHT / 2;

		// 背景を暗く
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, Utility::SCREEN_WIDTH, Utility::SCREEN_HEIGHT, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawFormatStringToHandle((int)x - 280, (int)y - 180, GetColor(255, 255, 255), mnFontSmall, "名前を入力 (英数字10字まで)");
		// 入力中の文字を表示
		DrawKeyInputString((int)x - 120, (int)y - 30, mNameInputHandle);

		DrawFormatStringToHandle((int)x - 150, (int)y + 100, GetColor(200, 200, 200), mnFontSmall2, "[Enter / B] で決定");
	}


	// ゲームが選択されたら色々出す奴
	if (!mbInputName && mbSelectGame)
	{
		// チュートリアルをやるかやらないかのやつ
		std::string game[] =
		{
			"",
			"  PLAY",
			"  SKIP"
		};
		// 参考演算子で選択中のを見やすく表示
		float x = (float)Utility::SCREEN_WIDTH / 2 - 150;
		float y = (float)Utility::SCREEN_HEIGHT / 2;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 245); // 透明度をさげた

		//スケール設定描画 青い枠のやつ
		DrawRotaGraph((int)x + 200, (int)y, 0.9f, 0.0f, mnHandel_Tutorial, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// 文字
		DrawFormatStringToHandle((int)x - 170, (int)y - 200, GetColor(255, 255, 255),
			mnFontSmall,
			"チュートリアルをプレイしますか\n");

		DrawFormatStringToHandle((int)x - 100, (int)y - 130, GetColor(230, 230, 230),
			mnFontSmall2,
			"スキップでキャラ選択に移ります");


		int SelectNow = mnGameSelect; // 1, 2, 3 のどれを強調するか
		for (int i = 1; i <= mngameSelectMax; i++) // 表示するため
		{
			int posY = (int)y + (i - 1) * 75; // 毎回100ずつずらす
			if (i == SelectNow)
			{	//ここで枠
				DrawBox((int)x - 5, posY - 5, (int)x + 385, posY + 75, GetColor(120, 255, 233), TRUE);
			}
			// 選択中か、それ以外かの処理を三項演算子でやってる
			int Color = (i == SelectNow) ? GetColor(255, 255, 255) : GetColor(150, 150, 150); // 文字色
			int Font = (i == SelectNow) ? mnFontBig : mnFontNormal; // サイズ　今は同じ

			int BoxColor = (i == SelectNow) ? GetColor(115, 200, 210) : GetColor(50, 50, 50); // 背景ボックス
			DrawBox((int)x, posY, (int)x + 380, posY + 70, BoxColor, TRUE);
			DrawFormatStringToHandle((int)x + 20, posY, Color, Font, "%s", game[i].c_str());
		}
	}


	// ３D用の戻す
	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);
}


void Title3D::Finalize()
{

	// フォントハンドルの削除
	DeleteFontToHandle(mnFontBig);
	DeleteFontToHandle(mnFontNormal);
	DeleteFontToHandle(mnFontSmall);
	DeleteFontToHandle(mnFontSmall2);

	// グラフィックハンドルの削除
	DeleteGraph(mnHandle_Bag);
	DeleteGraph(mnHandle_Sky);


	//	// BGM停止
	//	Master::mpSoundManager->StopBGM();
}