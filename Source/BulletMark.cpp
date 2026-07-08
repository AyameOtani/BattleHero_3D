#include "BulletMark.h"
#include "Master.h"
#include "SceneManager.h"
#include "ObjectManager.h"


BulletMark::BulletMark(std::string filename, VECTOR initPos, VECTOR normal)
	: Object3D(initPos)
    , mvNormal(VNorm(normal)) // 正規化
{
	SetTag(Object3D::T_BulletMark3D); // タグ

	// 画像読み込み Bulletで描いてる
	mnHandle = LoadGraph(filename. c_str());



    const float size = 20.0f; // 弾痕サイズ 今ちょっと大きめ

    // 法線から右・上ベクトルを作る
    VECTOR right = VNorm(VCross(mvNormal, VGet(0, 1, 0)));
    VECTOR up = VNorm(VCross(right, mvNormal));

	// ４頂点分のデータをセット
    // 左上
    mVertex[0].pos = VAdd(initPos, VAdd(VScale(right, -size), VScale(up, size)));
	mVertex[0].norm = mvNormal;
	mVertex[0].spc = GetColorU8(0, 0, 0, 0);
	mVertex[0].u = 0.0f;
	mVertex[0].v = 0.0f;
	mVertex[0].su = 0.0f;
	mVertex[0].sv = 0.0f;

    // 右上
    mVertex[1].pos = VAdd(initPos, VAdd(VScale(right, size), VScale(up, size)));
	mVertex[1].norm = mvNormal;
	mVertex[1].spc = GetColorU8(0, 0, 0, 0);
	mVertex[1].u = 1.0f;
	mVertex[1].v = 0.0f;
	mVertex[1].su = 1.0f;
	mVertex[1].sv = 0.0f;

    // 左下
    mVertex[2].pos = VAdd(initPos, VAdd(VScale(right, -size), VScale(up, -size)));
	mVertex[2].norm = mvNormal;
	mVertex[2].spc = GetColorU8(0, 0, 0, 0);
	mVertex[2].u = 0.0f;
	mVertex[2].v = 1.0f;
	mVertex[2].su = 0.0f;
	mVertex[2].sv = 1.0f;

    // 右下
    mVertex[3].pos = VAdd(initPos, VAdd(VScale(right, size), VScale(up, -size)));
	mVertex[3].norm = mvNormal;
	mVertex[3].spc = GetColorU8(0, 0, 0, 0);
	mVertex[3].u = 1.0f;
	mVertex[3].v = 1.0f;
	mVertex[3].su = 1.0f;
	mVertex[3].sv = 1.0f;
}


BulletMark::~BulletMark()
{

}

void BulletMark::Update()
{
	
	mfTime += 10.0f; // ※本当は GetDeltaTime() が理想

	// 上限を超えたら消す
	if (mfTime >= FADE_TIME)
	{
		SetDeleteFlag(true);
		return;
	}

	// フェード率(0-1) Hpバーとかと同じ考え方でやる
	float rate = mfTime / FADE_TIME;
	float alpha = 1.0f - rate;

	// 透明度
	mfAlpha = (alpha * 255.0f);
	if (mfAlpha < 0)
	{
		mfAlpha = 0;
	}

	for (int i = 0; i < 4; i++)
	{
		mVertex[i].dif = GetColorU8(255, 255, 255, (int)mfAlpha);
	}



}

void BulletMark::Draw()
{
	// ここをビルボードじゃなく、他のにしたいがきびいか？
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

	// ２つの三角形ポリゴンの描画
	// 使う引数、頂点の数、インデックスデータ、三角形の数、画像ハンドル、透過
	DrawPolygonIndexed3D(mVertex, 4, index, 2, mnHandle, true); // 頂点のデータ配列を使って書いてもらう

	SetUseLighting(true); // ライト関係の関数
}