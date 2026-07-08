#pragma once
#include "DxLib.h"
#include "Texture.h"
#include "Object2D.h"
#include <string>

class OffsetBullet : public Object2D // 継承忘れずに
{
public:

	OffsetBullet(VECTOR initPos, std::string filename);

	// virtual 継承の時に使う
	virtual ~OffsetBullet();

	virtual void Update() override;

	virtual void Draw() override;

	// 移動処理
	void Move();


	// 画面外に出たかどうか
	bool IsScreenOut();

	// 当たり判定
	void CalcCollision();

protected:


	// 移動方向
	VECTOR mvDirection;
	// 現在の目標角度
	float mfAngle;
};
