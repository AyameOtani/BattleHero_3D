#include "SelectWeapon.h"
#include "Utility.h"
#include "DxLib.h"
#include "Master.h"
#include "InputManager.h"
#include "Status3D.h"
#include "DrawWeapon.h"
#include "Player3D.h"
#include "GameManager.h"
#include "Graph.h"


// 選択シーンにモデルを表示する可能性があるため3D
SelectWeapon::SelectWeapon()
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

	mnPlayerFont = CreateFontToHandle(
		NULL, // 今使ってるのと同じになるはず
		30,   // 大きさだけ違う
		3     // 太さらしいけど変えても意味なし
	);

	mnFontSsize = CreateFontToHandle(
		NULL,
		33,   // Sフォントサイズ
		3
	);

	// 画像読み込み用
	mnHandle_Bag = LoadGraph("Resource/Option/Graph2.png");
	if (mnHandle_Bag == -1)
	{
		printfDx("画像の読み込みに失敗");
	}

	mnBoxHandle = LoadGraph("Resource/Option/Graph2.png");
	if(mnBoxHandle == -1)
	{
		printfDx("画像の読み込みに失敗");
	}

	// 選択している左の画像
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

	mnHandle_Error = LoadGraph("Resource/Graph/Error.png");
	if(mnHandle_Error == -1)
	{
		printfDx("メッセ画像の読み込みに失敗");
	}

	mnHandle_ErrorBag = LoadGraph("Resource/Graph/ErrorBag.png");
	if(mnHandle_ErrorBag == -1)
	{
		printfDx("メッセ画像の読み込みに失敗");
	}

	mnHandle_SelectWeapon = LoadGraph("Resource/Graph/WeaponSelect.png");
	if(mnHandle_SelectWeapon == -1)
	{
		printfDx("メッセ画像の読み込みに失敗");
	}


	mnHandle_Sky = LoadGraph("Resource/Scene/sky2.png");
	if (mnHandle_Sky == -1)
	{
		printfDx("画像の読み込みに失敗");
	}


	SetFontSize(35); // 文字の大きさ
	mnSelect = 1; // 初期を１にする
	mnChengeSlect = 1;

	mfErrorCount = ERROR_COUNT;
}


SelectWeapon::~SelectWeapon()
{
	
}


void SelectWeapon::Initialize()
{
	// ここでカメラを元の位置に戻している
	Master::mpCamera->Reset();


	mpDrawWeapon = new DrawWeapon("", VGet(-100, 0, 0));
	mpDrawWeapon->AllLoad();
	mpDrawWeapon->SetDrawPlayer(DrawWeapon::Shild);


	// プレイヤーの証明画像のやつ
	//VECTOR pos = VGet(190, 906, 0);
	// mpGraph = Graph::PlayerPhoto(Master::msPlayerModelName, pos, 1.0f);

	  // インスタンス化 ｊｙなくハンドルだけ取得する
	mPhotoHandle = Graph::PlayerPhoto(Master::msPlayerModelName);


	// BGM再生
	//Master::mpSoundManager->PlayBGM(SoundManager::BGM_TITLE);
}


void SelectWeapon::Update()
{
	// intをboolに変換して揃えて押されたら絶対に返すようにする　1118
	bool KEY_ENTER_Down = InputManager::CheckDownKey(KEY_INPUT_RETURN) != 0;
	bool PAD_B_Down = InputManager::CheckDownPad(XINPUT_BUTTON_B) != 0;

	
	// おおはばなへんこうした
	// エンターキーが押されたらゲーム起動画面へ移動
	if (KEY_ENTER_Down || PAD_B_Down)
	{
		// SE再生
		Master::mpSoundManager->PlaySE(SoundManager::SE_ENTER);

		auto wp = GameManager::GetInstance();
		// 両方決定済みならENTERで移動する
		if (mbSelectFinished)
		{
			mbRoadFlag = true;
			mbBgmStop = true; // BGMをとめるかをtrueにする
			Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::LOADING_SCENE_3D);
			return; // 選択されないようにする
		}

		// sub選択中で main と同じ
		if (!mbSelectMain && mnSelect == (int)wp->WeaponSelect.mainWeapon)
		{
			// errorメッセを表示するためのフラグ
			mbErrorMesse = true;
		}
		// メイン武器選択
		else if (mbSelectMain)
		{
			wp->WeaponSelect.mainWeapon = (WeaponManager::WeaponID)mnSelect;
			mbSelectMain = false; // 次は sub
		}
		else
		{
		// サブ武器選択
			wp->WeaponSelect.subWeapon = (WeaponManager::WeaponID)mnSelect;
			mbSelectFinished = true; 
		}
	}



	// ここだけキー入力をやっている
	static XINPUT_STATE prevInput = {}; // 前回の入力を保存（staticで維持）
	XINPUT_STATE input;
	GetJoypadXInputState(DX_INPUT_PAD1, &input);

	// intをboolに変換して揃えて押されたら絶対に返すようにする　1118
	bool BACK = InputManager::CheckDownKey(KEY_INPUT_BACK) != 0;
	bool PAD_BACK = InputManager::CheckDownPad(XINPUT_BUTTON_A) != 0;
	
	//プレイヤー選択画面に戻る
	if (BACK || PAD_BACK)
	{
		// SE再生
		Master::mpSoundManager->PlaySE(SoundManager::SE_BACKSPACE, 200); // 大きめ

		if (mbSelectFinished)
		{
			// 両方埋まっているなら サブ武器の選択を解除する
			mbSelectFinished = false;
			mbSelectMain = false; // サブ武器選択状態
		}
		else if (!mbSelectMain)
		{
			// サブ武器を選択中 メインだけ埋まっているならメイン武器の選択を解除する
			mbSelectMain = true; // メイン武器選択状 態
		}
		else
		{
			// メイン武器選択中ならプレイヤー選択画面へ戻る
			mbRoadFlag = true;
			Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::SELECT_SCENE_3D);
		}
		return; // 戻る処理をした場合は以降の更新をスキップ
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

	if (mnSelect != mnChengeSlect)
	{
		Master::mpSoundManager->PlaySE(SoundManager::SE_SELECT, 130); // 選択音 ちいさめ
		mnChengeSlect = mnSelect; // 更新
	}


	WeaponDraw(); // ここで呼ばないと位置が一瞬バグるから変えない


	// ちかちかタイマー
	mCurrectPhoto += 10.0f;
	if (mCurrectPhoto > 300.0f)
	{
		mFlushTimer = !mFlushTimer;
		mCurrectPhoto = 0.0f;
	}

	// 色を変えるタイマー
	mfSelectCount+= 10.0f;
	if (mfSelectCount > 300.0f)
	{
		mbSelectNow = !mbSelectNow;
		mfSelectCount = 0.0f;
	}

	// errorメッセを秒でとじるため
	if (mbErrorMesse)
	{
		mfErrorCount--;
		if (mfErrorCount < 0.0f)
		{
			mbErrorMesse = false;
			mfErrorCount = ERROR_COUNT;
		}
	}

	// 基底クラスの更新処理を呼びだす
	Scene::Update();
}

void SelectWeapon::WeaponDraw()
{
	// ここで表示するプレイヤーを切り換えている
	switch (mnSelect)
	{
	case 1:
		mpDrawWeapon->SetDrawPlayer(DrawWeapon::Shild);
		break;

	case 2:
		mpDrawWeapon->SetDrawPlayer(DrawWeapon::WriteGun);
		break;

	case 3:
		mpDrawWeapon->SetDrawPlayer(DrawWeapon::BlackGun);
		break;

	case 4:
		mpDrawWeapon->SetDrawPlayer(DrawWeapon::BlueGun);
		break;

	case 5:
		mpDrawWeapon->SetDrawPlayer(DrawWeapon::YARI);
		break;

	case 6:
		mpDrawWeapon->SetDrawPlayer(DrawWeapon::GT);
		break;

	default:
		mpDrawWeapon->SetDrawPlayer(DrawWeapon::Shild);
		break;
	}
}

void SelectWeapon::Draw()
{
	// 先に２Dを書く
	// ２D用に設定
	SetUseZBufferFlag(FALSE);
	SetWriteZBufferFlag(FALSE);

	DrawGraph(0, 0, mnHandle_Sky, TRUE);
	Draw2D(); // 背景とかを描画
	SelectPlayer();

	// ２D用に設定
	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);


	// 基底クラスの更新処理を呼びだす
	Scene::Draw();

	DrawMesseage(); // 呼び出し
}


void SelectWeapon::DrawMesseage()
{
	// errorメッセージはいちばん上に表示したいから
	SetUseZBufferFlag(FALSE);
	SetWriteZBufferFlag(FALSE);

	// えらー
	if (mbErrorMesse)
	{
		// 表示する黒い画像　透過度低め
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120); // 透明度をさげた
		DrawGraph(0, 0, mnHandle_ErrorBag, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// エラーメッセージの後ろの枠
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 230); // 透明度をさげた
		DrawGraph(0, 0, mnHandle_Error, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// エラーメッセージ表示
		DrawFormatStringToHandle(
			Utility::SCREEN_WIDTH / 2 - 250,
			Utility::SCREEN_HEIGHT / 2 -50,
			GetColor(255, 255, 255),
			mnFontNormal,
			"同じ武器は選択できません\n %d秒後自動で閉じます",
			(int)mfErrorCount / 60 + 1
		);
	}

	// ２D用に設定
	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);
}

// 選択しているプレイヤーの表示
void SelectWeapon::SelectPlayer()
{
	// 証明写真の後ろに表示しているやつ
	DrawModiGraph(
		50, 680,  // 左上
		650, 680,  // 右上
		650, 1250,  // 右下
		50, 1250,  // 左下
		mnBoxHandle,
		TRUE
	);
	// 選択画面は選択中player　みたいな感じで、ふつうに左下に出す
	// しょうめいしゃしんの後ろの四角
	DrawBox(116,  808,  264, 1008, GetColor(255,255,255), TRUE);
	// 選択したプレイヤー
	DrawRotaGraph(190, 906, 1.0f, 0.0f, mPhotoHandle, TRUE);

	// ステータスとかの表示したい
	if (Master::mpNextStatus)
	{
		// 選択されたHPとかを表示している
		DrawFormatStringToHandle(
			300,
			797,
			GetColor(255, 255, 255),
			mnPlayerFont,
			"選択中プレイヤー\n\n"
			"NAME      %s\n"
			"HP        %d\n"
			"Boost     %d\n"
			"Speed     %d\n"
			"MaxSpeed  %d",
			Master::mpNextStatus->GetName().c_str(), // c_strつけないと文字化けする
			(int)Master::mpNextStatus->Get_Base_Hp(),
			(int)Master::mpNextStatus->Get_Base_Boost(),
			(int)Master::mpNextStatus->Get_Base_Speed(),
			(int)Master::mpNextStatus->Get_Base_MaxSpeed()
		);
	}
}

void SelectWeapon::Draw2D()
{
	// 色を変えている
	unsigned int Cr2;
	Cr2 = GetColor(255, 255, 255); // 白
	//スケール設定描画
	DrawRotaGraph(1350, 470, 1.0f, 0.0f, mnHandle_Bag, TRUE);
	// 選択した武器の枠
	DrawGraph(0, 0, mnHandle_SelectWeapon, TRUE);


	// 選択中かを見ている
	if (!mbSelectFinished)
	{
		// 選択中武器の文字表示
		DrawFormatStringToHandle(
			50,
			30,
			GetColor(255, 255, 255),
			mnFontNormal,
			mbSelectMain ? "メイン武器 を選択中" : "サブ武器 を選択中"
		);
	}
	else
	{
		// 選択中武器の文字表示
		DrawFormatStringToHandle(
			50,
			30,
			GetColor(255, 255, 255),
			mnFontNormal,
			"Enter / Bボタンでゲーム開始"
		);
	}

	std::string Guide;
	if (mbSelectFinished)
	{
		Guide = "[BackSpace / A] で サブ武器 を選び直す";
	}
	else if (!mbSelectMain)
	{
		Guide = "[BackSpace / A] で メイン武器 を選び直す";
	}
	else
	{
		Guide = "[BackSpace / A] で プレイヤー選択へ戻る";
	}
	DrawStringToHandle(50, 100, Guide.c_str(), GetColor(255, 255, 255), mnFontSsize);
	

	//WeaponManagerからデータを取得
	// 選択されているやつのデータだけとって変数にいれた
	WeaponManager::WeaponSpec spec = WeaponManager::GetBaseSpec((WeaponManager::WeaponID)mnSelect);

	// 名前 攻撃力　弾数を取得している intに直してる
	std::string SelectName = spec.name;
	int SelectAttack = (int)spec.attack;
	int SelectBullet = (int)spec.bulletMax;
	std::string SelectMemo = spec.memo;




	// 選択した武器の名前の表示 0209 ついか
	{
		auto wp = GameManager::GetInstance();

		// 位置
		int Xbox = 930;     // メインのボックス
		int XSubBox = 1370; // サブのボックス
		int YBox = 750;
		// 幅
		int XWidht = 400;
		int YHeight = 60;

		// BOXとかの色
		int color; // 選んでいる方の意色
		int Waku = GetColor(255, 255, 255); // 枠の色
		int color2 = GetColor(150, 150, 150); // 選択していない方のやつ
		int FinisheColor = GetColor(140, 140, 140); // ぜんぶ選択された

		// 選択中武器の文字表示
		DrawFormatStringToHandle(
			Xbox,
			YBox - 110,
			GetColor(255, 255, 255),
			mnFontNormal,
			"選択中武器"
		);

		// 名前は少し大きめ
		DrawFormatStringToHandle(
			Xbox,
			YBox - 30,
			GetColor(255, 255, 255),
			mnFontSmall,
			"メイン武器                        サブ武器"
		);

		// 点滅させたい
		if (mbSelectNow)
		{
			color = GetColor(255, 180, 180);
		}
		else
		{
			color = GetColor(255, 144, 144);
		}

		// 選択された奴の後ろに出るBOXの色を変える
		if (mbSelectMain)
		{
			DrawBox(Xbox, YBox, Xbox + XWidht, YBox + YHeight, color, TRUE);
			DrawBox(XSubBox, YBox, XSubBox + XWidht, YBox + YHeight, color2, TRUE);

			// 選択中武器の文字表示
			DrawFormatStringToHandle(
				Xbox + 110,
				YBox +5,
				GetColor(255, 255, 255),
				mnFontNormal,
				"選択中"
			);
		}
		else if(!mbSelectMain && !mbSelectFinished) // サブを選択中なら

		{
			DrawBox(Xbox, YBox, Xbox + XWidht, YBox + YHeight, color2, TRUE);
			DrawBox(XSubBox, YBox, XSubBox + XWidht, YBox + YHeight, color, TRUE);

			// 選択中武器の文字表示
			DrawFormatStringToHandle(
				XSubBox + 110,
				YBox + 5,
				GetColor(255, 255, 255),
				mnFontNormal,
				"選択中"
			);
		}
		else if (mbSelectFinished) // 両方選択されたら明るくする
		{
			DrawBox(Xbox, YBox, Xbox + XWidht, YBox + YHeight, FinisheColor, TRUE);
			DrawBox(XSubBox, YBox, XSubBox + XWidht, YBox + YHeight, FinisheColor, TRUE);
		}

		// 白い枠
		DrawBox(Xbox, YBox, Xbox + XWidht, YBox + YHeight, Waku, FALSE);
		DrawBox(XSubBox, YBox, XSubBox + XWidht, YBox + YHeight, Waku, FALSE);



		int NameSpaceX = 100; // 名前の為にずらすやつX
		int NomeSpaceY = 10; // Y
		int StringColor = GetColor(255, 255, 255);
		auto mainSpec = WeaponManager::GetBaseSpec((WeaponManager::WeaponID)wp->WeaponSelect.mainWeapon);
		auto subSpac = WeaponManager::GetBaseSpec((WeaponManager::WeaponID)wp->WeaponSelect.subWeapon);

		// 名前の表示
		if (!mbSelectMain || mbSelectFinished)
		{
			DrawFormatString(Xbox + NameSpaceX, YBox + NomeSpaceY, StringColor, "%s", mainSpec.name.c_str());
		}

		if (mbSelectFinished)
		{
			DrawFormatString(XSubBox + NameSpaceX, YBox + NomeSpaceY, StringColor, "%s", subSpac.name.c_str());
		}
	}

	// 二重になっちゃうからなし
	// int Attack[] = { 0, 10, 25, 100 };
	// int Bullet[] = { 0, 50, 30, 5 };



	



	int MaxState = 2; // BOSとかの描画最大数
	float x = (float)Utility::SCREEN_WIDTH / 2 + 40;
	float y = (float)Utility::SCREEN_HEIGHT / 2 - 330;

	float stringY = y + 130;
	// ---------ステータス系--------------------------
	// HP
	DrawFormatStringToHandle((int)x, (int)stringY, GetColor(255, 255, 255), mnFontNormal,
		"ATTACK        %d", SelectAttack);
	// BOOST
	DrawFormatStringToHandle((int)x, (int)stringY + 60, GetColor(255, 255, 255), mnFontNormal,
		"BULLET SIZE   %d", SelectBullet);


	// 名前は少し大きめ
	DrawFormatStringToHandle(
		(int)x,
		(int)y,
		GetColor(255, 255, 255),
		mnFontBig,
		"%s",
		SelectName.c_str()
	);

	// 説明は少し小さめ
	DrawFormatStringToHandle(
		(int)x,
		(int)y + 205,
		GetColor(255, 255, 255),
		mnFontSmall,
		"\n\n\n%s",
		SelectMemo.c_str()
	);


	// いま選択している奴のバーを描画したい
	const int Bx = 1420; // 位置X
	const int By = 345; // 位置Y
	const int BoostY = By + 60; // 位置 BOOST


	const int width = 250; // 横幅
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

		// 枠を太くしたいからBoxにBoxを重ねる
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

			// WeaponManagerから現在のループ番号iのスペックを取得
			auto listSpec = WeaponManager::GetBaseSpec((WeaponManager::WeaponID)mnSelect);


			// 名前は少し大きめ
			DrawFormatStringToHandle(
				30,
				posY + 5,
				GetColor(255, 255, 255),
				mnFontNormal,
				"%s",
				listSpec.name.c_str()
			);
		}
		else // 選択していないやつ
		{
			drawX = -600;
			drawY = posY - 230;
			cx = drawX + imgW / 2;
			cy = drawY + imgH / 2;
			scale = 0.40f;


			// ここにがぞうを用意したい
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
		float rate = (float)SelectAttack / MaxSta;
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
		float boostrate = (float)SelectBullet / MaxSp;
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


	for (int i = 1; i <= MaxState; i++) // 表示するため
	{
		int posY = By + (i - 1) * 60; // 毎回100ずつずらす
		DrawBox(Bx, posY, Bx + width, posY + height, GetColor(255, 255, 255), FALSE);
	}


	float DrawX = Utility::SCREEN_WIDTH / 2 + 400; // この横のを今は使うので変えている もとは半分のやつ
	float DrawY = Utility::SCREEN_HEIGHT / 2 + 300;
	DrawString((int)DrawX - 200, (int)DrawY + 150, "Lスティック または WSキーで選択", Cr2); // のちのち画像かな
	
	if (mFlushTimer)
	{
		if(mbSelectFinished)
		{
			DrawString((int)DrawX - 100, (int)DrawY + 100, "Enterでゲームを始める", Cr2);
		}
		else
		{
			DrawString((int)DrawX, (int)DrawY + 100, "Enterで決定", Cr2);
		}
	}

}


void SelectWeapon::Finalize()
{

	// --- フォントハンドルの削除 ---
	DeleteFontToHandle(mnFontBig);
	DeleteFontToHandle(mnFontNormal);
	DeleteFontToHandle(mnFontSmall);
	DeleteFontToHandle(mnPlayerFont);
	DeleteFontToHandle(mnFontSsize);

	// --- グラフィックハンドルの削除 ---
	DeleteGraph(mnHandle_Bag);
	DeleteGraph(mnBoxHandle);
	DeleteGraph(mnHandle_Select);
	DeleteGraph(mnHandle_No_Select);
	DeleteGraph(mnHandle_Error);
	DeleteGraph(mnHandle_ErrorBag);
	DeleteGraph(mnHandle_SelectWeapon);
	DeleteGraph(mnHandle_Sky);
	DeleteGraph(mPhotoHandle);

	if (mpDrawWeapon != nullptr)
	{
		delete mpDrawWeapon;
	}

	if (mbBgmStop)
	{
		// BGM停止
		Master::mpSoundManager->StopBGM();
	}
}