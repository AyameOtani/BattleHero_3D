#pragma once

#include "Scene.h" // シーン継承したいからインクルード

class GameScene : public Scene
{

public:

	GameScene();

	virtual ~GameScene();

	// 初期化
	virtual void Initialize() override;
	// 更新
	virtual void Update() override;
	// 描画
	virtual void Draw() override;
	// 終了処理
	virtual void Finalize() override;

	// レベルのデータ
	void LevelDate(int LevelImage);

	//機体保存のvoid
	void PlayerSelectDate(int mPlayerImage);
	
private:
	int mLevelImage;    // レベルのやつ
	int mTimeCount = 0;


	int mPlayerImage; // 変える変数の追加
	// 背景画像の読み込み
	int mnHandleBag;

};

