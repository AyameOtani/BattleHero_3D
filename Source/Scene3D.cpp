#include "Scene3D.h"
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


Scene3D::Scene3D()
	:Scene()
{
	// 文字の大きさ関係
	mnFontBig = CreateFontToHandle(
		NULL,
		300, // すごく大きく
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

	// 時間切れ画像関係
	mnTimeUpHandle = LoadGraph("Resource/2d/TimeUp.png");
	if (mnTimeUpHandle == -1)
	{
		printfDx("時間切れの画像がない");
	}
	mnHandle_TimeUpUI = LoadGraph("Resource/2d/TimeUpUI.png");
	if (mnHandle_TimeUpUI == -1)
	{
		printfDx("時間切れの画像がない");
	}
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

	// カウントダウン関係
	mnHandle_Count1 = LoadGraph("Resource/Scene/Count1.png");
	if (mnHandle_Count1 == -1)
	{
		printfDx("カウント画像の読み込みに失敗");
	}

	mnHandle_CountUI1 = LoadGraph("Resource/Scene/CountUI1.png");
	if (mnHandle_CountUI1 == -1)
	{
		printfDx("カウント画像の読み込みに失敗");
	}
	mnHandle_Count2 = LoadGraph("Resource/Scene/Count2.png");
	if (mnHandle_Count2 == -1)
	{
		printfDx("カウント画像の読み込みに失敗");
	}

	mnHandle_CountUI2 = LoadGraph("Resource/Scene/CountUI2.png");
	if (mnHandle_CountUI2 == -1)
	{
		printfDx("カウント画像の読み込みに失敗");
	}


	// GOの画像
	mnHandle_GO = LoadGraph("Resource/Scene/GO.png");
	if (mnHandle_GO == -1)
	{
		printfDx("GO失敗");
	}

	mnHandle_BlackBag = LoadGraph("Resource/Scene/Black.png");
	if (mnHandle_BlackBag == -1)
	{
		printfDx("Black失敗");
	}

	mnHandle_Sousa = LoadGraph("Resource/Manual/Sousa.png");
	if (mnHandle_Sousa == -1)
	{
		printfDx("操作画像失敗");
	}

	mnSelect = 1; // 初期を１にする
	mnChengeSelect = 1;
}

Scene3D::~Scene3D()
{

}

void Scene3D::Initialize() // 初期化
{
	// 画面サイズと同じサイズの透明なキャンバスを作る
	mnSceneScreen = MakeScreen(Utility::SCREEN_WIDTH, Utility::SCREEN_HEIGHT, FALSE);

	Master::mpCamera->Reset();
	GameManager::mnKillEnemyCount = 0;

	// ゲームマネからintのをもらってやる
	SkyBox* pSkyBox = new SkyBox(GameManager::Resources.skyBoxModel);
	pSkyBox->SetScale(9.0f);

	//左側のは、あたり判定なしの画像のみ　右側のは、画像なしの判定のみ
	new Stage(GameManager::Resources.stageModel, GameManager::Resources.stageCollModel);


	// flag  ヒーローじゃなかったらPlayeranimで別のを読み込むため
	bool Flag = false;
	if (Master::msPlayerModelName != "Resource/3d/Player/Hero.mv1")
	{
		Flag = true;
	}

	// 分割アニメーションの使い方の参考
	// プレイヤーの生成
	auto player = new Player3D(
		GameManager::Resources.playerModel,
		VGet(0.0f, 0.0f, 0.0f),
		Master::mpNextStatus.get(),
		Flag // ここをfalseにする ローカル変数にするといけそ
	);

	if (Flag)
	{
		// 対応するファイルのモデルをロード
		PlayerAnim::Load(
			player, // プレイヤー
			Master::msPlayerModelName // マスターに入っているパスで変える
		);
	}

	// ここでタイマーを呼び出している
	player->P_TimerInitialize();
	player->Initialize(); // 初期化の呼び出し


	// 敵情報の取得 一応
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);
	//左側のは、あたり判定なしの画像のみ　右側のは、画像なしの判定のみ
	// １　攻撃型
	// ２　銃撃型
	// ３　逃げる型
	// 位置　HP　BOOST　SPEED MAXSPEED
	new Enemy3D(1, "Resource/3d/Player/Hero.mv1", VGet(4000.0f, 0.0f, -3000.0f), 2920.0f, 500.0f, 7.2f, 18.0f);
	new Enemy3D(2, "Resource/3d/Player/Hero.mv1", VGet(-1000.0f, 0.0f, 4200.0f), 3280.0f, 300.0f, 6.0f, 15.0f);
	new Enemy3D(2, "Resource/3d/Player/Hero.mv1", VGet(-4000.0f, 0.0f, -3000.0f),3260.0f, 300.0f, 6.0f, 15.0f);
	new Enemy3D(3, "Resource/3d/Player/Hero.mv1", VGet(5000.0f, 0.0f, 5000.0f),  2460.0f, 600.0f, 6.3f, 16.0f);


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
	//mbStartCount = true;   // カウント開始

	mbStartCount = false;  // 最初はfalse
	mfFadeAlpha = 255.0f;  // 最初は真っ黒
	mbFadeIn = false;
}


// 一時停止中の処理とか色々
void Scene3D::PoseUI()
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
		if (KEY_ENTER_Down || PAD_B_Down && !mbSousa)
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

				Master::mpNextStatus = std::make_unique<Status3D>(spec);
				// 一回ローディング画面にいく リスタート
				Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::LOADING_SCENE_3D);
				break;

			case 4:
				mbSousa = true;
				break;

			default: // 保険
				break;
			}

			// 操作の時は閉じない
			if (!mbSousa)
			{
				mbPose = false; // フラグをfalse
			}
		}


		// 戻るが押されたら
		bool BACK = InputManager::CheckDownKey(KEY_INPUT_BACK) != 0;
		bool PAD_BACK = InputManager::CheckDownPad(XINPUT_BUTTON_BACK) != 0;
		if (BACK || PAD_BACK)
		{
			if (mbSousa)
			{
				mbSousa = false; // 説明画面を閉じる
			}
		}



		// 左の枠
		std::string menu[] =
		{
			"",
			"ゲームを再開",
			"タイトルに戻る",
			" リスタート",
			"  操作説明"
		};

		int SelectNow = mnSelect;
		int DrawX = 800;
		int DrawY = 400;
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
					0.9f,
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
					0.8f,
					0.0f,
					mnHandle_PoseSelect,
					TRUE
				);
			}

			// 選択しているかによって文字を色々変えた
			DrawFormatStringToHandle(
				i == SelectNow ? DrawX : DrawX + 30,
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

		if (!mbSousa) // 操作説明画面だったら変えられないように
		{
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
		}

		// スティックが中央に戻ったらフラグのリセット
		if (StickY < StickZone && StickY > -StickZone)
		{
			mbStickInput = false; // ここでfalseにしてまた入力出来るように
		}

		if (mnSelect != mnChengeSelect)
		{
			Master::mpSoundManager->PlaySE(SoundManager::SE_SELECT, 110); // 選択音 ちいさけ
			mnChengeSelect = mnSelect; // 更新
		}

		// ２D用に設定
		SetUseZBufferFlag(TRUE);
		SetWriteZBufferFlag(TRUE);
	}
	
}


void Scene3D::Update() // 更新
{
	// player情報の取得
	auto pPlayerList =
		Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(Object3D::T_Player3D);

	// 敵情報の取得
	auto pEnemyList =
		Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);

	// ずっと手に入れたいからUPDに書いている
	if (!pPlayerList.empty())
	{
		// プレイヤーが存在するならHPを更新
		for (auto obj : pPlayerList)
		{
			Player3D* pPlayer = dynamic_cast<Player3D*>(obj);
			if (pPlayer)
			{
				GameManager::mnLastPlayerHp = (int)pPlayer->GetHp();
			}
		}
	}
	else
	{
		// 死んだら０にする
		GameManager::mnLastPlayerHp = 0;
	}



	// 一時停止ボタン 0209ついか 操作中じゃなかったら
	if ((InputManager::CheckDownKey(KEY_INPUT_TAB) || InputManager::CheckDownPad(XINPUT_BUTTON_START)) && !mbSousa)
	{
		mnSelect = 1; // 変ったら１にする
		mbPose = !mbPose;
		for (auto obj : pPlayerList)
		{
			Player3D* pPlayer = dynamic_cast<Player3D*>(obj);
			pPlayer->P_Timer_Reset(); // 停止から戻った時に急に進めないように
		}
	}

	// フェードイン
	if (!mbFadeIn && mbDrawOK)
	{
		mfFadeAlpha -= 1.3f; // 明るくなる

		if (mfFadeAlpha <= 150.0f)
		{
			if (!mbDoorSe)	 // ドアの閉めるSE
			{
				Master::mpSoundManager->PlaySE(SoundManager::SE_DOOR_CLOSE, 120); // ドア開ける
				mbDoorSe = true;
			}

			mfGraphX_L += 50;
			mfGraphX_R -= 50;
			if (mfGraphX_L > 0) mfGraphX_L = 0;
			if (mfGraphX_R < 0) mfGraphX_R = 0;

		}
		if (mfFadeAlpha <= 0.0f)
		{
			mfFadeAlpha = 0.0f;
			mbFadeIn = true; // フェード完了
			mbStartCount = true;   // カウントダウンを開始
		}
		Master::mpCamera->SetCountdown(true); // フラグリセットで動かせない

		for (auto obj : pPlayerList)
		{
			Player3D* pPlayer = dynamic_cast<Player3D*>(obj);
			pPlayer->SetGameStart(false); // ゲームを始めないように
		}

		return; // フェード中は以下のカウント処理やゲーム処理をさせない
	}


	// 一時停止していないなら処理をする
	//  開始カウントtrue　かつ　描画されたら
	if (mbStartCount && mbDrawOK)
	{
		if (mbPose) return; // ポーズ中にカウントを減らさないように
		if (!pPlayerList.empty())
		{
			Player3D* pPlayer =
				dynamic_cast<Player3D*>(pPlayerList.front());
			// カメラのセッターをTRUE 動かせないように
			Master::mpCamera->SetCountdown(true);
			mfCount -= 1.0f / 60.0f; // 減らす

			if (mfCount <= 0.90f && !mbTimeStart)
			{
				mbTimeStart = true; // タイマーをUPDATEしたいからtreu

				pPlayer->P_Timer_Reset(); // タイマーをリセット
				Master::mpCamera->SetCountdown(false); // フラグリセットで動かせるように
				pPlayer->SetGameStart(true); // ゲーム初めて良いフラグON


				// BGM開始
				if (!mbBGMStarted)
				{
					// ２分の１で違う曲が流れる
					if (GetRand(1) == 0)
					{
						Master::mpSoundManager->PlayBGM(SoundManager::BGM_GAME, mnVolume);
					}
					else
					{
						Master::mpSoundManager->PlayBGM(SoundManager::BGM_GAME_SUB, mnVolume);

					}
					mbBGMStarted = true;
				}
			}

			
		
			// 完全に0になったらカウントダウン終了
			if (mfCount <= 0.0f)
			{
				mfCount = 0.0f;
				mbStartCount = false; // これでUpdateのこのブロックに入らなくなる
			}
		}
		// げーむ更新処理
		if (mfCount > 0.90f)
		{
			return;
		}
	}


	if (mbShowResultText)
	{
		mfResultDisplayTimer -= 1.0f / 60.0f; // タイマーを減らす

		if (mfResultDisplayTimer <= 0.0f)
		{
			mbEndGame = true; // 3秒経ったらフェードアウト開始
		}

		if (mbEndGame)
		{
			if (blendscale2 >= 254) // 真っ暗になったら遷移
			{
				// ここで GameManager に保存する
				GameManager::mnLastTime = mnTime;

				auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);
				if (mbIsVictory)
				{
					Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::WIN_RESULT_3D);
				}
				else
				{
					Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::LOSE_RESULT_3D);
				}
			}
		}

		// 決着がついている間はカメラを固定
		Master::mpCamera->SetCountdown(true);
		return;
	}


	// 時間止める
	if (mbShowResultText && blendscale2 < 254)
	{
		// プレイヤー勝利の時だけUIが描画される
		for (auto obj : pPlayerList)
		{
			Player3D* pPlayer = dynamic_cast<Player3D*>(obj);
			pPlayer->P_Timer_Reset(); // 停止から戻った時に急に進めないように
		}
	}


	// ポーズ画面じゃないなら動かす
	// タイマーはとめたいから下の部分mifにいれてる
	if (!mbPose)
	{
		Master::mpSoundManager->ChangeVolume(mnVolume); // 音量戻す

		// 追加　0120
		for (auto obj : pPlayerList)
		{
			Player3D* pPlayer = dynamic_cast<Player3D*>(obj);
			if (mbDrawFlag && mbTimeStart && !mbStartCount) // 描画されたかつ　スタートがtrueなら
			{
				pPlayer->P_Timer_Update(); // タイマーのアップデートをやる
			}
			// タイマーの情報はずっと取得したいから条件の外にしている
			mnTime = pPlayer->P_GetTimer(); // タイマーの残り秒数の取得
		}

		GameOver(); // 呼び出し

		Master::mpCamera->SetCountdown(false);
		Scene::Update();
	}
	else
	{
		for (auto obj : pPlayerList)
		{
			Player3D* pPlayer = dynamic_cast<Player3D*>(obj);
			pPlayer->P_Timer_Reset(); // 停止から戻った時に急に進めないように
		}
		// カメラのセッターをTRUE 動かせないように
		Master::mpCamera->SetCountdown(true);
	}

}

void Scene3D::Draw() // 描画
{

	// ブラー処理関係
	SetDrawScreen(mnSceneScreen);
	ClearDrawScreen(); // キャンバスを綺麗に

	// カメラの位置を「今」の設定で確定させる
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
	auto pPlayerList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Player3D);
	if (!pPlayerList.empty())
	{
		Player3D* pPlayer = dynamic_cast<Player3D*>(pPlayerList.front());
		isDashing = pPlayer->IsDashing();
		isDashFly = pPlayer->IsDushFlying();
		isOverHeat = pPlayer->IsOverHeat();
		playerPos = pPlayer->GetPosition();
	}


	bool isGameRunning = (!mbStartCount && mbTimeStart);
	if (isGameRunning && (isDashing || isDashFly || isOverHeat))
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


	//ここはまた2D
	if (TimUp && mbShowResultText) // 時間切れになったら画像を出す
	{
		// ここで時間切れ画像を出す
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 230); // 透明度をさげた
		DrawGraph(0, 0, mnHandle_TimeUpUI, TRUE); // 赤いやつ
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawGraph(0, 0, mnTimeUpHandle, TRUE); // 黒枠
	}

	// フェード画像描画
	if (!mbFadeIn && mfFadeAlpha > 0.0f)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)mfFadeAlpha);
		DrawGraph(0, 0, mnHandle_BlackBag, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}


	 // ここでカウントダウンの数字を出している
	 if (mbStartCount || mfFadeAlpha <= 150.0f)
	 {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200); // 透明度をさげた
		DrawGraph(mfGraphX_L, 0, mnHandle_Count1, TRUE); // 緑画像左
		DrawGraph(mfGraphX_R, 0, mnHandle_Count2, TRUE); // 緑画像右
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawGraph(mfGraphX_L, 0, mnHandle_CountUI1, TRUE); //  黒い枠左
		DrawGraph(mfGraphX_R, 0, mnHandle_CountUI2, TRUE); //  黒い枠右

		// ここで３からスタートにしている
		int count = (int)mfCount;
		// 幅を取得
		int stringW = GetDrawFormatStringWidthToHandle(mnFontBig, "%d", count);
		int stringGO = GetDrawFormatStringWidthToHandle(mnFontBig, "GO");
		int DrawY = Utility::SCREEN_HEIGHT / 2 - 160;

		// SE再生
		if (count != mPrevCount)
		{
			if (count <= 3 && count > 0) // 3,2,1 のとき
			{
				Master::mpSoundManager->PlaySE(SoundManager::SE_TIMER, 140); // 決定音
			}
			else if (count == 0) // GO のとき 変えたいけど時間ないない
			{
				Master::mpSoundManager->PlaySE(SoundManager::SE_SELECT_OK, 160); // 決定音
				Master::mpSoundManager->PlaySE(SoundManager::SE_DOOR_OPEN, 200); // ドア開ける

			}

			mPrevCount = count; // 前回カウント更新
		}



		if (count == 0)
		{
			// 0になったら開ける
			mfGraphX_L -= 60;
			mfGraphX_R += 60;
			mfGoSize += 0.25; // Goの大きさあっぷ
			mfAlpha -= 10;

			if (mfAlpha > 0) // 透過度が０以上なら描画
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, mfAlpha);
				DrawRotaGraph(Utility::SCREEN_WIDTH / 2, Utility::SCREEN_HEIGHT / 2, mfGoSize, 0, mnHandle_GO, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}

		}
		else if (count > 0 && count <= 3) // ４も表示されちゃうときがあるから修正
		{
			// 数字を表示
			DrawFormatStringToHandle(
				(Utility::SCREEN_WIDTH - stringW) / 2,
				DrawY,
				GetColor(255, 255, 255),
				mnFontBig,
				"%d",
				count
			);
		}
	 }


	// 文字の表示
	if (mbShowResultText && !TimUp)
	{
		auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);
		const char* text = mbIsVictory ? "VICTORY" : "DEFEAT";
		int color = mbIsVictory ? GetColor(255, 215, 0) : GetColor(255, 0, 0);

		int stringW = GetDrawFormatStringWidthToHandle(mnFontBig, text);
		DrawFormatStringToHandle((Utility::SCREEN_WIDTH - stringW) / 2, Utility::SCREEN_HEIGHT / 2 - 100, color, mnFontBig, text);
	}
	// ここで暗くしていく
	if (mbEndGame)
	{
		blendscale2 += 3.0f;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)blendscale2);
		DrawGraph(0, 0, mnHandle_BlackBag, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	PoseUI(); //　ポーズ画面のUI呼び出し


	// 操作フラグがON
	if (mbSousa)
	{
		// ２D用に設定
		SetUseZBufferFlag(FALSE);
		SetWriteZBufferFlag(FALSE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 250); // 透明度をさげた
		// 説明画像
		DrawRotaGraph(
			Utility::SCREEN_WIDTH / 2,
			Utility::SCREEN_HEIGHT / 2,
			1.0f,
			0.0f,
			mnHandle_Sousa, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // 透明度をさげた
		DrawFormatString(850, 900, GetColor(255, 255, 255), "[BackSpace / BACK] で閉じる");
		// ３D用に設定
		SetUseZBufferFlag(TRUE);
		SetWriteZBufferFlag(TRUE);
	}


	// ３D用の戻す
	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);

	mbDrawOK = true; // 描画されたかをON
}


void Scene3D::DrawOllUI()
{
	auto pPlayerList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Player3D);

	for (auto obj : pPlayerList)
	{
		Player3D* pPlayer = dynamic_cast<Player3D*>(obj);

		pPlayer->CloudUI(); // 雲のUI
		pPlayer->DrawPlayerAction(); // ダッシュの集中線
		pPlayer->OverHeatUI(); // OHノイズ
		pPlayer->DamageUI(); // ダメージUI
		pPlayer->DrawBariaUI(); // バリアUI
		pPlayer->RockOnUI(); // ロックオンのUI


		// ここからは隠れちゃいけんUI
		pPlayer->DrawUI(); // 描画するものを取得
		pPlayer->DrawTimer(); // タイマーの描画

	}
}

void Scene3D::BariaDraw()
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

void Scene3D::DrawMap()
{
	auto pMapList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Map3D);

	for (auto obj : pMapList)
	{
		MiniMap3D* pMap = dynamic_cast<MiniMap3D*>(obj);
		pMap->Draw(Utility::SCREEN_WIDTH - 30, Utility::SCREEN_HEIGHT);
	}
}

void Scene3D::DrawEnemyUI()
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

void Scene3D::GameOver()
{
	if (mbPose) return; // ポーズ中にゲームオーバーさせないため
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);
	auto pPlayerList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Player3D);

	// 時間切れの時に、画面をとめてから時間切れにしたい
	if (mnTime <= 0 && !TimUp && mbDrawFlag) // 画面に写されたかも入れないとすぐ入るから注意
	{
		mbShowResultText = true;
		TimUp = true; // 時間切れフラグをON
		mfResultDisplayTimer = TIMEOVER;
		mbIsVictory = false; // 時間切れは負け
	}

	if (mbDrawFlag && !mbShowResultText) // リザルト画面なら判定しないように
	{
		// 敵 全滅
		if (pEnemyList.empty())
		{
			mbShowResultText = true;
			mfResultDisplayTimer = TIMEOVER; // 3秒間文字を出す
			mbIsVictory = true; // 勝ちフラグを立てる
		}
		// プレイヤー 死亡
		else if (pPlayerList.empty())
		{
			mbShowResultText = true;
			mfResultDisplayTimer = TIMEOVER;
			mbIsVictory = false;
		}
	}
}

//// ダッシュしているときにプレイヤーに向かってずーむ
//void Scene3D::DrawWithBlur(VECTOR playerWorldPos)
//{
//	// まず元画像をそのまま描画（ベース）
//	DrawGraph(0, 0, mnSceneScreen, FALSE);
//
//	// プレイヤーのワールド座標を画面上の座標（2D）に変換
//	VECTOR playerScreenPos = ConvWorldPosToScreenPos(playerWorldPos);
//
//	float centerX = playerScreenPos.x;
//	float centerY = playerScreenPos.y;
//
//	// 画面外にいるときなど、変換に失敗した場合はブラーをかけない等の安全策
//	//if (playerScreenPos.z < 0.0f || playerScreenPos.z > 1.0f) return;
//
//	const int blurCount = 10;     // 重ねる回数
//	const float blurStep = 0.001f; // 拡大率の増分
//
//	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 40);
//
//	for (int i = 0; i < blurCount; ++i)
//	{
//		float scale = 1.0f + (i + 1) * blurStep;
//
//		float w = Utility::SCREEN_WIDTH * scale;
//		float h = Utility::SCREEN_HEIGHT * scale;
//
//		float x1 = centerX - centerX * scale;
//		float y1 = centerY - centerY * scale;
//		float x2 = x1 + w;
//		float y2 = y1 + h;
//
//		DrawExtendGraph(
//			static_cast<int>(x1),
//			static_cast<int>(y1),
//			static_cast<int>(x2),
//			static_cast<int>(y2),
//			mnSceneScreen,
//			FALSE
//		);
//	}
//
//	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
//}



void Scene3D::DrawWithBlur(VECTOR playerWorldPos)
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
			FALSE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}


void Scene3D::Finalize() // 終了処理
{
	//  フォントハンドルの削除
	DeleteFontToHandle(mnFontBig);
	DeleteFontToHandle(mnFontNormal);
	DeleteFontToHandle(mnFontSmall);


	// 作ったキャンバスを削除する
	if (mnSceneScreen != -1)
	{
		DeleteGraph(mnSceneScreen);
		mnSceneScreen = -1;
	}

	// 画像の削除　たくさん
	DeleteGraph(mnTimeUpHandle);
	DeleteGraph(mnHandle_TimeUpUI);
	DeleteGraph(mnHandle_PoseBag);
	DeleteGraph(mnHandle_Pose);
	DeleteGraph(mnHandle_PoseSelect);
	DeleteGraph(mnHandle_Count1);
	DeleteGraph(mnHandle_Count2);
	DeleteGraph(mnHandle_CountUI1);
	DeleteGraph(mnHandle_CountUI2);
	DeleteGraph(mnHandle_GO);
	DeleteGraph(mnHandle_BlackBag);
	DeleteGraph(mnHandle_Sousa);


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

//  MV1SetDifColorScale