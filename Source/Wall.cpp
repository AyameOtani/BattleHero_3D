#include "Wall.h"
#include "Master.h"
#include "Scene.h"
#include "ObjectManager.h"



Wall::Wall(std::string filename, VECTOR centerPos, VECTOR topLeft, VECTOR bottomRight)
	:Object3D(centerPos)
{
	SetTag(Object3D::T_Wall3D); // タグ設定

	// 画像読み込み
	mnGraphHandle = LoadGraph(filename. c_str());

	// ４頂点分のデータをセット
	// 左上
	mVertex[0].pos = VAdd(centerPos, topLeft); // 中心座標と左上になる座標を足す
	mVertex[0].norm = VGet(1.0f, 0.0f, 0.0f);  // 後で計算するので仮の値
	mVertex[0].dif = GetColorU8(255, 255, 255, 255);
	mVertex[0].spc = GetColorU8(0, 0, 0, 0);
	mVertex[0].u = 0.0f;
	mVertex[0].v = 0.0f;
	mVertex[0].su = 0.0f;
	mVertex[0].sv = 0.0f;

	// 右上
	mVertex[1].pos = VAdd(centerPos, VGet(bottomRight.x, topLeft.y, bottomRight.z)); // 右上のX　左のY　右の下のZ
	mVertex[1].norm = VGet(1.0f, 0.0f, 0.0f);  // 後で計算するので仮の値
	mVertex[1].dif = GetColorU8(255, 255, 255, 255);
	mVertex[1].spc = GetColorU8(0, 0, 0, 0);
	mVertex[1].u = 1.0f; // Xは１
	mVertex[1].v = 0.0f; // Yは０
	mVertex[1].su = 1.0f;
	mVertex[1].sv = 0.0f;

	// 左下
	mVertex[2].pos = VAdd(centerPos, VGet(topLeft.x, bottomRight.y, topLeft.z)); // 右上のX　左のY　右の下のZ
	mVertex[2].norm = VGet(1.0f, 0.0f, 0.0f);  // 後で計算するので仮の値
	mVertex[2].dif = GetColorU8(255, 255, 255, 255);
	mVertex[2].spc = GetColorU8(0, 0, 0, 0);
	mVertex[2].u = 0.0f; // Xは０
	mVertex[2].v = 1.0f; // Yは
	mVertex[2].su = 0.0f;
	mVertex[2].sv = 1.0f;

	// 右下
	mVertex[3].pos = VAdd(centerPos,  bottomRight); // 右上のX　左のY　右の下のZ
	mVertex[3].norm = VGet(1.0f, 0.0f, 0.0f);  // 後で計算するので仮の値
	mVertex[3].dif = GetColorU8(255, 255, 255, 255);
	mVertex[3].spc = GetColorU8(0, 0, 0, 0);
	mVertex[3].u = 1.0f; // Xは１
	mVertex[3].v = 1.0f; // Yは０
	mVertex[3].su = 1.0f;
	mVertex[3].sv = 1.0f;

	// 法線の設定
	// 左上から右上にいくベクトルと左上から左下にいくベクトル
	VECTOR norm = VCross(VSub(mVertex[3].pos, mVertex[1].pos), VSub(mVertex[3].pos, mVertex[2].pos));
	norm = VNorm(norm); // 正規化（ベクトルの大きさを１にする）

	// ４つの頂点に対して法線を設定する
	mVertex[0].norm = norm;
	mVertex[1].norm = norm;
	mVertex[2].norm = norm;
	mVertex[3].norm = norm;
}

Wall::~Wall()
{
	// 画像の破棄
	DeleteGraph(mnGraphHandle);
}

// 更新
void Wall::Update()
{
	// プレイヤーの情報取得
	auto pPlayer = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DByTag(Object3D::T_Player3D);

	// 壁の情報取得
	auto pWallList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Wall3D); // 壁の情報をとる

	//VECTOR moveDir = VGet(0.0f, 0.0f, 0.0f); // 今フレームの移動方向
	//float speed = 5.0f; // 速さ


	//if (!pWallList.empty())
	//{
	//	for (int i = 0; i < pWallList.size(); i++)
	//	{
	//		auto pWall = pWallList[i];

	//		if (pPlayer != nullptr && pWall != nullptr)
	//		{
	//			Wall* pTargetWall = dynamic_cast<Wall*>(pWall);
	//			if (pTargetWall == nullptr) return;

	//			VECTOR playerPos = pPlayer->GetPosition();      // プレイヤー座標

	//			// 壁の中心を求めている
	//			VECTOR wallCenter = VGet(
	//				(pTargetWall->GetVertex()[0].pos.x + pTargetWall->GetVertex()[3].pos.x) / 2,
	//				(pTargetWall->GetVertex()[0].pos.y + pTargetWall->GetVertex()[3].pos.y) / 2,
	//				(pTargetWall->GetVertex()[0].pos.z + pTargetWall->GetVertex()[3].pos.z) / 2
	//			);

	//			VECTOR toPlayer = VSub(playerPos, wallCenter);
	//			float distance = VSize(toPlayer);
	//			pTargetWall->SetDistance(distance);


	//			// デバッグ用に線を描画（赤色）
	//			//DrawLine3D(wallCenter, playerPos, GetColor(255, 0, 255));

	//			pTargetWall->SetDistance(distance); // 距離をセッターに入れている

	//		

	//			if (distance <= 3000)
	//			{
	//				mfAlpha = 255.0f; // 見える
	//				//pWall->SetAlpha(mfAlpha);
	//				pTargetWall->SetAlpha(mfAlpha);

	//			}
	//			else if (distance > 3000 && distance < 4000)
	//			{
	//				float alphaRate = 1.0f - (distance - 3000) / (4000 - 3000);
	//				mfAlpha = alphaRate * 255.0f;
	//				pTargetWall->SetAlpha(mfAlpha);
	//			}
	//			else
	//			{
	//				mfAlpha = 0.0f; // 見えない
	//				pTargetWall->SetAlpha(mfAlpha);
	//			}
	//			//DrawFormatString(800, 300+ i*20, GetColor(255, 255, 255), "距離: %f", pTargetWall->GetDistance());

	//		}

	//	}
	//}
}

// 描画
void Wall::Draw()
{
	WORD index[6];

	// 2ポリゴン分のインデックスデータを設定
	// 右辺は、頂点データの配列番号
	index[0] = 0;
	index[1] = 1;
	index[2] = 2;
	index[3] = 3;
	index[4] = 2;
	index[5] = 1;

	SetUseLighting(false); // ライト関係の関数



	// ここで透明度を変えるようにしている
	/*for (int i = 0; i < 4; i++)
	{
		mVertex[i].dif = GetColorU8(255, 255, 255, GetAlpha());
	}*/

	// ２つの三角形ポリゴンの描画
	// 使う引数、頂点の数、インデックスデータ、三角形の数、画像ハンドル、透過
	DrawPolygonIndexed3D(mVertex, 4, index, 2, mnGraphHandle, true); // 頂点のデータ配列を使って書いてもらう




	SetUseLighting(true); // ライト関係の関数
}