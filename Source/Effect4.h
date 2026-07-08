#pragma once

#include "DxLib.h"
#include "Object3D.h"

class Effect4 : public Object3D
{
public:
	static const int PARTICLE_NUM = 15; // パーティクルの最大数

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
		float angle;       // 角度をランダムに
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
	Effect4(VECTOR initPos, std::string filename);
	~Effect4();

	void Update() override;
	void Draw() override;

private:

	int mnGraphHandle;     // 画像ハンドル
	EffectInfo* mpEffect;  // エフェクトデータ 構造体をポインタとして持つ

	const int SPEED_RAND_MAX = 100;      // 速度の最大乱数値
	const int SPEED_RAND_MIN = 100;      // 速度の最小乱数値

	const int SIZE_RAND_MAX = 1200;      // サイズの最大乱数値
	const int SIZE_RAND_MIN = 100;       // サイズの最小乱数値

	const int VISIBLE_TIME_RAND_MAX = 100; // 表示時間の最大乱数値
	const int VISIBLE_TIME_RAND_MIN = 30; // 表示時間の最小乱数値

};
