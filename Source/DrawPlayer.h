#pragma once
#include "Model.h"
#include <string>
#include "Object3D.h"
#include "SelectEffect.h"

class DrawPlayer : public Object3D
{
public:

	// 武器みたいに改造ありだと思った0126だからやってみる
	// モデルごとにファイルを分けている
	enum DrawSelectID
	{
		None = 0,

		Hero = 1,
		Zombi = 2,
		Guntai = 3,
		Bob = 4,
		Devil = 5,

		DrawMax,
	};

	DrawPlayer(std::string filename, VECTOR initPos);
	~DrawPlayer();

	void Update() override;
	void Draw() override;
	void Rotation(); // 回転させる

	// 追加0126
	void LoadPlayer(DrawSelectID id, std::string filename, VECTOR initPos); // IDとファイルでおK？
	// 呼べるように
	void SetDrawPlayer(DrawSelectID id);
	// 一応ゲッターも
	int GetDrawPlayer() const { return mnNowDraw; }

	void AllLoad(); // ロードする

	// ★New★
	// アニメーション追加
	// Modelクラスへの橋渡し関数
	void AddAnimation(AnimationState state, std::string filename);

	void EffectLoad();

private:
	// エフェクト関係
	SelectEffect mSelectEffect; // エフェクト


	Model* mpModel;
	float mfAngle; // 回転させたいから

	// 追加
	DrawSelectID mnNowDraw;
	std::map<DrawSelectID, Model*> mmDrawPlayer;

	float posX = -100.0f; // 左にずらすXのやつ
	float posY = 10.0f;

	int prevTime = 0;
};