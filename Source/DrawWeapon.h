#pragma once
#include "Model.h"
#include <string>
#include "Object3D.h"
#include "SelectEffect.h"

class DrawWeapon : public Object3D
{
public:

	// 武器みたいに改造ありだと思った0126だからやってみる
	// モデルごとにファイルを分けている
	enum DrawSelectID
	{
		None = 0,

		Shild = 1,
		WriteGun = 2,
		BlackGun = 3,
		BlueGun = 4,
		YARI = 5,
		GT = 6,

		DrawMax,
	};

	DrawWeapon(std::string filename, VECTOR initPos);
	~DrawWeapon();

	void Update() override;
	void Draw() override;
	void Rotation(); // 回転させる

	// 追加0126
	void LoadWeapon(DrawSelectID id, std::string filename, VECTOR initPos); // IDとファイルでおK？
	// 呼べるように
	void SetDrawPlayer(DrawSelectID id);
	// 一応ゲッターも
	int GetDrawPlayer() const { return mnNowDraw; }

	void AllLoad(); // ロードする


	void EffectLoad();

private:
	// エフェクト関係
	SelectEffect mSelectEffect; // エフェクト


	Model* mpModel;
	float mfAngle; // 回転させたいから
	VECTOR mvDirection; // 回転

	// 追加
	DrawSelectID mnNowDraw;
	std::map<DrawSelectID, Model*> mmDrawPlayer;

	float posX = -100.0f; // 左にずらすXのやつ
	float posY = 160.0f;

	int prevTime = 0;
};