#pragma once

#include "Scene.h" // シーン継承したいからインクルード
#include <string>


class Title3D : public Scene
{

public:
	// コンストラクタ
	Title3D();
	// デストラクタ
	virtual ~Title3D();

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
	int mnFontSmall2;

	// 画像用
	int mnHandle_Bag;
	int mnHandle_Sky;

	// 選択するやつ
	int mnSelect;
	int mnSelectMax = 3; // 最大数

	bool mbStickInput = false; // スティックで連続入力を避けるため

	// チュートリアル選択関係
	bool mbSelectGame; // ゲームがえらればたときにチュートリアルをするかのフラグ
	int mnGameSelect; // チュートリアルか選択画面かのやつ
	int mngameSelectMax = 2;
	int mnHandel_Tutorial; // 選択の後ろに出すやつ

	bool mbInputName = false;    // 名前入力中かどうかのフラグ
	std::string mName = "Player";
	int mNameInputHandle = -1;   // 入力ハンドルの管理用

	int mnSelectChenge;     // 選択保存
	int mnGameSelectChenge; // ゲーム選択保存

};
