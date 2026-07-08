#pragma once

#include "Scene.h" // シーン継承したいからインクルード
#include <vector>

class OperateScene3D : public Scene
{

public:
	// コンストラクタ
	OperateScene3D();
	// デストラクタ
	virtual ~OperateScene3D();

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
	bool mFlushTimer = true; // フラッシュのタイマーを有効にしている

	// 追加
	bool mbRoadFlag = false; // ロードしているかのフラグ


	// 文字のおおきさを変えるため
	int mnFontBig;
	int mnFontNormal;
	int mnFontSmall;
	// 画像用
	int mnHandle_Bag;

	// 選択するやつ
	int mnSelect;
	int mnSelectMax = 8; // 最大数

	bool mbStickInput = false; // スティックで連続入力を避けるため

	std::vector<int> mGraph; // 画像の配列

	int mnSelectChenge; // 選択が変わったかどうか
};
