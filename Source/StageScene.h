#pragma once

#include "Scene.h" // シーン継承したいからインクルード

class StageScene : public Scene
{
private:    // Playerクラスで使用する定数の定義
	static const int MAX_KEY = 3;


public:

	StageScene();

	virtual ~StageScene();

	// 初期化
	virtual void Initialize() override;
	// 更新
	virtual void Update() override;
	// 描画
	virtual void Draw() override;
	// 終了処理
	virtual void Finalize() override;

	
private:

	int mnKey;

	// 画像のハンドル
	int mnHandle0;
	int mnHandle1;
	int mnHandle2;
	int mnHandle3;

	int mCurrentImage; // 変える変数のtuika

	float mCurrentPhoto = 0;
	bool mFlushTimer = true;
};
