#pragma once
#include <string>
#include "Object3D.h"
#include <vector>

class BulletMark : public Object3D
{
public:
	BulletMark(std::string filename, VECTOR initPos, VECTOR normal);
	~BulletMark();

	void Update() override;
	void Draw() override;

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

private:

	VECTOR mvNormal;
	float mfTime = 0.0f;
	float mfAngle = 0.0f; // 回転
	int mnHandle; // 画像ハンドル

	VERTEX3D mVertex[4]; // 頂点座標（最終的に四角を書くので４つ）


	// どんどんフェードさせるために使う
	static constexpr float FADE_TIME = 1400.0f; // 消す時間の上限
	float mfAlpha = 255; // 透過度

};