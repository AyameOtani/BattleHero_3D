#pragma once
#include "DxLib.h"
#include "Object3D.h"
#include <vector>

class Wall : public Object3D
{
public:
	// コンストラクタ
	// centerPos   = 中心座標
	// topLeft     = 左上座標
	// bottomRight = 右下座標
	Wall(std::string filename, VECTOR centerPos, VECTOR topLeft, VECTOR bottomRight);
	// デストラクタ
	~Wall();

	void Update() override; // 更新
	void Draw() override;   // 描画

	// 壁を透明モデル扱いにする 1222
	DrawType GetDrawType() const { return DrawType::ToumeiModel; }

	// 頂点情報の取得
	std::vector<VERTEX3D> GetVertex()
	{
		std::vector<VERTEX3D> result;
		result.push_back(mVertex[0]);
		result.push_back(mVertex[1]);
		result.push_back(mVertex[2]);
		result.push_back(mVertex[3]);
		return result;
	}

	//float GetAlpha() { return mfAlpha; } // 透明度の取得
	//void SetAlpha(float alpha) { mfAlpha = alpha; } // 透明度をセット

	//float GetDistance() { return mfDistance; } // 距離のゲッター
	//void SetDistance(float distance) { mfDistance = distance; } // 距離のセッター


private:
	int mnGraphHandle; // 画像ハンドル

	VERTEX3D mVertex[4]; // 頂点座標（最終的に四角を書くので４つ）

	//float mfAlpha = 0.0f; // 追加11/10 透明度の変数

	//float mfDistance = 0.0f; // 距離の変数
};