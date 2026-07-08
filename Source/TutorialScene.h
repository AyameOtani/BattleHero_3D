#pragma once
#include "Scene.h" // シーン継承したいからインクルード
#include "Score3D.h"
#include "Graph.h"


class TutorialScene : public Scene
{

public:
	// コンストラクタ
	TutorialScene();
	// デストラクタ
	virtual ~TutorialScene();

	// 初期化
	virtual void Initialize() override;
	// 更新
	virtual void Update() override;
	// 描画
	virtual void Draw() override;
	// 終了処理
	virtual void Finalize() override;

	// UIを描画するための関数
	void DrawOllUI();
	// ミニマップを描画するためのやつ
	void DrawMap();
	// 敵のUＩを描画するやつ
	void DrawEnemyUI();
	// ゲームオーバーの関数をまとめる関数
	void GameOver();
	void PoseUI(); // 一時停止するかしないかのやつ
	void BariaDraw(); // バリア描画するやつ
	// ブラー描画用の関数を追加
	void DrawWithBlur(VECTOR playerWorldPos);


	void TutorialUI(); // チュートリアル中のテキストを書くやつ


private:
	int mnSceneScreen = -1; // オフスクリーン用のハンドル

	// 追加
	bool mbDrawFlag = false; // オブジェクトが配置されたか (敵が出てくる前のシーン還移を防ぐため)
	bool mbTimerFlag = false; // タイマーを一回だけ初期化するため


	// 開始のカウントダウン
	bool mbDrawOK = false;     // 描画されたかのフラグ

	int mnFontBig; // 大きいフォント　カウントで使う
	int mnFontNormal; // ふつうのフォント
	int mnFontSmall; // 小さいやつ
	int mnFontLSize; // 少し大きめ

	// ポーズ画面関係
	bool mbPose = false; // 一時停止をするかどうかのフラグ
	int mnHandle_PoseBag; // ポーズのときに出す黒い画像
	int mnHandle_Pose; // ポーズ中の背景画像
	int mnHandle_PoseSelect; // 選択画像BOX
	int mnSelect; // 選択で使う
	int mnSelectMax = 3; // 選択肢最大
	bool mbStickInput = false; // スティックで連続入力を避けるため
	int mnChengeSelect; // 変わったかのやつ


	// 少し待つやつ
	bool mbShowResultText = false;  // ここでチュートリアルクリアを表示したい
	float mfResultDisplayTimer = 0.0f; // 文字を表示しておく時間（3.0f）

	// 暗くするやつ
	float blendscale2 = 0.0f;	// アルファ値
	bool startgame = false;     // 暗くするのフラグ
	int fadeout = 0;     // 画像 黒いやつ


	// リザルド画面に行くまでにとめるやつ
	float TIMEOVER = 3.0f;
	bool mbIsVictory = false; // 勝利したかどうかを保存


	// スキップしたときの文字の描画
	bool mbSkip = false; // skipされたか

	int mnHandle_Bag; // ステップの枠
};