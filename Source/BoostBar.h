#pragma once
#include "DxLib.h"

class Unit;

class BoostBar
{
public:
	BoostBar(Unit* unit); // Unitの変数を受取りたい
	~BoostBar();

	void Update();

	// ブーストバーの設定と描画
	void PlayerBoostBar();
	void EnemyBoostBar();

private:
	Unit* mpUnit;

	// フラッシュ（ゲージ用）関係
	float mCurrentPhoto = 0.0f;
	bool mFlushTimer = false;

	// 二重テクスチャ
	int scrollX = 0; // スクロールする変数
	int mnHandel_Tex; // スクロールする光画像

};
