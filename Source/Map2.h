#pragma once
#include "DxLib.h"
#include "Texture.h"
#include "Object2D.h"

class Map2 : public Object2D // 継承
{
public:
	Map2(VECTOR initPos);
	virtual ~Map2();

	void Update() override; // 上書きしたからoverrideをつける
	void Draw() override;
	// 移動処理
	void Move();

private:

	int mnHandle;      // 読み込んだ画像のハンドル
	//VECTOR mvMapPosition; // ポジション

	// 6/30 に追加したもの●
	int mnHandle1;      // 読み込んだ画像のハンドル1
	VECTOR mvMapPosition1; // ポジション


};
