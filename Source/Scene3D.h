#pragma once
#include "Scene.h" // シーン参照したいからインクルード
#include "DxLib.h"


class Baria;
class Object3D;
class Scene3D : public Scene
{

public:
	// コンストラクタ
	Scene3D();
	// デストラクタ
	virtual ~Scene3D();
									 
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

private:
	int mnSceneScreen = -1; // オフスクリーン用のハンドル

	// 追加
	bool mbDrawFlag = false; // オブジェクトが配置されたか (敵が出てくる前のシーン還移を防ぐため)
	bool mbTimerFlag = false; // タイマーを一回だけ初期化するため

	int mnTime = 0; // 呼びだしたタイマーをメンバにいれるため



	// 開始のカウントダウン
	bool mbTimeStart = false;  // 時間を減らしてよいか
	bool mbDrawOK = false;     // 描画されたかのフラグ
	float mfCount = 4.0f;      // 開始までのカウント  0のときにGOって出したいから４にしてる
	bool  mbStartCount = false;//　カウントを開始するかのフラグ
	int mnHandle_Count1; // カウントダウンのとき
	int mnHandle_Count2; // カウントダウンのとき
	int mnHandle_CountUI1; // 上と同じく
	int mnHandle_CountUI2; // 上と同じく


	int mnFontBig; // 大きいフォント　カウントで使う
	int mnFontNormal; // ふつうのフォント
	int mnFontSmall; // 小さいやつ


	// ポーズ画面関係
	bool mbPose = false; // 一時停止をするかどうかのフラグ
	int mnHandle_PoseBag; // ポーズのときに出す黒い画像
	int mnHandle_Pose; // ポーズ中の背景画像
	int mnHandle_PoseSelect; // 選択画像BOX
	int mnSelect; // 選択で使う
	int mnSelectMax = 4; // 選択肢最大
	bool mbStickInput = false; // スティックで連続入力を避けるため
	int mnChengeSelect;


	// リザルド画面に行くまでにとめるやつ
	float TIMEOVER = 2.0f;
	int mfGraphX_R = 800; // 右画像スライド
	int mfGraphX_L = -800; // 左画像スライド
	int mnHandle_GO; // GO
	float mfGoSize = 0.2f; // GOの文字サイズ　どんどｎ大きく成る

	int mfAlpha = 255; // 透過 かうんとのやつ

	// 少し待つやつ
	bool mbShowResultText = false;  // 「VICTORY/DEFEAT」を表示するか
	float mfResultDisplayTimer = 0.0f; // 文字を表示しておく時間（3.0f）

	// 暗くするやつ
	float blendscale2 = 0.0f;	// アルファ値
	bool mbEndGame = false;     // 暗くするのフラグ
	int mnHandle_BlackBag = 0;     // 画像 黒いやつ


	bool TimUp = false; // 時間切れになったかのフラグ
	int mnTimeUpHandle; // 時間切れのときに出す画像
	int mnHandle_TimeUpUI; // 時間切れのときの画像２


	int mnHandle_Sousa; // 説明の画像
	bool mbSousa = false; // 操作説明画像を出すかどうか

	bool mbIsVictory = false; // 勝利したかどうかを保存

	float mfFadeAlpha = 0.0f; // 開始透過
	bool mbFadeIn = false; // ふぇーどいんが完了したか

	bool mbBGMStarted = false; // BGMを流すかの奴

	int mPrevCount = -1;  // 前フレームのカウント
	int mnVolume = 120; // 音量

	bool mbDoorSe = false; // ドアしめる「やつ

};

// 地面に格子状の線を描画する
	// note: 必要がなくなった時はコメントアウトする
	//const int count = 51;          // 個数
	//const float distance = 500.0f; // 距離
	//for (int i = 0; i < count; i++)
	//{
	//	float base = (count / 2 - i) * -distance;
	//	// 横線の描画
	//	DrawLine3D(
	//		VGet(-distance * (count / 2), 0.0f, base),
	//		VGet(distance * (count / 2), 0.0f, base),
	//		GetColor(255, 255, 255)
	//	);
	//	// 縦線の描画
	//	DrawLine3D(
	//		VGet(base, 0.0f , -distance * (count / 2)),
	//		VGet(base, 0.0f, distance * (count / 2)),
	//		GetColor(255, 255, 255)
	//	);
	//}
	// 
