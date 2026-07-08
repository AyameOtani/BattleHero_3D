#pragma once
#include "Scene.h"
#include <array>
#include <string>
#include "Graph.h"

class DrawWeapon;
class SelectWeapon : public Scene
{
public:

	SelectWeapon();
	~SelectWeapon();

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void Finalize() override;

	void Draw2D(); // 文字などの描画
	void WeaponDraw(); // 武器の見た目の描画

	void SelectPlayer(); // 選択しているplayerの画像を描画する
	void DrawMesseage(); // errorメッセージ表示など



private:

	// フラッシュ
	float mCurrectPhoto = 0.0f;
	bool mFlushTimer = true;
	bool mbRoadFlag = false;
	int mnSelect; // 選択で使うもの
	int mnSelectMax = 6; // 選択肢の最大値 上限に使う

	// 文字OR画像関係
	int mnFontBig; // 文字をおおきく表示するため
	int mnFontNormal; // いつものフォントサイズ
	int mnFontSmall; // 小さいやつ
	int mnFontSsize; // S

	int mnPlayerFont; // プレイヤーのすて表示
	
	int mnBoxHandle; // 画像
	int mnHandle_Select; // 選択の付箋のやつ
	int mnHandle_No_Select; // 選択してない付箋
	int mnHandle_Error; // errorメッセ表示用
	int mnHandle_ErrorBag; // エラーのときに暗くする画像を出す
	int mnHandle_SelectWeapon; // 選択武器の枠

	int mnHandle_Bag;	// 画像表示用
	bool mbStickInput = false; // スティックで連続入力を避けるため
	int mnHandle_Sky;

	// ポインタ関係
	DrawWeapon* mpDrawWeapon = nullptr;

	// 土日に追加した
	bool mbSelectMain = true;   // true = main選択中   false = sub選択中
	bool mbSelectFinished = false; // メインとサブが決定していたらゲーム画面に進むためのフラグ

	bool mbErrorMesse = false; // 同じものが選択されたときに出すメッセージ
	float mfErrorCount = 0.0f; // メッセ閉じるためのカウント
	const float ERROR_COUNT = 80.0f; // 1秒ぐらい


	// 選択中の文字の色を変えるカウントとフラグ
	bool mbSelectNow = true; // 初めは光らせたいからTRUEにしてる
	float mfSelectCount = 0.0f;
	std::string msPlayerName;
	// GRAPF
	int mPhotoHandle = -1;
	// BGMをとめるかのフラグ
	bool mbBgmStop = false;
	// 選択が変わったかの
	int mnChengeSlect;


	const float MaxSta = 1500; // 最大攻撃力
	const float MaxSp = 80; // 最大弾数
};

