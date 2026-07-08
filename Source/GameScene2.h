#include "Scene.h" // シーン継承したいからインクルード

class GameScene2 : public Scene
{

public:

	GameScene2();

	virtual ~GameScene2();

	// 初期化
	virtual void Initialize() override;
	// 更新
	virtual void Update() override;
	// 描画
	virtual void Draw() override;
	// 終了処理
	virtual void Finalize() override;

	void LevelDate(int LevelImage);
	// 機体保存のvoid
	void PlayerSelectDate(int mPlayerImage);

private:
	int mLevelImage;    // レベルのやつ
	int mPlayerImage; // 変える変数の追加
	int mnHandleBag1;
};