#include "TutorialScene.h"
#include "Player3D.h"
#include "Wall.h"
#include "Enemy3D.h"
#include "Master.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Floor.h"
#include "Stage.h"
#include "Baria.h"
#include "SkyBox.h"
#include "Loading.h"
#include <memory>
#include "Utility.h"
#include "ModelUtility.h"
#include "MiniMap3D.h"
#include "PlayerAnim.h"
#include "LoseResult3D.h"
#include "GameManager.h"
#include "TutorialManager.h"


TutorialScene::TutorialScene()
	:Scene()
{
	// 文字の大きさ関係
	mnFontBig = CreateFontToHandle(
		NULL,
		300, // すごく大きく
		3
	);


	mnFontLSize = CreateFontToHandle(
		NULL,
		80,   // 小さめのフォントサイズ
		3
	);

	mnFontNormal = CreateFontToHandle(
		NULL,
		45,   // 今のフォントサイズ
		3
	);

	mnFontSmall = CreateFontToHandle(
		NULL,
		35,   // 小さめのフォントサイズ
		3
	);


	// ポーズ画面関係
	mnHandle_PoseBag = LoadGraph("Resource/Graph/ErrorBag.png");
	if (mnHandle_PoseBag == -1)
	{
		printfDx("メッセ画像の読み込みに失敗");
	}

	mnHandle_Pose = LoadGraph("Resource/Option/Graph1.png");
	if (mnHandle_Pose == -1)
	{
		printfDx("メッセ画像の読み込みに失敗");
	}

	mnHandle_PoseSelect = LoadGraph("Resource/Graph/PoseBox.png");
	if (mnHandle_PoseSelect == -1)
	{
		printfDx("メッセ画像の読み込みに失敗");
	}


	fadeout = LoadGraph("Resource/Scene/Black.png");
	if (fadeout == -1)
	{
		printfDx("Black失敗");
	}

	mnHandle_Bag = LoadGraph("Resource/Graph/TutorialGraph.png");
	if (mnHandle_Bag == -1)
	{
		printfDx("枠画像の読み込みに失敗");
	}

	mnSelect = 1; // 初期を１にする
	mnChengeSelect = 1;
}

TutorialScene::~TutorialScene()
{

}

void TutorialScene::Initialize() // 初期化
{
	// 画面サイズと同じサイズの透明なキャンバスを作る
	mnSceneScreen = MakeScreen(Utility::SCREEN_WIDTH, Utility::SCREEN_HEIGHT, FALSE);
	// ここでカメラを元の位置に戻している
	Master::mpCamera->Reset();

	// チュートリアルさん  リセット 初期化
	TutorialManager::GetInstance()->Reset();
	TutorialManager::GetInstance()->StringInitialize();


	// タイトルでもnewしているからいらぬ
	// ゲームマネからintのをもらってやる
	SkyBox* pSkyBox = new SkyBox(GameManager::Resources.skyBoxModel);
	pSkyBox->SetScale(9.0f);


	if (GameManager::Resources.stageModel == -1)
	{
		// まだ誰もロードしていないなら、普通にロードして GameManager に預ける
		GameManager::Resources.stageModel = MV1LoadModel("Resource/NewMap/マップすべて.mqo");
	}
	if (GameManager::Resources.stageCollModel == -1)
	{
		// まだ誰もロードしていないなら、普通にロードして GameManager に預ける
		GameManager::Resources.stageCollModel = MV1LoadModel("Resource/NewMap/マップすべて.mqo");
	}
	//左側のは、あたり判定なしの画像のみ　右側のは、画像なしの判定のみ
	new Stage(GameManager::Resources.stageModel, GameManager::Resources.stageCollModel);




	// チュートリアルのプレイヤーはHero固定
	Master::msPlayerModelName = "Resource/3d/Player/Hero.mv1";
	// ステータスとか入れてる １でHeroになる
	auto spec = Status3D::GetPlayerSpec(1);
	Master::mpNextStatus = std::make_unique<Status3D>(spec);


	// プレイヤーの生成
	auto player = new Player3D(
		"Resource/3d/Player/Hero.mv1",
		VGet(0.0f, 0.0f, 0.0f),
		Master::mpNextStatus.get(),
		false // ここをfalseにする ローカル変数にするといけそ
	);
	player->Initialize(); // 初期化の呼び出し



	// 敵情報の取得 一応
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);
	// １　攻撃型
	// ２　銃撃型
	// ３　逃げる型
	// 位置　HP　BOOST　SPEED MAXSPEED
	new Enemy3D(2, "Resource/3d/TutorialEnemy/TutorialEnemy.mqo", VGet(0.0f, 0.0f, 1000.0f), 4000.0f, 300.0f, 3.0f, 8.0f);


	// 壁の基本設定
	/// SkyBoxとの兼ね合いで6000 / 3000 が良いかも
	const float wallWidth = 6000.0f;   // 幅     // 元々は8000
	const float wallDistance = 3000.0f; // 距離  // 元々は4000
	const float wallHigh = 4500.0f; // 壁の高さ // 元々は300
	const float wallLow = 0.0f; // 壁を地面にめり込ませるために設定 (見た目で空が見えちゃう)

	// 奥
	new Wall(
		"Resource/2d/Wall.png",                 // 画像名
		VGet(0.0f, 0.0f, wallDistance),         // 中心点
		VGet(-wallWidth, wallHigh, wallDistance), // 左上座標
		VGet(wallWidth, wallLow, wallDistance)     // 右下座標
	);
	// 手前
	new Wall(
		"Resource/2d/Wall.png",                 // 画像名
		VGet(0.0f, 0.0f, -wallDistance),        // 中心点
		VGet(wallWidth, wallHigh, -wallDistance), // 左上座標
		VGet(-wallWidth, wallLow, -wallDistance)   // 右下ddwasdd座標
	);
	// 左
	new Wall(
		"Resource/2d/Wall.png",                    // 画像名
		VGet(-wallDistance, 0.0f, 0.0f),           // 中心点
		VGet(-wallDistance, wallHigh, -wallWidth), // 左上座標
		VGet(-wallDistance, wallLow, wallWidth)       // 右下座標
	);
	// 右
	new Wall(
		"Resource/2d/Wall.png",                    // 画像名
		VGet(wallDistance, 0.0f, 0.0f),            // 中心点
		VGet(wallDistance, wallHigh, wallWidth),   // 左上座標
		VGet(wallDistance, wallLow, -wallWidth)       // 右下座標
	);



	// ミニマップ生成 0123
	auto miniMap = new MiniMap3D(VGet(0, 0, 0));
	miniMap->Initialize(256, 800.0f);   // サイズ・高さは好み

	mbDrawFlag = true; // 敵が出たかのフラグをONにする


	// BGM再生
	Master::mpSoundManager->PlayBGM(SoundManager::BGM_TUTORIAL, true, 120); // 少し小さくしてる
}


// 一時停止中の処理とか色々
void TutorialScene::PoseUI()
{
	// ポーズ中の処理
	if (mbPose)
	{
		Master::mpSoundManager->ChangeVolume(80); // 音量変えたい
		Master::mpSoundManager->Change_SE_Volume(SoundManager::SE_BOOST, 50); // 上昇の時の音量でかすぎ音大

		// errorメッセージはいちばん上に表示したいから
		SetUseZBufferFlag(FALSE);
		SetWriteZBufferFlag(FALSE);

		// 表示する黒い画像　透過度低め
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120); // 透明度をさげた
		DrawGraph(0, 0, mnHandle_PoseBag, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


		// ここに背景画像 蒼い四角
		DrawRotaGraph(
			Utility::SCREEN_WIDTH / 2,
			Utility::SCREEN_HEIGHT / 2,
			1.0f,
			0.0f,
			mnHandle_Pose,
			TRUE
		);

		// ここで設定した数値をStatusに代入
		// intをboolに変換して揃えて押されたら絶対に返すようにする　1118
		bool KEY_ENTER_Down = InputManager::CheckDownKey(KEY_INPUT_RETURN) != 0;
		bool PAD_B_Down = InputManager::CheckDownPad(XINPUT_BUTTON_B) != 0;
		// 今のキャラ番号を保存しているならそれを使う
		auto spec = Status3D::GetPlayerSpec(GameManager::mnSelectPlayerDate);

		// エンターキーが押されたらゲーム起動画面へ移動
		if (KEY_ENTER_Down || PAD_B_Down)
		{
			Master::mpSoundManager->PlaySE(SoundManager::SE_ENTER); // 決定音

			// ここで設定した数値をStatusに代入
			switch (mnSelect)
			{
			case 1: // ゲームに戻る
				// 下に書いているフラグFALSEだけでオッケーそう
				break;

			case 2:
				// タイトル画面にいく
				Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::TITLE_3D);
				break;

			case 3:
				// スキップして選択画面に行く
				TutorialManager::GetInstance()->Event(TutorialManager::Event_Skip);
				startgame = true;
				mbSkip = true; // スキップをtrue
				break;

			default: // 保険
				break;
			}
			mbPose = false; // フラグをfalse
		}

		// 左の枠
		std::string menu[] =
		{
			"",
			"チュートリアルを再開",
			"   タイトルに戻る",
			"    スキップする"
		};

		int SelectNow = mnSelect;
		int DrawX = 800;
		int DrawString = 720;
		int DrawY = 500;
		int Cr = GetColor(255, 255, 255); // 白色
		for (int i = 1; i <= mnSelectMax; i++)
		{
			int posY = (DrawY - 50) + (i - 1) * 100;
			int StringY = (DrawY - 100) + (i - 1) * 100;
			int DarkColor = GetColor(90, 90, 90); // 選んでないほうの色

			if (i == SelectNow) //選択しているやつだけ明るく　大きく
			{
				// Boxの表示
				DrawRotaGraph(
					DrawX + 150,
					posY - 20,
					1.1f,
					0.0f,
					mnHandle_PoseSelect,
					TRUE
				);
			}
			else // 選択していないやつ
			{
				// Boxの表示
				DrawRotaGraph(
					DrawX + 150,
					posY - 20,
					1.0f,
					0.0f,
					mnHandle_PoseSelect,
					TRUE
				);
			}

			// 選択しているかによって文字を色々変えた
			DrawFormatStringToHandle(
				i == SelectNow ? DrawString : DrawString + 30,
				i == SelectNow ? StringY : StringY + 10,
				i == SelectNow ? Cr : DarkColor,
				i == SelectNow ? mnFontNormal : mnFontSmall,
				"%s",
				menu[i].c_str()
			);
		}

		// 名前は少し大きめ
		DrawFormatStringToHandle(
			DrawX - 100,
			720,
			Cr,
			mnFontSmall,
			"[W/Sキー / Lスティック] で選択\n    [Eenter / B]で決定"
		);





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

		if (mnSelect != mnChengeSelect)
		{
			Master::mpSoundManager->PlaySE(SoundManager::SE_SELECT, 130); // 選択音 ちいさけ
			mnChengeSelect = mnSelect; // 更新
		}


		// ２D用に設定
		SetUseZBufferFlag(TRUE);
		SetWriteZBufferFlag(TRUE);
	}
}


void TutorialScene::Update() // 更新
{
	// 敵が消えたらまた出す
	auto pEnemyList =
		Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);

	if (pEnemyList.empty()) // 敵がいないなら
	{
		new Enemy3D(2, "Resource/3d/TutorialEnemy/TutorialEnemy.mqo", VGet(0.0f, 0.0f, 1000.0f), 400.0f, 300.0f, 3.0f, 8.0f);
	}


	// チュートリアルさん  更新
	TutorialManager::GetInstance()->CountUpdate();

	// player情報の取得
	auto pPlayerList =
		Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(Object3D::T_Player3D);


	// 一時停止ボタン 0209ついか
	if (InputManager::CheckDownKey(KEY_INPUT_TAB) || InputManager::CheckDownPad(XINPUT_BUTTON_START))
	{
		mnSelect = 1; // 変ったら１にする
		mbPose = !mbPose;
		for (auto obj : pPlayerList)
		{
			Player3D* pPlayer = dynamic_cast<Player3D*>(obj);
			pPlayer->P_Timer_Reset(); // 停止から戻った時に急に進めないように
		}
	}

	if (mbShowResultText)
	{
		mfResultDisplayTimer -= 1.0f / 60.0f; // タイマーを減らす

		if (mfResultDisplayTimer <= 0.0f)
		{
			startgame = true; // 3秒経ったらフェードアウト開始
		}
	}

	// シーン移動
	if (startgame)
	{
		if (blendscale2 >= 254) // 真っ暗になったら遷移
		{
			if (mbIsVictory)
			{
				Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::SELECT_SCENE_3D);
			}
			else
			{
				Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::SELECT_SCENE_3D);
			}
		}
	}

	// ポーズ画面じゃないなら動かす
	// タイマーはとめたいから下の部分mifにいれてる
	if (!mbPose)
	{
		Master::mpSoundManager->ChangeVolume(160); // 音量戻す
		GameOver(); // 呼び出し
		Master::mpCamera->SetCountdown(false);
		Scene::Update();
	}
	else
	{
		// カメラのセッターをTRUE 動かせないように
		Master::mpCamera->SetCountdown(true);
	}

}

void TutorialScene::Draw() // 描画
{
	// ブラー処理関係
	SetDrawScreen(mnSceneScreen);
	ClearDrawScreen(); // キャンバスを綺麗に

	// カメラの位置の設定
	SetCameraPositionAndTarget_UpVecY(
		VAdd(Master::mpCamera->GetPosition(), Master::mpCamera->GetShakePosition()),
		VAdd(Master::mpCamera->GetLookAtPosition(), Master::mpCamera->GetShakePosition())
	);


	Scene::Draw();
	Master::mpCamera->DrawCameraUI(); // ロックオン関係のマークを呼び出し

	SetWriteZBufferFlag(false);
	DrawEnemyUI(); // 敵のバー系描画　ちかちかしなくなった神
	SetWriteZBufferFlag(true);

	BariaDraw(); // バリaの描画呼び出し



	// ブラー処理関係
	SetDrawScreen(DX_SCREEN_BACK);
	bool isDashing = false;
	bool isDashFly = false;
	bool isOverHeat = false;
	VECTOR playerPos = VGet(0.0f, 0.0f, 0.0f);

	// ダッシュ中ならブラーをかける、そうでないならそのまま貼り付ける
	if (isDashing || isDashFly || isOverHeat)
	{
		DrawWithBlur(playerPos);
	}
	else
	{
		DrawGraph(0, 0, mnSceneScreen, FALSE);
	}


	// ２D用に設定
	SetUseZBufferFlag(FALSE);
	SetWriteZBufferFlag(FALSE);

	DrawOllUI(); // UIを描画 

	// ミニマップに風エフェクトが重なってほしくないから場所を変えた
	// ３D用の戻す
	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);

	DrawMap();   // ミニマップを描画 3D

	// ２D用に設定
	SetUseZBufferFlag(FALSE);
	SetWriteZBufferFlag(FALSE);

	TutorialUI(); // チュートリアル中の文字の描画呼び出し

	// 文字の表示
	if (mbShowResultText)
	{
		DrawFormatStringToHandle(Utility::SCREEN_WIDTH / 2 - 400,
			Utility::SCREEN_HEIGHT / 2 - 100,
			GetColor(255, 215, 0),
			mnFontLSize,
			"  チュートリアルクリア\n"
			"プレイヤー選択に移動します");
	}


	// ここで暗くしていく
	if (startgame)
	{
		blendscale2 += 3.0f;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)blendscale2);
		DrawGraph(0, 0, fadeout, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	PoseUI(); //　ポーズ画面のUI呼び出し

	if (blendscale2 >= 254) // 真っ暗になった　または、スキップONなら遷移
	{
		DrawFormatStringToHandle(1400,
			900,
			GetColor(255, 255, 255),
			mnFontNormal,
			"モデル ロード中");
	}

	// ３D用の戻す
	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);


	mbDrawOK = true; // 描画されたかをON
}


void TutorialScene::TutorialUI()
{
	if (mbDrawFlag && mbDrawOK)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 245);
		DrawGraph(0, 0, mnHandle_Bag, TRUE); // 枠
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawFormatStringToHandle(
			50,
			50,
			GetColor(255, 255, 255),
			mnFontNormal,
			"チュートリアル中\n"
			"tab / STARTでポーズ"
			);

		// チュートリアルの描画の呼び出し
		TutorialManager::GetInstance()->DrawEvent();
	}

}


void TutorialScene::DrawOllUI()
{
	auto pPlayerList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Player3D);

	for (auto obj : pPlayerList)
	{
		Player3D* pPlayer = dynamic_cast<Player3D*>(obj);

		pPlayer->DrawPlayerAction(); // ダッシュの集中線
		pPlayer->OverHeatUI(); // OHノイズ
		pPlayer->DamageUI(); // ダメージUI
		pPlayer->DrawBariaUI(); // バリアUI
		pPlayer->RockOnUI(); // ロックオンのUI
		pPlayer->CloudUI(); // 雲のUI

		// ここからは隠れちゃいけんUI
		pPlayer->DrawUI(); // 描画するものを取得
		//pPlayer->DrawTimer(); // タイマーの描画

	}
}

void TutorialScene::BariaDraw()
{
	auto pPlayerList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Player3D);
	for (auto obj : pPlayerList)
	{
		Player3D* pPlayer = dynamic_cast<Player3D*>(obj);
		pPlayer->BariaDraw(); // バリアの描画を呼びだしている
		pPlayer->DrawTrail3D(); // 軌跡の呼び出し

	}
}

void TutorialScene::DrawMap()
{
	auto pMapList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Map3D);

	for (auto obj : pMapList)
	{
		MiniMap3D* pMap = dynamic_cast<MiniMap3D*>(obj);
		pMap->Draw(Utility::SCREEN_WIDTH - 30, Utility::SCREEN_HEIGHT);
	}
}

void TutorialScene::DrawEnemyUI()
{
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Enemy3D);

	for (auto obj : pEnemyList)
	{
		Enemy3D* pEnemy = dynamic_cast<Enemy3D*>(obj);
		pEnemy->DrawEnemyUI(); // 描画するものを取得
		pEnemy->DrawEnemyTrail3D();	// 軌跡の呼び出し
	}
}

void TutorialScene::GameOver()
{
	if (mbPose) return; // ポーズ中にゲームオーバーさせないため

	// チュートリアルさん   完了フラグを取得
	bool isTutorialEnd = TutorialManager::GetInstance()->GetDate().isComplete;

	// リザルト画面なら判定しないように スキップの時は出さない
	if (mbDrawFlag && !mbShowResultText && !mbSkip)
	{
		// チュートリアルクリアしたか
		if (isTutorialEnd)
		{
			mbShowResultText = true;
			mfResultDisplayTimer = TIMEOVER; // 3秒間文字を出す
			mbIsVictory = true; // 勝ちフラグを立てる
		}
		
	}
}


void TutorialScene::DrawWithBlur(VECTOR playerWorldPos)
{
	DrawGraph(0, 0, mnSceneScreen, FALSE);

	float centerX = Utility::SCREEN_WIDTH * 0.5f;
	float centerY = Utility::SCREEN_HEIGHT * 0.5f;

	const int blurCount = 8;
	const float blurStep = 0.003f;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 30);

	for (int i = 0; i < blurCount; ++i)
	{
		float scale = 1.0f + (i + 1) * blurStep;

		float w = Utility::SCREEN_WIDTH * scale;
		float h = Utility::SCREEN_HEIGHT * scale;

		float x1 = centerX - w * 0.5f;
		float y1 = centerY - h * 0.5f;
		float x2 = x1 + w;
		float y2 = y1 + h;

		DrawExtendGraph(
			(int)x1,
			(int)y1,
			(int)x2,
			(int)y2,
			mnSceneScreen,
			FALSE
		);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void TutorialScene::Finalize() // 終了処理
{
	// チュートリアルさん 終了
	TutorialManager::GetInstance()->StringFinalize();

	//  フォントハンドルの削除
	DeleteFontToHandle(mnFontBig);
	DeleteFontToHandle(mnFontNormal);
	DeleteFontToHandle(mnFontSmall);
	DeleteFontToHandle(mnFontLSize);


	// 作ったキャンバスを削除する
	if (mnSceneScreen != -1)
	{
		DeleteGraph(mnSceneScreen);
		mnSceneScreen = -1;
	}

	// 画像の削除　たくさん
	DeleteGraph(mnHandle_PoseBag);
	DeleteGraph(mnHandle_Pose);
	DeleteGraph(mnHandle_PoseSelect);
	DeleteGraph(fadeout);

	Master::mpSoundManager->StopSE(SoundManager::SE_BOOST); // BOOST音とめる
	Master::mpSoundManager->StopSE(SoundManager::SE_WIND); //WIND音とめる
	Master::mpSoundManager->StopSE(SoundManager::SE_OH); // OHも一応とめる


	auto pMapList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Map3D);

	for (auto obj : pMapList)
	{
		MiniMap3D* pMap = dynamic_cast<MiniMap3D*>(obj);
		pMap->Finalize();
	}

	// BGM停止
	Master::mpSoundManager->StopBGM();
}
