#pragma once

#include "DxLib.h"
#include "Object3D.h"

class Effect : public Object3D
{
public:
	static const int PARTICLE_NUM = 128; // パーティクルの最大数

	/*enum OllEffect
	{
		None = 0,
		Fire = 1,
		Heel = 2,
		Water = 3,
		Baria = 4,

		EffectMax,
	};*/

	// パーティクル構造体
	// 粒１つ１つのデータ
	struct ParticleInfo
	{
		
		VECTOR pos;        // 座標
		VECTOR dir;        // 進行方向
		float speed;       // 速度
		float size;        // 大きさ
		float visibleTime; // 表示時間
		float alpha;       // 不透明度
	};

	// エフェクト情報
	// 粒をまとめて１つのエフェクトとしてのデータ
	struct EffectInfo
	{
		COLOR_U8 color; // エフェクト色
		ParticleInfo particle[PARTICLE_NUM]; // パーティクル情報
	};


	// 透明のモデルにするため
	DrawType GetDrawType() const { return DrawType::ToumeiModel; }



public:
	Effect(VECTOR initPos, std::string filename);
	~Effect();

	void Update() override;
	void Draw() override;

private:

	int mnGraphHandle;     // 画像ハンドル
	EffectInfo* mpEffect;  // エフェクトデータ 構造体をポインタとして持つ

	const int SPEED_RAND_MAX = 100;      // 速度の最大乱数値
	const int SPEED_RAND_MIN = 50;      // 速度の最小乱数値

	const int SIZE_RAND_MAX = 800;      // サイズの最大乱数値
	const int SIZE_RAND_MIN = 400;       // サイズの最小乱数値

	const int VISIBLE_TIME_RAND_MAX = 50; // 表示時間の最大乱数値
	const int VISIBLE_TIME_RAND_MIN = 20; // 表示時間の最小乱数値

};
