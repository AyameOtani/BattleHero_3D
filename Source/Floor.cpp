#include "Floor.h"

Floor::Floor(std::string filename, VECTOR centerPos, VECTOR topLeft, VECTOR bottomRight)
	:Object3D(centerPos)
{
	SetTag(Object3D::T_Floor3D); // タグ設定

	// 画像読み込み
	mnGraphHandle = LoadGraph(filename.c_str());

	// ４頂点分のデータをセット


	// 左前
	mVertex[0].pos = VAdd(centerPos, VGet(topLeft.x, centerPos.y, topLeft.z)); // 中心座標と左上になる座標を足す
	mVertex[0].norm = VGet(1.0f, 0.0f, 0.0f);  // 後で計算するので仮の値
	mVertex[0].dif = GetColorU8(255, 255, 255, 255);
	mVertex[0].spc = GetColorU8(0, 0, 2550, 0);
	mVertex[0].u = 0.0f;
	mVertex[0].v = 0.0f;
	mVertex[0].su = 0.0f;
	mVertex[0].sv = 0.0f;

	// 右前
	mVertex[1].pos = VAdd(centerPos, VGet(bottomRight.x, centerPos.y, topLeft.z)); // 右上のX　左のY　右の下のZ
	mVertex[1].norm = VGet(1.0f, 0.0f, 0.0f);  // 後で計算するので仮の値
	mVertex[1].dif = GetColorU8(255, 255, 255, 255);
	mVertex[1].spc = GetColorU8(0, 0, 0, 0);
	mVertex[1].u = 1.0f; // Xは１
	mVertex[1].v = 0.0f; // Yは０
	mVertex[1].su = 1.0f;
	mVertex[1].sv = 0.0f;

	//  左奥
	mVertex[2].pos = VAdd(centerPos, VGet(topLeft.x, centerPos.y, bottomRight.z)); // 右上のX　左のY　右の下のZ
	mVertex[2].norm = VGet(1.0f, 0.0f, 0.0f);  // 後で計算するので仮の値
	mVertex[2].dif = GetColorU8(255, 255, 255, 255);
	mVertex[2].spc = GetColorU8(0, 0, 0, 0);
	mVertex[2].u = 0.0f; // Xは０
	mVertex[2].v = 1.0f; // Yは
	mVertex[2].su = 0.0f;
	mVertex[2].sv = 1.0f;

	// 　右奥
	mVertex[3].pos = VAdd(centerPos, VGet(bottomRight.x, centerPos.y, bottomRight.z)); // 右上のX　左のY　右の下のZ
	mVertex[3].norm = VGet(1.0f, 0.0f, 0.0f);  // 後で計算するので仮の値
	mVertex[3].dif = GetColorU8(255, 255, 255, 255);
	mVertex[3].spc = GetColorU8(0, 0, 255, 0);
	mVertex[3].u = 1.0f; // Xは１
	mVertex[3].v = 1.0f; // Yは０
	mVertex[3].su = 1.0f;
	mVertex[3].sv = 1.0f;


	// 法線の設定
	//左上から右上にいくベクトルと左上から左下にいくベクトル
	//VCrossは外積でそれぞれ左上から右上を引き、左上から左下を引いている
	VECTOR norm = VCross(VSub(mVertex[3].pos, mVertex[1].pos), VSub(mVertex[3].pos, mVertex[2].pos));

	norm = VNorm(norm); // 正規化（ベクトルの大きさを１にする）

	// ４つの頂点に対して法線を設定する
	mVertex[0].norm = norm;
	mVertex[1].norm = norm;
	mVertex[2].norm = norm;
	mVertex[3].norm = norm;
}

Floor::~Floor()
{
	// 画像の破棄
	DeleteGraph(mnGraphHandle);
}

// 更新
void Floor::Update()
{

}

// 描画
void Floor::Draw()
{
	WORD index[6];

	// 2ポリゴン分のインデックスデータを設定
	// 右辺は、頂点データの配列番号　この6点を使って3角形を2つ作る
	index[0] = 0;
	index[1] = 1;
	index[2] = 2;
	index[3] = 3;
	index[4] = 2;
	index[5] = 1;

	SetUseLighting(false); // ライト関係の関数

	// ２つの三角形ポリゴンの描画
	// 使う引数、頂点の数、インデックスデータ、三角形の数、画像ハンドル、透過
	DrawPolygonIndexed3D(mVertex, 4, index, 2, mnGraphHandle, true); // 頂点のデータ配列を使って書いてもらう


	SetUseLighting(true); // ライト関係の関数
}