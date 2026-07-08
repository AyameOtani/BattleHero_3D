#pragma once
#include "DxLib.h"
#include "Object3D.h"
#include <vector>

class MiniMap3D : public Object3D
{
public:
	MiniMap3D(VECTOR iniPos);
	~MiniMap3D();

	// ミニマップ用のスクリーン作成
	bool Initialize(int mapSize, float mapHeight);

	// 終了処理
	void Finalize();

	// マップを描画するやつ　幅と高さ
	void Draw(int screenW, int screenH);

	// 描画するおbジェクトを書く関数
	void DrawObj();
	// 描画するプレイヤーの位置とかを書く関数
	void Draw2DObj();

private:

	// ミニマップ使う
	int mnMiniMapScreen;     // ミニマップ用スクリーン
	int mnMiniMapSize;       // ミニマップのサイズ
	float mnMapCameraHeight; // マップをどこの高さから見るかの位置

	int X = 0;
	int Y = 0;


	// プレイヤーの画像と回転
	int mnPlayerHandle;
	float mfPlayerAngle = 0.0f;

	// カメラの画像と回転
	int mnCameraHandle;
	float mfCameraAngle = 0.0f;

	// 敵の画像と回転
	int mnEnemyHandle;
	float mfEnemyAngle = 0.0f;
};

