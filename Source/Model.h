#pragma once

#include "DxLib.h"
#include <string>
#include "ModelUtility.h"
#include "ModelAnimation.h"
#include "SeparateModelAnimation.h"
#include <map>


// 前方宣言
class AttachmentModel;

class Model
{
public:
	//Model(std::string filename, VECTOR initPos);  // コンストラクタ

	// ★New★
	// コンストラクタ
	// note: 分割アニメーションを使うかどうかの設定を追加。
	// コンストラクタ仲で分岐しているからひとつで良い
	Model(std::string filename, VECTOR initPos, bool isSeparateAnimation = false);


	// すでにロード済みのハンドルをそのまま使う時に使う
	Model(int modelHandle, VECTOR initPos, bool isSeparateAnimation = false);


	// 読み込むモデルのファイル名 , 位置
	~Model(); // デストラクタ
	void Update(); // 更新
	void Draw();   // 描画

	// -------------アニメーション関係---------------------------
	// アニメーションの切り替え
	void ChangeAnimation(AnimationState state, float speed = 0.5f); // 速さ追加	 デフォ値追加
	// ループ設定
	void SetLoop(bool loop);
	void SetLoopFinishState(AnimationState state);
	// アニメーションのブレンド設定
	void SetAnimationBlend(bool isBlend);
	// 現在再生されているアニメーションの取得
	// Animationstateから呼びだす
	AnimationState GetNowState();
	bool IsAnimationLoopFinish();// アニメーションのループが終了しているか


	// -------------アタッチ関係---------------------------
	// アタッチモデル関連
	// アタッチメントを追加
	void AddAttachment(std::string filename, std::string attachFrameName);
	// アタッチモデルの座標取得　こっちは敵で使ってる
	VECTOR GetAttachmentPosition();


	// -------------ポジションto回転関係---------------------------
	VECTOR GetPosition() { return mvPosition; }        // 座標取得
	void SetPosition(VECTOR pos) { mvPosition = pos; } // 座標設定
	VECTOR GetRotation() { return mvRotation; }        // 回転取得
	void SetRotation(VECTOR rot) { mvRotation = rot; } // 回転設定

	// -------------SkyBox用---------------------------
	void SetScale(float scale);
	void SetTexture(std::string filename, int index = 0);

	// ★New★
	// アニメーションデータの追加
	// note: SeparateModelAnimation クラスへの橋渡し関数
	void AddAnimation(AnimationState state, std::string filename);

	// メインモデルのハンドルを返す
	int GetHandle() const { return mnHandle; }


	// ★New★
	// 初期行列の設定
	void SetupInitializeMatrix(std::string rootFrameName);

	// ★New★
	// 有効なフレームかどうかの判定
	bool ValidRootFrameIndex();

	float GetAnimTime(); // アニメーションの時間を返すやつ
	float GetTotalTiime(); // あにめのそう再生時間

	void SetAnimSpeed(float speed); // アニメの速さ変える

private:

	int mnHandle;      // 読み込んだモデルのハンドル
	VECTOR mvPosition; // 座標
	VECTOR mvRotation; // 回転
	// sukai
	float mfScale;
	int mnChangeTextureHandle;
	ModelAnimation* mpAnimation; // モデルアニメーションンクラスのポインタ
	AttachmentModel* mpAttachment; // アタッチモデルクラスのポインタ
	// (複数持たせたいときは std::vector や配列で管理すると良い)


	// ★New★
	// 分割読み込みバージョンのモデルアニメーションクラスのポインタ
	SeparateModelAnimation* mpSeparateAnimation;


	// あたらいいやつ
	MATRIX mmInitializeMatrix;  // ★New★ // モデル内部で移動をしている（であろう）フレームの初期行列
	int mnRootFrameIndex;       // ★New★ // モデル内部で移動をしている（であろう）フレームのインデックス（初期値 -1）


	// これが true の時はデストラクタで MV1DeleteModel を呼ばない
	bool mbIsSharedHandle = false;
};

// メモ
// std::map...IDと値をセットで保存できるもの
// 例えば数字とポインタとか、文字列とかもできるから神

