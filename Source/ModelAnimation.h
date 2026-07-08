#pragma once

#include <string>
#include <vector>
#include "ModelUtility.h"


class ModelAnimation
{
public:
	ModelAnimation(int modelHandle); // コンストラクタ
	~ModelAnimation(); // デストラクタ

	void Update(); // 更新

	//void ChangeAnimation(AnimationState state); // アニメーション切り替え処理
	// 引数にして受け取れるようにしないと次の切り替えが分からないから（）の中に入れる

	 // note: mixamo でダウンロードすると、
	// 0番目のアニメーションデータはダミーが入っているので、
	// デフォルトで適用するインデックスを1にしておく
	void ChangeAnimation(AnimationState state, float speed = 0.5, int index = 1); // アニメーション切り替え処理



	void SetLoop(bool isLoop) { mbLoop = isLoop; } // ループ設定
	// 終了時に再生するアニメーション
	void SetLoopFinishState(AnimationState state) { mnLoopFinishState = state; }
	// アニメーションのブレンド設定
	void SetAnimationBlend(bool isBlend);

	// 現在再生されているアニメーションの取得
	AnimationState GetNowState() { return mnState; }
	bool IsLoopFinish() { return mbLoopFinish; } // アニメーションのループが終了しているかどうか


	// ゲッターせったーを作る（スピード遅くするため）
	void SetAnimSpeed(float speed) { mfSpeed = speed; }
	float GetAnimSpeed() { return mfSpeed; }


	// 今再生しているアニメーションの秒数取得
	float GetAnimTime() const { return mfAnimationTime; }
	// 全体の長さの取得も入れとくか
	float GetTotalTime() const {return MV1GetAttachAnimTotalTime(mnModelHandle, mnAnimationIndex);}



private:
	int mnModelHandle;        // モデルのハンドル

	float mfAnimationTime;    // 再生しているアニメーションの現在の再生時間
	int mnAnimationIndex;     // 再生しているアニメーションのインデックス

	float mfOldAnimationTime; // 1つ前のアニメーション再生時間
	int mnOldAnimationIndex;  // 1つ前のアニメーションのインデックス

	float mfAnimBlendRate;    // モーションの切り替わり度合

	AnimationState mnState;   // 再生しているアニメーションの番号


	bool mbLoop;                      // モーションをループさせるか
	AnimationState mnLoopFinishState; //ループが終わった時に再生したいアニメーション番号

	bool mbLoopFinish;                // モーションが終わったかどうか

	float mfSpeed; // アニメの速さのため
};
