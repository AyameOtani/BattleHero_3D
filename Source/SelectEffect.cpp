#include "SelectEffect.h"
#include <cmath>
#include <string>







// texture読み込み
// 魔法陣のやつ
bool SelectEffect::Load(std::string filename)
{
    // 同じファイルなら何もしない
	if (mnTex != -1 && msFilename == filename) return true;
    // 違うファイルなら破棄
    if (mnTex != -1)
    {
        DeleteGraph(mnTex);
        mnTex = -1;
    }

	mnTex = LoadGraph(filename.c_str());
    if (mnTex == -1)
    {
        msFilename.clear();
        return false;
    }


    msFilename = filename;

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseLighting(FALSE);

	return true;
}
void SelectEffect::StartHold(const VECTOR& playerPos)
{
	if (mnTex == -1) return;

	// 中心位置指定
	mvCenter = playerPos;
	mvCenter.y += mfYOffset; // Yはずらしている

	mfAngle = 0.0f; // 回転の初期化
}

void SelectEffect::Update(const VECTOR&)
{
	if (mnTex == -1) return;

	// 回転 反対まわりならー
	mfAngle += mfSpeed;
}

void SelectEffect::Draw() const
{
	if (mnTex == -1) return;

	// 輝度を設定
	SetDrawBright(mnBright, mnBright, mnBright);
	// 描画の呼び出し
	DrawHoldingBoard();
	// 元に戻す
	SetDrawBright(255, 255, 255);
}

// 画像の描画
void SelectEffect::DrawHoldingBoard() const
{
    const float half = mfSize * 0.5f;
    const float c = cosf(mfAngle);
    const float s = sinf(mfAngle);

    const VECTOR local[4] =
    {
        VGet(-half, 0.0f, -half),
        VGet(half, 0.0f, -half),
        VGet(half, 0.0f,  half),
        VGet(-half, 0.0f,  half),
    };

    VECTOR world[4];
    for (int i = 0; i < 4; ++i)
    {
        const float x = local[i].x;
        const float z = local[i].z;
        const float rx = x * c - z * s;
        const float rz = x * s + z * c;
        world[i] = VAdd(mvCenter, VGet(rx, 0.0f, rz));
    }

    VERTEX3D v4[4]{};
    const DxLib::COLOR_U8 dif = GetColorU8(255, 255, 255, 255);
    const DxLib::COLOR_U8 spc = GetColorU8(0, 0, 0, 0);
    const VECTOR norm = VGet(0.0f, 1.0f, 0.0f);

    for (int i = 0; i < 4; ++i)
    {
        v4[i].pos = world[i];
        v4[i].norm = norm;
        v4[i].dif = dif;
        v4[i].spc = spc;
    }

    v4[0].u = 0.0f; v4[0].v = 0.0f;
    v4[1].u = 1.0f; v4[1].v = 0.0f;
    v4[2].u = 1.0f; v4[2].v = 1.0f;
    v4[3].u = 0.0f; v4[3].v = 1.0f;

    SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
    SetUseBackCulling(FALSE);

    VERTEX3D tri1[3] = { v4[0], v4[1], v4[2] };
    VERTEX3D tri2[3] = { v4[0], v4[2], v4[3] };
    DrawPolygon3D(tri1, 3, mnTex, TRUE);
    DrawPolygon3D(tri2, 3, mnTex, TRUE);

    SetUseBackCulling(TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

