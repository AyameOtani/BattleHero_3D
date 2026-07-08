#pragma once
#include "Scene.h" // シーン継承したいからインクルード
#include "Score3D.h"
#include "Graph.h"
#include "Slide3D.h"


class LoseResult3D : public Scene
{

public:
	// コンストラクタ
	LoseResult3D();
	// デストラクタ
	virtual ~LoseResult3D();

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
	bool mbRoadFlag = false; // ロード中かのフラグ
	// 画像用のメンバ
	int mnHandle_Bag;
	int mnHandle_Sky;
	int mnHandle_D = 0; // Graphのやつを入れるハンドル

	// スコアのやつ
	Score3D m_Score;
	Slide3D m_Slide; // スライド

	// 文字の大きさ
	int mnFontBig; // 大きいフォント
	int mnFontNormal; // ふつうのフォント
	int mnFontSmall; // 小さいやつ

};
