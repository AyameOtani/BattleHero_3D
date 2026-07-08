#pragma once

#include "DxLib.h"
#include "Object3D.h"

class Effect2 : public Object3D
{
public:
	static const int PARTICLE_NUM = 128; // パーティクルの最大数

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
		int r, g, b;       // 色のランダム要素 追加

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
	Effect2(VECTOR initPos, std::string filename);
	~Effect2();

	void Update() override;
	void Draw() override;

private:

	int mnGraphHandle;     // 画像ハンドル
	EffectInfo* mpEffect;  // エフェクトデータ 構造体をポインタとして持つ

	const int SPEED_RAND_MAX = 100;      // 速度の最大乱数値
	const int SPEED_RAND_MIN = 100;      // 速度の最小乱数値

	const int SIZE_RAND_MAX = 600;      // サイズの最大乱数値
	const int SIZE_RAND_MIN = 300;       // サイズの最小乱数値

	const int VISIBLE_TIME_RAND_MAX = 70; // 表示時間の最大乱数値
	const int VISIBLE_TIME_RAND_MIN = 30; // 表示時間の最小乱数値

};
