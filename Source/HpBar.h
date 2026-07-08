#pragma once
#include "DxLib.h"
#include <vector>
#include "Baria.h"

class Unit;

class HpBar
{
public:

	HpBar(Unit* unit); // Unitの変数を受取りたい

	~HpBar();

	void Update();

	// Hpバーの設定と描画
	void PlayerHpBar();
	void EnemyHpBar();

private:
	Unit* mpUnit; // ユニットクラスのポインタ

	float mfSlowHp = 0.0f; // 遅れて表示するHP
	float mfSlowSpeed = 10.0f; // 1フレームに減る速度

	// フラッシュ（ゲージ用）関係
	float mCurrentPhoto = 0.0f;
	bool mFlushTimer = false;



	// 追加　HPバー
	//VERTEX3D mVertex[4]; // もういらない
	float mfSlowEHp = 0.0f; // 遅れて表示するHP
	float mfSlowESpeed = 0.2f; // 1フレームに減る速度


	// 二重テクスチャ
	int scrollX = 0; // スクロールする変数
	int mnHandel_Tex; // スクロールする光画像

};


