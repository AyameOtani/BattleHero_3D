#include "TutorialManager.h"
#include "DxLib.h"
#include "Master.h"

// シングルトンの実体
TutorialManager* TutorialManager::instance = nullptr;


void TutorialManager::Event(EventType type)
{
	if (data.isComplete || data.nextStep > 0) return;  // 終わってるならなんもしない

	// 今のシーンがチュートリアルじゃないならスキップ
	if (Master::mpSceneManager->GetCurrentSceneType() != SceneManager::SCENE_TYPE::TUTORIAL_SCENE_3D) return;


	// チュートリアルスキップ
	if (type == Event_Skip)
	{
		data.step = End;         // ステップを最後まで飛ばす
		data.isComplete = true;  // 完了フラグを立てる
		return;                  // 処理はしない
	}

	nowStep = data.step; // ステップが変ったかを出すため



	// ステップで処理分岐する
	switch (data.step)
	{
	//移動  カメラ
	case Move:
		if (type == Event_Move) data.moveCount += 5; // 移動入力をカウント
		if (type == Event_Camera) data.cameraCount += 5; // 移動入力をカウント

		// 両方達成したらジャンプ
		if (data.moveCount >= 300 && data.cameraCount >= 300)
		{
			data.nextStep = mnCountDownStep;
		}
		break;


	// ジャンプ	 上昇
	case Jump:
		if (type == Event_Jump) data.jumpCount += 1; // ジャンプ数える
		if (type == Event_JumpUp) data.jumpUpCount += 10; // ジャンプカウント

		if (data.jumpCount >= 3 && data.jumpUpCount >= 300)
		{
			data.nextStep = mnCountDownStep;
		}
		break;


	 // 攻撃
	case Attack:
		if (type == Event_Attack && data.mbAttack == false) // 攻撃アニメが終わっていたら
		  data.attackCount += 1;
		  data.mbAttack = true; // 攻撃カウントされたらtrue

		if (type == Event_AttackOff) data.mbAttack = false; // 待機モーションに戻ったらfalse


		if (data.attackCount >= 3 && data.mbAttack == false)
		{
			data.nextStep = mnCountDownStep;
		}
		break;	


	// 銃攻撃　　武器交換
	case Weapon:
		if (type == Event_Bullet) data.bulletAttack += 1; // 弾の数をやる
		if (type == Event_Weapon) data.weaponSelect += 1;

		if (data.bulletAttack >= 3 && data.weaponSelect >= 3)
		{
			data.nextStep = mnCountDownStep;
		}
		break;



	// ロックオン
	case RockOn:
		if (type == Event_RockOn) data.rockOn += 1;
		if (type == Event_RockOnGraph) data.isRockOn = true;

		if (type == Event_RockOff) data.rockOff += 1;
		if (type == Event_RockOffGraph) data.isRockOff = true;


		if(data.rockOn >= 1 && data.rockOff >= 1
			&& data.isRockOff == true && data.isRockOn == true)
		{
			data.nextStep = mnCountDownStep;
		}
		break;


	// バリア
	case Baria:
		if (type == Event_Baria)
			data.baria += 1;
		    data.bariaBreak = true; // はられてる
		if (type == Event_BariaOff) data.bariaBreak = false;

		if (data.baria >= 2 && data.bariaBreak == false)
		{
			// 最後のチュートリアルが終わったらカウントしてから終了
			data.nextStep = mnCountDownStep;
		}
		break;


	default:
		break;
	}


	// switch文の中で data.nextStep に値が入った「瞬間」だけ SE を鳴らす
	if (nowStep == data.step && data.nextStep == mnCountDownStep)
	{
		Master::mpSoundManager->PlaySE(SoundManager::SE_CLEAR, 130);
	}
}


void TutorialManager::CountUpdate()
{
	if (data.nextStep <= 0) return;

	data.nextStep--;

	if (data.nextStep == 0)
	{
		nowStep = data.step; // 変化前を記録

		// 現在のステップに応じて次を決める
		if (data.step == Move)   data.step = Jump;
		else if (data.step == Jump)   data.step = Attack;
		else if (data.step == Attack) data.step = Weapon;
		else if (data.step == Weapon) data.step = RockOn;
		else if (data.step == RockOn) data.step = Baria;
		else if (data.step == Baria)
		{
			data.step = End;
			Master::mpSoundManager->PlaySE(SoundManager::SE_ALL_CLEAR, 140);
			data.isComplete = true;
		}

		

		// ステップが変った カウントアップ
		if (nowStep != data.step)
		{
			data.stepCount += 1;
		}
	}
}


void TutorialManager::DrawEvent()
{
	// 今のシーンがチュートリアルじゃないならスキップ
	if (Master::mpSceneManager->GetCurrentSceneType() != SceneManager::SCENE_TYPE::TUTORIAL_SCENE_3D) return;

	int color = GetColor(255, 255, 255); // 文字色
	int gageColor = GetColor(20, 222, 200); // ゲージ色
	int wakuColor = GetColor(50, 50, 50); // 枠色
	int rookColor = GetColor(208, 75, 75); // ロックオン関係文字色

	int x = 80;
	int y = 400;
	int barWidth = 300; // ゲージの最大幅

	// 表示用のカウント変数を準備
	int drawMoveWidth = data.moveCount;
	int drawCameraWidth = data.cameraCount;
	int drawJumpUpWidth = data.jumpUpCount;
	// 幅を制限
	if (drawMoveWidth > barWidth)   drawMoveWidth = barWidth;
	if (drawCameraWidth > barWidth) drawCameraWidth = barWidth;
	if (drawJumpUpWidth > barWidth) drawJumpUpWidth = barWidth;
	
	// 0未満にならないように
	int drawJump   = (3 - data.jumpCount > 0) ? (3 - data.jumpCount) : 0;
	int drawAttack = (3 - data.attackCount > 0) ? (3 - data.attackCount) : 0;
	int drawBullet = (3 - data.bulletAttack > 0) ? (3 - data.bulletAttack) : 0;
	int drawWeapon = (3 - data.weaponSelect > 0) ? (3 - data.weaponSelect) : 0;
	int drawBaria  = (2 - data.baria > 0) ? (2 - data.baria) : 0;

	// 位置とかやりたい
	int BoxX = 110;
	int Box1 = 80;
	int Box1End = Box1 + 30;
	int Box2 = 250;
	int Box2End = Box2 + 30;
	int gosa = 20; // 誤差
	
	if (data.step != End)
	{
		if (data.nextStep > 0)
		{
			DrawFormatStringToHandle(x + 30, y - 60, GetColor(12, 255, 251), mnFontBig, "STEP %d CLEAR!", data.stepCount);
		}
		else
		{
			DrawFormatStringToHandle(x + 30, y - 60, color, mnFontBig,
				"STEP %d", data.stepCount);
		}
	
	}
	else
	{
		DrawFormatStringToHandle(x + 60, y - 60, color, mnFontBig,
			"STEP  CLEAR");
	}



	// 今のステップに応じて、画面に出す文字を切り替える
	switch (data.step)
	{
	case Move:
		DrawFormatStringToHandle(x, y, color, mnFontNormal,
			"・移動してみよう\n"
			"  [WASD / Lスティック]\n\n\n\n\n"
			"・視点を動かそう\n"
			"  [↑←↓→ / Rスティック]");

		DrawFormatString(BoxX, Box2 + 470, color,
			"左shiftを押しながら移動すると\n"
			"BOOSTを使いダッシュ出来ます");

		// ゲージ移動
		DrawBox(BoxX, y + Box1, BoxX + barWidth, y + Box1End, wakuColor, TRUE); // 背景
		DrawBox(BoxX, y + Box1, BoxX + drawMoveWidth, y + Box1End, gageColor, TRUE); // 進歩
		DrawBox(BoxX, y + Box1, BoxX + barWidth, y + Box1End, color, FALSE); // 枠
		// ゲージカメラ
		DrawBox(BoxX, y + Box2, BoxX + barWidth, y + Box2End, wakuColor, TRUE); // 背景
		DrawBox(BoxX, y + Box2, BoxX + drawCameraWidth, y + Box2End, gageColor, TRUE); // 進歩
		DrawBox(BoxX, y + Box2, BoxX + barWidth, y + Box2End, color, FALSE); // 枠

		// 文字出す
		if (data.moveCount >= barWidth)
		{
			DrawString(BoxX + 100, y + Box1 + 5, "クリア", color);
		}
		if (data.cameraCount >= barWidth)
		{
			DrawString(BoxX + 100, y + Box2 + 5, "クリア", color);
		}
		break;


	case Jump:
		DrawFormatStringToHandle(x, y, color, mnFontNormal,
			"・ジャンプしよう\n"
			"  [SPACE / Bボタン or LT]\n\n"
			"  残り %d回\n\n\n\n\n"
			"・長押しして上昇しよう", drawJump);
		
		// ゲージ
		DrawBox(BoxX, y + Box2 + gosa, BoxX + barWidth, y + Box2End + gosa, wakuColor, TRUE);
		DrawBox(BoxX, y + Box2 + gosa, BoxX + drawJumpUpWidth, y + Box2End + gosa, gageColor, TRUE);
		DrawBox(BoxX, y + Box2 + gosa, BoxX + barWidth, y + Box2End + gosa, color, FALSE);

		if (data.jumpUpCount >= barWidth)
		{
			DrawString(BoxX + 100, y + Box2 + 5 + gosa, "クリア", color);
		}
		break;


	case Attack:

		DrawFormatStringToHandle(x, y, color, mnFontNormal,
			"・攻撃しよう\n"
			"  [C / Yボタン]\n\n\n"
			"  残り %d回 ", drawAttack);
		break;


	case Weapon:

		DrawFormatStringToHandle(x, y, color, mnFontNormal,
			"・銃攻撃をしよう\n"
			"  [F / RT] \n\n"
			"  残り %d回\n\n\n"
			"・武器を変えよう\n"
			"  [E / RB] \n\n"
			"  残り %d回",
			(drawBullet),
			(drawWeapon));

		DrawString(BoxX, y + Box2 + 80, "走り / 待機モーションの時に\n変更できます", GetColor(210, 210, 210));
		break;


	case RockOn:

		DrawFormatStringToHandle(x, y, color, mnFontNormal,
			"・敵をロックオンしよう\n"
			"  [R or 右shift/ R押し込み]\n\n\n"
			"・もう一度押して解除しよう");



		DrawFormatString(BoxX, Box2 + 390, GetColor(210, 210, 210),
			"敵に近づくとマークがでます\n\n"
			"赤 / 緑\n"
			"弾がホーミング  カメラ追跡\n\n"
			"深緑\n"
			"カメラ追跡");


		// ロックオン状態によって出す文字を変える
		if (data.isRockOn)
		{
			// ロックオン中の表示
			DrawString(BoxX, y + Box1 - gosa, " LOCK ON   CLEAR!", rookColor);
		}
		else
		{
			// まだしていない時の表示
			DrawString(BoxX, y + Box1 - gosa, "ターゲットの近くに行こう", GetColor(255, 255, 255));
		}

		if (data.isRockOff)
		{
			// ロックオン中の表示
			DrawString(BoxX, y + Box2 - 90, "LOCK OFF !", rookColor);
		}
		else
		{
			// まだしていない時の表示
			DrawString(BoxX, y + Box2 - 90, "キーを押して解除しよう", GetColor(255, 255, 255));
		}
		break;


	case Baria:
		DrawFormatStringToHandle(x, y, color, mnFontNormal,
			"・バリアを 2回 はろう!\n"
			"  [V / Aボタン]\n\n"
			"  残り %d回", drawBaria);
		break;


	case End:
		DrawFormatStringToHandle(x, y, color, mnFontNormal,
			"  !! Mission Complete !!\n");
		break;


	default:
		break;
	}

	
}


bool TutorialManager::IsAction(EventType type) const
{
	// チュートリアル以外は無条件で解放
	if (data.isComplete) return true;
	if (Master::mpSceneManager->GetCurrentSceneType() != SceneManager::SCENE_TYPE::TUTORIAL_SCENE_3D) return true;
	// スキップは常に許可する場
	if (type == Event_Skip) return true;


	// どこまでみｓｓｙんをお許可するか
	switch (data.step)
	{
	case Move:
		// 移動ステップ中は、移動とカメラだけ
		return (type == Event_Move || type == Event_Camera);

	case Jump:
		// ジャンプステップ中は、移動・カメラに加えてジャンプ系も許可
		return (type <= Event_JumpUp);

	case Attack:
		// 攻撃ステップ中は、攻撃まで許可
		return (type <= Event_AttackOff);

	case Weapon:
		return (type <= Event_Weapon);

	case RockOn:
		return (type <= Event_RockOffGraph);

	case Baria:
		return (type <= Event_BariaOff);

	default:
		return true;
	}
}

// リセットせんとデータ持ったままにある
void TutorialManager::Reset()
{
	data = TutorialData(); // 構造体をまるごと初期化
	data.step = Move;
	data.isComplete = false;
	data.moveCount = 0;
	data.cameraCount = 0;
	data.jumpCount = 0;
	data.jumpUpCount = 0;
	data.attackCount = 0;
	data.bulletAttack = 0;
	data.rockOn = 0;
	data.rockOff = 0;
	data.weaponSelect = 0;
	data.baria = 0;
	data.isRockOn = false;
	data.isRockOff = false;
	data.mbAttack = false;
	data.bariaBreak = false;
	data.stepCount = 1; 
	data.nextStep = 0;
}

// 文字見解
void TutorialManager::StringInitialize()
{
	// 文字の大きさ関係
	mnFontNormal = CreateFontToHandle(
		NULL,
		26,
		3
	);

	// 文字の大きさ関係
	mnFontBig = CreateFontToHandle(
		NULL,
		40,
		3
	);
}

void TutorialManager::StringFinalize()
{
	DeleteFontToHandle(mnFontNormal);
	DeleteFontToHandle(mnFontBig);
}