#include "OperateScene3D.h"
#include "Utility.h" // 呼び出すと、SCREEN_WIDTHとかを使える
#include "DxLib.h"
#include "Master.h"
#include "inputManager.h"
#include "Player3D.h"


OperateScene3D::OperateScene3D()
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

	// 画像をロード
	mGraph =
	{
		-1, // ここは０だから-1にしとく
		LoadGraph("Resource/Manual/Hp.png"),
		LoadGraph("Resource/Manual/Jump.png"),
		LoadGraph("Resource/Manual/Baria.png"), // バリアも画像まとめる
		LoadGraph("Resource/Manual/OverHeat.png"),
		LoadGraph("Resource/Manual/Weapon.png"),
		LoadGraph("Resource/Manual/Time.png"), // 時間も纏める
		LoadGraph("Resource/Manual/RockOn1.png"), // ロックオン纏める
		LoadGraph("Resource/Manual/Poaz.png")
	};


	// 画像読み込み場所
	mnHandle_Bag = LoadGraph("Resource/Scene/sky.png");
	if (mnHandle_Bag == -1)
	{
		printfDx("画像の読み込みに失敗");
	}

	SetFontSize(50); // 文字の大きさ
	mnSelect = 1; // 初めは１
	mnSelectChenge = 1; // 1
}

OperateScene3D::~OperateScene3D()
{

}

void OperateScene3D::Initialize()
{

	// タイトルロゴのクラスの作成
	// プレイヤーの生成 などをここで行う
	// タイトル画面で必要なオブジェクトをここで生成する

	// BGM再生
	//Master::mpSoundManager->PlayBGM(SoundManager::BGM_TITLE);
}


void OperateScene3D::Update()
{
	// intをboolに変換して揃えて押されたら絶対に返すようにする　1118
	bool KEY_ENTER_Down = InputManager::CheckDownKey(KEY_INPUT_BACK) != 0;
	bool PAD_B_Down = InputManager::CheckDownPad(XINPUT_BUTTON_A) != 0;

	// エンターキーが押されたらゲーム起動画面へ移動
	if (KEY_ENTER_Down || PAD_B_Down)
	{

		Master::mpSoundManager->PlaySE(SoundManager::SE_ENTER, 255); // 決定音
		Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::TITLE_3D);
		mbRoadFlag = true; // ロード中かのフラグをONにする

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
	if (!mbStickInput && (InputManager::CheckDownKey(KEY_INPUT_S) || (StickY > StickZone)))
	{
		mnSelect++;
		if (mnSelect > mnSelectMax)
		{
			mnSelect = 1;
		}
		mbStickInput = true;
	}
	// Lスティック上
	if (!mbStickInput && (InputManager::CheckDownKey(KEY_INPUT_W) || (StickY < -StickZone)))
	{
		mnSelect--;
		if (mnSelect < 1)
		{
			mnSelect = mnSelectMax;
		}
		mbStickInput = true;
	}

	// スティックが中央に戻ったらフラグのリセット
	if (StickY < StickZone && StickY > -StickZone)
	{
		mbStickInput = false; // ここでfalseにしてまた入力出来るように
	}

	// 違うならSEポーン
	if (mnSelect != mnSelectChenge)
	{
		Master::mpSoundManager->PlaySE(SoundManager::SE_SELECT, 130); // 選択音  ちいさめ
		mnSelectChenge = mnSelect; // 更新
	}

	// 基底クラスの更新処理を呼びだす
	Scene::Update();
}


void OperateScene3D::Draw()
{
	// ２D用に設定
	SetUseZBufferFlag(FALSE);
	SetWriteZBufferFlag(FALSE);

	DrawGraph(0, 0, mnHandle_Bag, TRUE); // 空
    DrawBox(750, 92, 1675, 620, GetColor(255,255,255), TRUE); // 画像の後ろ

	// 色を変えている
	unsigned int Cr2;
	Cr2 = GetColor(255, 255, 255); // siro

	// 画像表示
	if (mnSelect >= 1 && mnSelect < mGraph.size())
	{
		// 画像の番号と選択の場の腕同じものを出している
		DrawGraph(700, 100, mGraph[mnSelect], TRUE);
	}


	// 点滅が有効なら
	if (mFlushTimer && !mbRoadFlag)
	{
		DrawString(Utility::SCREEN_WIDTH / 2 - 150, Utility::SCREEN_HEIGHT / 2 + 400, "W/S or Lスティックで選択\n""BackSpace　or Aボタンでタイトルへ", Cr2);
	}
	if (mbRoadFlag)
	{
		DrawString(Utility::SCREEN_WIDTH / 2 - 150, Utility::SCREEN_HEIGHT / 2 + 400, "ロード中です", Cr2);

	}


	{
		// 出したい文字列
		std::string hpList[] =
		{
			"", // 一つ目は空に
			"ゲームについて",
			"上昇",
			"バリア",
			"OVER HEAT",
			"武器の変え方",
			"制限時間",
			"ロックオン",
			"ポーズ画面",
		};

		// 説明のやつ
		// 名前
		std::string name[] =
		{
			"",                              // 空
			"",	                             // ステータス
			"[SPACEキー / Bボタン or LT]",   // 上昇
			"[Vキー / Aボタン]",	         // バリア
			"",                              // オーバーヒート
			"[Eキー / RBボタン]",            // 武器変え方
			"",	                             // 制限時間
			"[Rキー or 右shift / Rスティック押し込み]", // ロックオン
			"[tabキー / STARTボタン]"        // ポーズ
		};
		// 説明
		std::string memo[] =
		{
			"", // 空

			// ステータス
			"このゲームは選択したキャラや武器を使い\n"
			"制限時間内に敵を全滅させるゲームです\n"
			"画面UIは上の図のようになっています",

			// 上昇
			"単押しでジャンプが出来る\n"
			"長押しで上昇出来る\n"
			"これらの操作にはBOOSTが必要",

			// バリア
			"単押しで一定時間バリアを展開する\n"
			"バリア中は無敵だがクールタイムがあるので注意!",

			// OVERHEAT
			"BOOSTゲージが0になるとOVERHEATしてしまう\n"
			"OVERHEATは一定時間で回復する",

			// 武器の切り換え
			"メイン武器とサブ武器に変更出来る\n"
			"プレイヤーが走りまたは待機の場面だけ変更できます\n"
			"攻撃中や上昇中は変更出来ません",

			// 制限時間
			"左上のタイマーが0になったら強制的に敗北する\n"
			"時間にも気を付けて戦おう",

			// ロックオン
			"敵にマークが出現していたらロックオン出来る\n"
			"ロックオンするとカメラは敵を追跡する\nロックオンの色によって弾が追跡弾になる",

			// ポーズ画面
			"ゲーム中に押すとポーズ画面を開くことが出来る"
		};


		// 参考演算子で選択中のを見やすく表示
		float x = (float)Utility::SCREEN_WIDTH / 2 - 800;
		float y = (float)Utility::SCREEN_HEIGHT / 2 - 400;
		int SelectNow = mnSelect; // 1, 2, 3 のどれを強調するか
		for (int i = 1; i <= mnSelectMax; i++) // 表示するため
		{
			int posY = (int)y + (i - 1) * 75; // 毎回75ずつずらす
			// 選択中か、それ以外かの処理を三項演算子でやってる
			int Color = (i == SelectNow) ? GetColor(255, 80, 80) : GetColor(150, 60, 60);
			int Font = (i == SelectNow) ? mnFontBig : mnFontNormal;

			int BoxColor = (i == SelectNow) ? GetColor(255, 255, 255) : GetColor(220, 220, 220);
			// ここで四角と文字を描いてる
			DrawBox((int)x - 20, (int)posY, (int)x + 350, (int)posY + 70, BoxColor, TRUE);
			DrawFormatStringToHandle((int)x, (int)posY + 10, Color, Font, "%s", hpList[i].c_str());
		}

		// 説明 選択されているやつの表示 キーと説明
		DrawFormatStringToHandle(750, 670, GetColor(255, 228, 95), mnFontNormal, "%s", name[mnSelect].c_str());
		// 名前があるならYを下に　ないなら名前の位置
		int nameY = 0;
		if (name[mnSelect] == "") {nameY = 670;}
		else { nameY = 750;}
		DrawFormatStringToHandle(750, nameY, GetColor(255, 255, 255), mnFontSmall, "%s", memo[mnSelect].c_str());

	}

	// 3D用に設定
	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);


	// 基底クラスの更新処理を呼びだす
	Scene::Draw();
}


void OperateScene3D::Finalize()
{
	// フォントの削除
	DeleteFontToHandle(mnFontBig);
	DeleteFontToHandle(mnFontNormal);
	DeleteFontToHandle(mnFontSmall);


	// vectorの画像ハンドルをすべて削除
	for (int handle : mGraph)
	{
		if (handle != -1)
		{
			DeleteGraph(handle);
		}
	}
	mGraph.clear(); // 中身を空にする
	DeleteGraph(mnHandle_Bag);


	// BGM停止
	//Master::mpSoundManager->StopBGM();
}