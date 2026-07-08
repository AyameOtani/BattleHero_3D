#pragma once

#include "Scene.h" // シーン継承したいからインクルード

class LoseResultScene : public Scene
{

public:

	LoseResultScene();

	virtual ~LoseResultScene();

	// 初期化
	virtual void Initialize() override;
	// 更新
	virtual void Update() override;
	// 描画
	virtual void Draw() override;
	// 終了処理
	virtual void Finalize() override;

private:
	// フラッシュ用
	float mCurrentPhoto = 0.0f;
	bool mFlushTimer = true;
};

