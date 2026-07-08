#pragma once
#include "Object3D.h"
#include "Model.h"
#include "BreakableGlass.h"

class Model;
class Baria : public Object3D // モデルにしたから継承させた 1217
{
private:
	static constexpr float COOL_TIME = 9.0f; // バリアのクールタイム  表示されるのは8から
	static constexpr float FADE_TIME = 3.0f; // 貼る時間

public:

	Baria(std::string filename, VECTOR initPos);

	~Baria();

	void Update();
	void DrawBaria();
	void Start();

	// シールドがはられているかのゲッター（敵とかからも、参照するため）
	bool IsActive() const { return mbBaria; }// ゲッター

	void DrawBar(); // バリアのバーの描画

	// バリアを透明モデル扱いにする 1218
	DrawType GetDrawType() const { return DrawType::ToumeiModel; }

	bool Initialize(); // 初期化之追加

	 //フェードを透過に変換したいから取得 
	float GetFadeTime()const { return mfFadetime; }
	float GetMaxFadeTime()const { return FADE_TIME; }

private:
	float mfAlpha; // 透明度　ビルボードとかで使った

	// 画像用
	int mnHandle;
	int mnGaugeHandle; // バリアのゲージ画像を読み込むための
	int mnModelHandle;   // ★ 3Dモデル

	// フラッシュ（ゲージ用）関係
	float mCurrentPhoto = 0.0f;
	bool mFlushTimer = false;


	// クールタイム関係
	bool mbBaria;                     // バリア中かを見る
	float mfCoolTime = 0.0f;          // クールタイムの現在値
	bool mbGetCool = false;           // バリアをできるかのフラグ　ON OFF
	float mfFadetime = 0.0f;          // 現在のバリアの残り秒数
	
	int mnFontBig; // 文字をおおきく表示するため


private:
	//Model* mpModel; //　モデルポインタ

private: // 割れるバリア関係

	BreakableGlass mGlass;     // ★追加：割れるガラス
	bool mWireGlass = false;   // ★追加：ガラスをワイヤー表示するか
	bool   mBarrierAlive; // まだ割れてないか

	// ★★★★★★★★ バリアが割れるときの「飛び散る強さ」
	// 数値が大きいほど破片が勢いよく飛ぶ
	const float  BARRIER_BREAK_POWER = 500.0f;
	const float  FIXED_DT = 1.0f / 60.0f;


};