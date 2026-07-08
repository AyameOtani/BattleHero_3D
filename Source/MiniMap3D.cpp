#include "MiniMap3D.h"
#include "Player3D.h"
#include "Master.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Enemy3D.h"


MiniMap3D::MiniMap3D(VECTOR initPos)
	:Object3D(initPos)
	, mnMiniMapScreen(-1)
	, mnMiniMapSize(0)
	, mnMapCameraHeight(0.0f)
{
	// マップ用のタグ
	SetTag(Object3D::T_Map3D);

	//　画像の読み込み
	mnPlayerHandle = LoadGraph("Resource/2d/PlayerMark.png");
	if (mnPlayerHandle == -1) printfDx("プレイヤー画像ない");
	mnCameraHandle = LoadGraph("Resource/2d/CameraMark.png");
	if (mnCameraHandle == -1) printfDx("カメラ画像ない");
	mnEnemyHandle = LoadGraph("Resource/2d/EnemyMark.png");
	if (mnEnemyHandle == -1) printfDx("敵画像ない");
	
}

MiniMap3D::~MiniMap3D()
{
	Finalize();
}

bool MiniMap3D::Initialize(int mapSize, float mapHeight)
{
	mnMiniMapSize = mapSize;
	mnMapCameraHeight = mapHeight;

	// ミニマップ用のスクリーン作成
	// SetDrawScreen で描画対象にできるグラフィックハンドルを作成する DXから引用
	mnMiniMapScreen = MakeScreen(mnMiniMapSize, mnMiniMapSize, TRUE);
	if (mnMiniMapScreen == -1) // 存在しなかったらのこと
	{
		return false;
	}

	return true;
}

void MiniMap3D::Finalize()
{
	// 終了処理を呼びだす
	if (mnMiniMapScreen != -1)
	{
		DeleteGraph(mnMiniMapScreen);
		mnMiniMapScreen = -1;
	}


	// ロードしたアイコン画像の削除
	if (mnPlayerHandle != -1)
	{
		DeleteGraph(mnPlayerHandle);
		mnPlayerHandle = -1;
	}
	if (mnCameraHandle != -1)
	{
		DeleteGraph(mnCameraHandle);
		mnCameraHandle = -1;
	}
	if (mnEnemyHandle != -1)
	{
		DeleteGraph(mnEnemyHandle);
		mnEnemyHandle = -1;
	}
}

void MiniMap3D::Draw(int screenW, int screenH)
{
	// 存在しなかったら通らないように
	if (mnMiniMapScreen == -1)
	{
		return;
	}

	// プレイヤーの位置取得
	auto pPlayerList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Player3D);
	if (pPlayerList.empty())
	{
		return;
	}
	// 先頭要素への参照
	Player3D* pPlayer = dynamic_cast<Player3D*> (pPlayerList.front());
	VECTOR playerPos = pPlayer->GetPosition(); // 位置取得

	// 敵の位置取得
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);
	if (pEnemyList.empty())
	{
		return;
	}
	

	// メイン用のカメラの保存
	VECTOR oldCameraPos = GetCameraPosition();
	VECTOR oldCameraTarget = GetCameraTarget();
	VECTOR oldCameraUp = GetCameraUpVector();

	// ミニマップを書く
	SetDrawScreen(mnMiniMapScreen);
	ClearDrawScreen();

	// ミニマップ用のカメラの設定
	VECTOR camPos = VGet(playerPos.x, playerPos.y + mnMapCameraHeight, playerPos.z);
	VECTOR camTarget = playerPos; // カメラのターゲットをプレイヤーの位置にしている
	VECTOR camUp = VGet(0.0f, 0.0f, 1.0f); // 上方向ベクトル

	// 真下向きに見るので、"上方向" をZ軸にしてやる
	// （ステージの向きによって 0,0,1 / 0,0,-1 は調整してOK）
	//***　上下逆にしたい場合はこちら　****//
	//VECTOR camUp = VGet(0.0f, 0.0f, -1.0f);

	// ここでセット
	SetCameraPositionAndTargetAndUpVec(camPos, camTarget, camUp);



	// ステージを半透明に
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
	DrawObj(); // 描画したいステージの呼びだし
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// バックバッファに戻す
	// 画面に貼り付ける 右上らへん
	SetDrawScreen(DX_SCREEN_BACK);
	// メインカメラを復元 いちばん重要
	SetCameraPositionAndTargetAndUpVec(oldCameraPos, oldCameraTarget, oldCameraUp);

	// ミニマップを２Dで貼る
	X = screenW - mnMiniMapSize - 10; // 右から 10px 余白
	Y = 10;							  // 上から 10px 余白

	// 縁取り
	DrawBox(
		X - 2,
		Y - 2,
		X + mnMiniMapSize + 2,
		Y + mnMiniMapSize + 2,
		GetColor(0, 0, 0),
		TRUE
	);

	// ミニマップ本体
	DrawGraph(X, Y, mnMiniMapScreen, FALSE);


	Draw2DObj(); // 2Dはマップの上だから跡から呼ぶ

}
void MiniMap3D::DrawObj()
{

	{	// ここでミニマップに描画したいものを書く
		// ミニマップに描画するものをもういちど描画
		// 床モデル
		auto stageList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Stage3D);
		for (auto obj : stageList)
		{
			obj->Draw();
		}
		// 壁モデル
		auto wallList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Wall23D);
		for (auto obj : wallList)
		{
			obj->Draw();
		}
	}
}

void MiniMap3D::Draw2DObj()
{
	// プレイヤーの位置取得
	auto pPlayerList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Player3D);
	if (pPlayerList.empty())
	{
		return;
	}
	// 先頭要素への参照
	Player3D* pPlayer = dynamic_cast<Player3D*> (pPlayerList.front());
	VECTOR playerPos = pPlayer->GetPosition(); // 位置取得
	// 敵の位置取得
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);
	if (pEnemyList.empty())
	{
		return; // いらんかも？
	}

	// プレイヤーの矢印マークを真ん中に表示できるように
	int centerX = X + mnMiniMapSize / 2;
	int centerY = Y + mnMiniMapSize / 2;
	float MapScale = 15.0f; // １５ぐらいがちょうどよさげ

	// 全員を書くため
	for (auto obj : pEnemyList)
	{
		Enemy3D* pEnemy = dynamic_cast<Enemy3D*>(obj);
		if (pEnemy != nullptr)
		{
			VECTOR enemyPos = pEnemy->GetPosition();

			// プレイヤーから見てどっち側にいるか
			float LookX = enemyPos.x - playerPos.x;
			float LookZ = enemyPos.z - playerPos.z;

			// 敵がプレイヤーとどれだけ離れているか
			float enemyX = (float)centerX + (LookX / MapScale);
			float enemyY = (float)centerY - (LookZ / MapScale);

			// ミニマップ範囲外では書かない ちょっと調整
			if (enemyX -10 < X || enemyX + 10 > X + mnMiniMapSize || enemyY - 10 < Y || enemyY + 10 > Y + mnMiniMapSize)
			{
				continue;
			}

			// Yが０以下だったら暗い画像
			if (pEnemy->GetPosition().y <= 10.0f)
			{
				// 表示する黒い画像　透過度低め
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200); // 透明度をさげた

				// ここで画像を描画
				DrawRotaGraph(
					(int)enemyX,             // 位置X
					(int)enemyY,             // 位置Y
					0.15f,              // サイズ
					pEnemy->GetAngle(), // 回転
					mnEnemyHandle,     // 画像
					TRUE                // 左右反転らしい
				);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			else
			{
				// ここで画像を描画
				DrawRotaGraph(
					(int)enemyX,             // 位置X
					(int)enemyY,             // 位置Y
					0.15f,              // サイズ
					pEnemy->GetAngle(), // 回転
					mnEnemyHandle,     // 画像
					TRUE                // 左右反転らしい
				);
			}

		}
	}

	// カメラ関係
	mfCameraAngle = Master::mpCamera->GetHorizonAngle() * DX_PI_F / 180.0f;



	// 表示する黒い画像　透過度低め
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120); // 透明度をさげた
	// カメラの角度画像
	// 中心も設定できるから神
	DrawRotaGraph2(
		(int)centerX,             // 位置X
		(int)centerY,             // 位置Y
		256/2,
		256/2,
		0.9f,               // サイズ
		-mfCameraAngle,      // 回転
		mnCameraHandle,     // 画像
		TRUE                // 左右反転らしい
	);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


	// プレイヤーマークの描画
	mfPlayerAngle = pPlayer->GetAngle(); // 角度を代入
	DrawRotaGraph((int)centerX, (int)centerY, 0.25, mfPlayerAngle, mnPlayerHandle, TRUE);
}

