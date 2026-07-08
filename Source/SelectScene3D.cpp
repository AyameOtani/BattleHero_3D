#include "SelectScene3D.h"
#include "Utility.h"
#include "DxLib.h"
#include "Master.h"
#include "InputManager.h"
#include "Status3D.h"
#include "DrawPlayer.h"
#include "Player3D.h"
#include "GameManager.h"


// 選択シーンにモデルを表示する可能性があるため3D
SelectScene3D::SelectScene3D()
	:Scene()
{
	mnFontBig = CreateFontToHandle(
		NULL, // 今使ってるのと同じになるはず
		75,   // 大きさだけ違う
		3     // 太さらしいけど変えても意味なし
	);

	mnFontNormal = CreateFontToHandle(
		NULL,
		45,   // 今のフォントサイズ
		3
	);

	mnFontSmall = CreateFontToHandle(
		NULL,
		25,   // 小さめのフォントサイズ
		3
	);

	mnFontSsize = CreateFontToHandle(
		NULL,
		33,   // Sフォントサイズ
		3
	);

	// 画像読み込み用１
	mnHandle_Bag = LoadGraph("Resource/Option/Graph.png");
	if (mnHandle_Bag == -1)
	{
		printfDx("画像の読み込みに失敗");
	}

	mnHandle_Select = LoadGraph("Resource/Graph/Graph00.png");
	if (mnHandle_Select == -1)
	{
		printfDx("画像の読み込みに失敗");
	}

	mnHandle_No_Select = LoadGraph("Resource/Graph/Graph01.png");
	if (mnHandle_No_Select == -1)
	{
		printfDx("画像の読み込みに失敗");
	}

	mnHandle_Sky = LoadGraph("Resource/Scene/sky2.png");
	if (mnHandle_Sky == -1)
	{
		printfDx("画像の読み込みに失敗");
	}

	SetFontSize(35); // 文字の大きさ
	mnSelect = 1; // 初期を１にする
	mnChengeSelect = 1;
}


SelectScene3D::~SelectScene3D()
{
	
}


void SelectScene3D::Initialize()
{
	// ここでカメラを元の位置に戻している
	Master::mpCamera->Reset();

	mpDrawPlayer = new DrawPlayer("", VGet(-100, 0, 0));
	mpDrawPlayer->AllLoad();
	mpDrawPlayer->SetDrawPlayer(DrawPlayer::Hero);

	

	// BGM再生 ループさせたいからfalseにｓた
	Master::mpSoundManager->PlayBGM(SoundManager::BGM_SELECT, false, 190);
}


void SelectScene3D::Update()
{
	// intをboolに変換して揃えて押されたら絶対に返すようにする　1118
	bool KEY_ENTER_Down = InputManager::CheckDownKey(KEY_INPUT_RETURN) != 0;
	bool PAD_B_Down = InputManager::CheckDownPad(XINPUT_BUTTON_B) != 0;

	// エンターキーが押されたらゲーム起動画面へ移動
	if (KEY_ENTER_Down || PAD_B_Down)
	{
		// SE再生
		Master::mpSoundManager->PlaySE(SoundManager::SE_ENTER);

		mbRoadFlag = true; // ロード中かのフラグをONにする

		// ここで設定した数値をStatusに代入
		switch (mnSelect)
		{
		case 1:
			Master::msPlayerModelName = ("Resource/3d/Player/Hero.mv1");
			break;

		case 2: // ここを基本のＨｐとＢＯＯＳＴにする
			Master::msPlayerModelName = ("Resource/3d/Zombie/Zombie_Mesh.mv1");
			break;

		case 3:
			Master::msPlayerModelName = ("Resource/3d/Military/Guntai.mv1");
			break;

		case 4:
			Master::msPlayerModelName = ("Resource/3d/Bob/Bob.mv1");
			break;

		case 5:
			Master::msPlayerModelName = ("Resource/3d/Devil/Devil.mv1");
			break;

		default: // 保険
			Master::msPlayerModelName = ("Resource/3d/Player/Hero.mv1");
			break;
		}

		// どれを選んだかの保存
		GameManager::mnSelectPlayerDate = mnSelect;
		// ステータスとか入れてる
		auto spec = Status3D::GetPlayerSpec(mnSelect);
		Master::mpNextStatus = std::make_unique<Status3D>(spec);

		// ここで、武器選択画面にいくようにした 0206
		Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::SELECT_WEAPON_3D);
	}

	// intをboolに変換して揃えて押されたら絶対に返すようにする　1118
	bool BACK = InputManager::CheckDownKey(KEY_INPUT_BACK) != 0;
	bool PAD_BACK = InputManager::CheckDownPad(XINPUT_BUTTON_A);
	//プレイヤー選択画面に戻る
	if (BACK || PAD_BACK)
	{
		mbRoadFlag = true;
		// SE再生
		Master::mpSoundManager->PlaySE(SoundManager::SE_BACKSPACE, 200); // 大きめ

		Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::TITLE_3D);
	}


	// スティック関係追加
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

	if (mnSelect != mnChengeSelect)
	{
		Master::mpSoundManager->PlaySE(SoundManager::SE_SELECT, 130); // 選択音 ちいさめ
		mnChengeSelect = mnSelect; // 更新
	}

	PlayerUpdate(); // 呼び出し関数

	// 基底クラスの更新処理を呼びだす
	Scene::Update();

	// ちかちかタイマー
	mCurrectPhoto += 10.0f;
	if (mCurrectPhoto > 300.0f)
	{
		mFlushTimer = !mFlushTimer;
		mCurrectPhoto = 0.0f;
	}
}

void SelectScene3D::PlayerUpdate()
{
	// ここで表示するプレイヤーを切り換えている
	switch (mnSelect)
	{
	case 1:
		mpDrawPlayer->SetDrawPlayer(DrawPlayer::Hero);
		// ここでチェンジを呼べればOKなのだがmpModeで呼べないから悩みだ
		break;

	case 2:
		mpDrawPlayer->SetDrawPlayer(DrawPlayer::Zombi);
		break;

	case 3:
		mpDrawPlayer->SetDrawPlayer(DrawPlayer::Guntai);
		break;

	case 4:
		mpDrawPlayer->SetDrawPlayer(DrawPlayer::Bob);
		break;

	case 5:
		mpDrawPlayer->SetDrawPlayer(DrawPlayer::Devil);
		break;

	}
}

void SelectScene3D::Draw()
{
	// 先に２Dを書く
	// ２D用に設定
	SetUseZBufferFlag(FALSE);
	SetWriteZBufferFlag(FALSE);

	DrawGraph(0, 0, mnHandle_Sky, TRUE);
	Draw2D(); // 背景とかを描画

	// ２D用に設定
	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);


	// 基底クラスの更新処理を呼びだす
	Scene::Draw();
}

void SelectScene3D::Draw2D()
{
	auto spec = Status3D::GetPlayerSpec(mnSelect);


	// 色を変えている
	unsigned int Cr2;
	Cr2 = GetColor(255, 255, 255); // 白
	//スケール設定描画 青い枠のやつ
	DrawRotaGraph(1350, 470, 1.0f, 0.0f, mnHandle_Bag, TRUE);


	// 選択中武器の文字表示
	DrawFormatStringToHandle(
		50,
		30,
		GetColor(255, 255, 255),
		mnFontNormal,
		"プレイヤー を選択中\n"
	);

	//　戻るやつ
	DrawFormatStringToHandle(
		50,
		100,
		GetColor(255, 255, 255),
		mnFontSsize,
		"[BackSpace / A] でタイトルへ戻る"
	);


	{   // 選択しているやつだけ表示
		int MaxState = 4; // BOSとかの描画最大数


		float x = (float)Utility::SCREEN_WIDTH / 2 + 40;
		float y = (float)Utility::SCREEN_HEIGHT / 2 - 330;


		float stringY = y + 130;
		// ---------ステータス系--------------------------
		// HP
		DrawFormatStringToHandle((int)x, (int)stringY, GetColor(255, 255, 255), mnFontNormal,
			"HP         %d", (int)spec.hp);
		// BOOST
		DrawFormatStringToHandle((int)x, (int)stringY + 60, GetColor(255, 255, 255), mnFontNormal,
			"BOOST      %d", (int)spec.boost);
		// SPEED
		DrawFormatStringToHandle((int)x, (int)stringY + 120, GetColor(255, 255, 255), mnFontNormal,
			"SPEED      %d", (int)spec.speed);
		// MAXSPEED
		DrawFormatStringToHandle((int)x, (int)stringY + 180, GetColor(255, 255, 255), mnFontNormal,
			"MAX SPEED  %d", (int)spec.maxSpeed);


		// 名前は少し大きめ
		DrawFormatStringToHandle(
			(int)x,
			(int)y,
			GetColor(255, 255, 255),
			mnFontBig,
			"%s",
			spec.name.c_str()
		);

		// 説明は少し小さめ
		DrawFormatStringToHandle(
			(int)x,
			(int)y + 370,
			GetColor(255, 255, 255),
			mnFontSmall,
			"\n\n\n%s",
			spec.memo.c_str()
		);


		// いま選択している奴のバーを描画したい
		const int Bx = 1370; // 位置X
		const int By = 345; // 位置Y
		const int BoostY = By + 60; // 位置 BOOST


		const int width = 300; // 横幅
		const int height = 30; // 縦幅
		int color = 0;
		color = GetColor(136, 255, 255); // 青白



		// バーの黒い背景の描画
		for (int i = 1; i <= MaxState; i++) // 表示するため
		{
			int posY = By + (i - 1) * 60; // 毎回100ずつずらす
			DrawBox(Bx, posY, Bx + width, posY + height, GetColor(80, 80, 80), TRUE);
		}



		// 左の枠
		int SelectNow = mnSelect;
		for (int i = 1; i <= mnSelectMax; i++)
		{
			int posY = ((int)y - 50) + (i - 1) * 76;
			int DarkColor = GetColor(150, 150, 150);


			// 使っている画像サイズ
			int imgW = 1024;
			int imgH = 512;

			// 描画位置
			int drawX;
			int drawY;

			float scale; // 画像でかすぎだから直す

			// 左上から中心に戻す
			int cx;
			int cy;

			// 左側の枠
			if (i == SelectNow) //選択しているやつだけ明るく　大きく
			{
				drawX = -450;
				drawY = posY - 230;
				cx = drawX + imgW / 2;
				cy = drawY + imgH / 2;
				scale = 0.50;

				DrawRotaGraph(
					cx,
					cy,
					scale,
					0.0,
					mnHandle_Select,
					TRUE
				);


				// 名前は少し大きめ
				DrawFormatStringToHandle(
					30,
					posY + 5,
					GetColor(255, 255, 255),
					mnFontNormal,
					"%s",
					spec.name.c_str()
				);
			}
			else // 選択していないやつ
			{
				drawX = -600;
				drawY = posY - 230;
				cx = drawX + imgW / 2;
				cy = drawY + imgH / 2;
				scale = 0.40f;


				// ここに位がぞ王を用意したい
				DrawRotaGraph(
					cx,
					cy,
					scale,
					0.0,
					mnHandle_No_Select,
					TRUE
				);

			}
		}



		{
			//---------------------- HPゲージの割合----------------------------
			float rate = spec.hp / (float)MaxSta;
			if (rate < 0.0f)
			{
				rate = 0.0f;
			}
			if (rate > 1.0f)
			{
				rate = 1.0f;
			}
			DrawBox(Bx, By, Bx + (int)(width * rate), By + height, color, TRUE); // HP


		}


		{
			// ---------------------スタミナのゲージの描画-------------------------
			// ゲージの割合
			float boostrate = spec.boost / (float)MaxStaB;
			if (boostrate < 0.0f)
			{
				boostrate = 0.0f;
			}
			if (boostrate > 1.0f)
			{
				boostrate = 1.0f;
			}
			DrawBox(Bx, BoostY, Bx + (int)(width * boostrate), BoostY + height, color, TRUE); // HP


		}


		const int SpeedY = BoostY + 60;
		const int MaxSpY = SpeedY + 60;

		{
			// ---------------------スピードのゲージの描画-------------------------
			// ゲージの割合
			float boostrate = spec.speed/ (float)MaxSp;
			if (boostrate < 0.0f)
			{
				boostrate = 0.0f;
			}
			if (boostrate > 1.0f)
			{
				boostrate = 1.0f;
			}
			DrawBox(Bx, SpeedY, Bx + (int)(width * boostrate), SpeedY + height, color, TRUE);

		}



		{
			// ---------------------最大速度のゲージの描画-------------------------
			// ゲージの割合
			float boostrate = spec.maxSpeed / (float)MaxSp;
			if (boostrate < 0.0f)
			{
				boostrate = 0.0f;
			}
			if (boostrate > 1.0f)
			{
				boostrate = 1.0f;
			}
			DrawBox(Bx, MaxSpY, Bx + (int)(width * boostrate), MaxSpY + height, color, TRUE);
		}

		// 白枠の描画
		for (int i = 1; i <= MaxState; i++) // 表示するため
		{
			int posY = By + (i - 1) * 60; // 毎回100ずつずらす
			DrawBox((int)Bx, (int)posY, (int)Bx + (int)width, (int)posY + (int)height, GetColor(255, 255, 255), FALSE);
		}
	}

	float DrawX = Utility::SCREEN_WIDTH / 2 + 400; // この横のを今は使うので変えている もとは半分のやつ
	float DrawY = Utility::SCREEN_HEIGHT / 2 + 300;
	DrawString((int)DrawX - 200, (int)DrawY + 150, "Lスティック または WSキーで選択", Cr2); // のちのち画像かな

	if (mFlushTimer)
	{
		DrawString((int)DrawX, (int)DrawY + 100, "Enterで決定", Cr2);
	}
}


void SelectScene3D::Finalize()
{
	// フォントハンドルの削除
	DeleteFontToHandle(mnFontBig);
	DeleteFontToHandle(mnFontNormal);
	DeleteFontToHandle(mnFontSmall);
	DeleteFontToHandle(mnFontSsize);

	// グラフィックハンドルの削除（これらも毎回ロードするなら必須）
	DeleteGraph(mnHandle_Bag);
	DeleteGraph(mnHandle_Select);
	DeleteGraph(mnHandle_No_Select);
	DeleteGraph(mnHandle_Sky);


	// BGM停止
	// Master::mpSoundManager->StopBGM();
	delete mpDrawPlayer;
}
