#pragma once
#include "DxLib.h"

class Collision
{
public:

	// コンストラクタ
	// なんもないから空でいい
	Collision()
	{
	}
	// デストラクタ
	~Collision()
	{
	}

	//円と円の当たり判定をしてくれるサークル
	//centerPosA...Aの円の中心座標
	//centerPosB...Bの円の中心座標
	//radiusA...Aの半径
	//radiusB...Bの半径



	// 円と円の当たり判定をしてくれ関数
	//const VECTOR&の&はconst参照型とかいうやつ。
	static bool CheckCircleToCircle(
	  const VECTOR& centerPosA,   // Aの円の中心座標
	  const float& radiusA,        // Aの円の半径
	  const VECTOR& centerPosB,   // Bの円の中心座標
	  const float& radiusB         // Bの円の半径
	);


	// 点と円の当たり判定
	static bool CheckPointToCircle(
		const VECTOR& PointPos,
		const VECTOR& centerPos,   // 円の中心座標
		const float& radius        // 円の半径
	);
};