#pragma once
#include "Scene.h"
#include <array>
#include <string>
#include "Object2D.h"



class DrawPlayer;
class SelectScene3D : public Scene
{
public:
	SelectScene3D();
	~SelectScene3D();

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void Finalize() override;


	// 追加 0126
	void Draw2D(); // 2Dのを描画 背景とかもろもろ
	void PlayerUpdate(); // モデルの見た目を更新 調整しやすいようにupdateと分ける


 private:

	// フラッシュ
	float mCurrectPhoto = 0.0f;
	bool mFlushTimer = true;
	bool mbRoadFlag = false;
	int mnSelect; // 選択で使うもの
	int mnSelectMax = 5; // 選択肢の最大値 上限に使う


	int mnFontBig; // 文字をおおきく表示するため
	int mnFontNormal; // いつものフォントサイズ
	int mnFontSmall; // 小さいやつ
	int mnFontSsize; // 小さいやつ

	float mfSelectedHp = 0.0f;  // Hpを一時的に保存するメンバ
	float mfSelectBoost = 0.0f; // Boostを一時的にほぞんする変数
	float mfSelectSpeed = 0.0f; // 速さ保存メンバ
	float mfSelectMaxSpeed = 0.0f; // 最大速さ保存メンバ



	int mnHandle_Bag;	// 画像表示用
	int mnHandle_Select; // 選択の付箋のやつ
	int mnHandle_No_Select; // 選択してない付箋
	int mnHandle_Sky;

	bool mbStickInput = false; // スティックで連続入力を避けるため

	// ポインタ関係
	DrawPlayer* mpDrawPlayer = nullptr; // 表示するプレイヤーモデルのポインタ

	// 前の選択保存
	int mnChengeSelect;

	const int MaxSta = 10000; // 最大HP
	const int MaxStaB = 1000; // 最大BOOST
	const int MaxSp = 50; // 最大速度
};
