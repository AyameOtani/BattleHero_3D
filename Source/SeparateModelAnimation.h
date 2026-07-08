#pragma once

// 
// ★New★
// 新規追加ファイル
//

#include <string>
#include <vector>
#include "ModelUtility.h"

class SeparateModelAnimation
{
public:
    SeparateModelAnimation(int modelHandle);   // コンストラクタ
    ~SeparateModelAnimation();  // デストラクタ

    void Update();  // 更新

    // note: mixamo でモーションをダウンロードすると、
    //       0番目のモーションデータはダミーが入っているらしいので、
    //       デフォルトで適用するインデックスを 1 にしておく
    void ChangeAnimation(AnimationState state, float speed = 0.5, int index = 1); // モーション切り替え処理

    // モーションデータの追加
    // note: 分割されているモーションをデータとして登録する。
    void AddAnimation(AnimationState state, std::string filename);

    // 対応したモーションハンドルの取得
    // note: vector配列にデータが格納されているため、
    //       毎回ハンドルを検索する手間が出てきてしまうので、
    //       それを行うための関数。
    int GetAnimationHandle(AnimationState state);


    // ループ設定
    void SetLoop(bool isLoop) { mbLoop = isLoop; }
    // ループ終了時に再生するモーション
    void SetLoopFinishState(AnimationState state) { mnLoopFinishState = state; }
    // モーションのブレンド設定
    void SetAnimationBlend(bool isBlend);

    // 現在再生されているモーションの取得
    AnimationState GetNowState() { return mnState; }
    // モーションのループが終了しているかどうか
    bool IsLoopFinish() { return mbLoopFinish; }


    // 今再生しているアニメーションの秒数取得
    float GetAnimTime() const { return mfAnimationTime; }
    // 全体の長さの取得も入れとくか
    float GetTotalTime() const { return MV1GetAttachAnimTotalTime(mnModelHandle, mnAnimationIndex); }

    // アニメはやく セッター
    void SetAnimSpeed(float speed) { mfSpeed = speed; }

private:
    int mnModelHandle;  // モデルのハンドル

    float mfAnimationTime;  // 再生しているモーションの現在の再生時間
    int mnAnimationIndex;   // 再生しているモーションのインデックス

    float mfOldAnimationTime;   // 1つ前のモーション再生時間
    int mnOldAnimationIndex;    // １つ前のモーションのインデックス

    float mfAnimBlendRate;      // モーションの切り替わり度合

    AnimationState mnState; // 現在再生しているモーションの番号

    bool mbLoop;            // モーションをループさせるかどうか
    AnimationState mnLoopFinishState;   // ループが終わった時に再生したいモーション番号
    bool mbLoopFinish;      // モーションループが終わったかどうか

    // ★New★
    // モーションデータのリスト
    std::vector<AnimationInfo*> mAnimationInfoList;

    float mfSpeed = 0.5;
};