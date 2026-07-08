#pragma once
#include "DxLib.h"
#include "Object3D.h"
#include <vector>

class Floor : public Object3D
{
public:
	// コンストラクタ
	// centerPos   = 中心座標
	// topLeft     = 左上座標
	// bottomRight = 右下座標
	Floor(std::string filename, VECTOR centerPos, VECTOR topLeft, VECTOR bottomRight);
	// デストラクタ
	~Floor();

	void Update() override; // 更新
	void Draw() override;   // 描画


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


private:
	int mnGraphHandle; // 画像ハンドル

	VERTEX3D mVertex[4]; // 頂点座標（最終的に四角を書くので４つ）

};