#pragma once
#include "DxLib.h"
#include <cmath>


class GameTimer
{
public:

	GameTimer();
	~GameTimer();

	void Update();
	void Draw();
	void Initialize(); // ゲームシーンで初期化するため
	void Reset(); // 時間をリセットするため


	// 扇形を法線上に書く円形ゲージ関数
	void DrawRadialGauge(
		float cx,			  // X
		float cy,			  // Y
		float r,			  // 半径
		int filledSeg,		  // 今埋まっている枚数
		float partial,		  // 一枚の埋まり具合
		int segments,		  // 最大数
		unsigned int colFill, // ゲージ色
		unsigned int colBack, // 背景色
		float startDeg        // ゲージ始まりの角度
	);

	// 残り秒数のゲッター　0になったらLOSEシーンに行きたいから
	int GetTime() const { return mnRemainTime; }



private:
	// 受取ったdを度からラジアンに変更する
	static inline float DegToRad(float d) { return d * (float)DX_PI / 180.0f; }

	
	//// ---- ゲージ管理用パラメータ ----
	//const int   SEGMENTS = 36;         // 三角形の枚数（細かさ）
	//const float FILL_SPEED = 2.5f;     // 押下中の増加スピード[枚/秒]（端数はpartialへ）
	//const float POP_INTERVAL = 0.1f;   // 離している間の「1枚ずつ消える」間隔[秒]
	//const float PARTIAL_SPEED = 3.0f;  // 端数(部分)を0→1に貯める速度[枚/秒]（見た目の滑らかさ用）
	//
	//int   filledSeg = 0;   // 完全に埋まっている三角形の枚数
	//float partial = 0.0f;  // 最後の1枚の埋まり具合[0..1) 最後で１になるが、途中の一枚一枚の埋まり具合でも変わる
	//float popTimer = 0.0f; // 離している時の「次の1枚を消すまでのタイマー」
	//int   prevTime = GetNowCount(); // 今の時間


private:
	// 360を分ける処理
	float stepDeg = 0.0f;
	// 時間にするため
	const float  SEGMENTS = 1000.0f; // 三角形の枚数   300でもまだ線が分かるから調整必要
	float TotalTime =180.0f; // 時間秒数 三分ぐらいにしたいから180秒ぐらい
	int filledSeg = 0; // 分ける数　減らすやつ
	float partial = 0.0f;

	// 時間
	int prevTime = 0;
	int mnFontBig = 0; // おおきく表示するため
	int mnRemainTime = 0; // 残り秒数のint版

	int mnChengeCount = -1;  // 前フレームのカウント
};