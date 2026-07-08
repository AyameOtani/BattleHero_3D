// 
// ★New★
// 新規追加ファイル
//


#include "DxLib.h"
#include "SeparateModelAnimation.h"
#include "Master.h"

// コンストラクタ
SeparateModelAnimation::SeparateModelAnimation(int modelHandle)
    : mnModelHandle(modelHandle)
    , mfAnimationTime(0.0f)
    , mnAnimationIndex(-1)
    , mfOldAnimationTime(0.0f)
    , mnOldAnimationIndex(-1)
    , mfAnimBlendRate(1.0f)
    , mnState(AnimationState::ANIMATION_MAX)    // 最初は最大値としておく
    , mbLoop(true)
    , mnLoopFinishState(AnimationState::ANIMATION_MAX)
    , mbLoopFinish(false)
    , mAnimationInfoList()
{
}

// デストラクタ
SeparateModelAnimation::~SeparateModelAnimation()
{
    // 追加読み込みしたモーションの削除
    if (!mAnimationInfoList.empty())
    {
        for (auto itr = mAnimationInfoList.begin(); itr != mAnimationInfoList.end(); )
        {
            auto temp = *itr;

            itr = mAnimationInfoList.erase(itr);
            MV1DeleteModel(temp->mnAnimationHandle);

            delete temp;
            temp = nullptr;
        }
    }
}

// 更新
void SeparateModelAnimation::Update()
{
    // モーションのブレンド率を進める
    if (mfAnimBlendRate < 1.0f)
    {
        mfAnimBlendRate += 0.1f;    // += 0.1f はブレンド速度。自由に変えてもok
        if (mfAnimBlendRate > 1.0f)
        {
            mfAnimBlendRate = 1.0f;
        }
    }


    // モーションの更新
    float fAnimTotalTime = 0.0f;
    if (mnAnimationIndex != -1)
    {
        // 総再生時間の取得
        fAnimTotalTime = MV1GetAttachAnimTotalTime(mnModelHandle, mnAnimationIndex);

        // モーションを進める
        mfAnimationTime += mfSpeed;

        // ループさせる
        if (mfAnimationTime > fAnimTotalTime)
        {
            // ループしない設定であれば
            if (!mbLoop)
            {
                // 次のモーションが設定されていない場合
                if (mnLoopFinishState == ANIMATION_MAX)
                {
                    // モーションはこれ以上進めず、処理を中断させる
                    mbLoopFinish = true;
                    return;
                }

                // ループ終了時のモーションへ変更
                ChangeAnimation(mnLoopFinishState, mfSpeed);
                // ブレンドはしない
                // ここ変更 0130
                SetAnimationBlend(true);
                // 変更されたので改めて総再生時間をとっておく
                fAnimTotalTime = MV1GetAttachAnimTotalTime(mnModelHandle, mnAnimationIndex);
            }

            mfAnimationTime = 0.0f;
        }

        // モーションを反映
        MV1SetAttachAnimTime(mnModelHandle, mnAnimationIndex, mfAnimationTime);

        // ブレンド率を設定
        MV1SetAttachAnimBlendRate(mnModelHandle, mnAnimationIndex, mfAnimBlendRate);
    }

    //// １つ前のモーションを更新
    //if (mnOldAnimationIndex != -1)
    //{
    //    // 総再生時間の取得
    //    fAnimTotalTime = MV1GetAttachAnimTotalTime(mnModelHandle, mnOldAnimationIndex);
    //    // モーションを進める
    //    mfOldAnimationTime += 0.5f;
    //    // ループさせる
    //    if (mfOldAnimationTime > fAnimTotalTime)
    //    {
    //        mfOldAnimationTime = 0.0f;
    //    }
    //    // モーションを反映
    //    MV1SetAttachAnimTime(mnModelHandle, mnOldAnimationIndex, mfOldAnimationTime);
    //    // ブレンド率を設定
    //    MV1SetAttachAnimBlendRate(mnModelHandle, mnOldAnimationIndex, 1.0f - mfAnimBlendRate);
    //}

    // 変更 0130 これでピクってなるのをやめている
    // １つ前のモーションを更新
    if (mnOldAnimationIndex != -1)
    {
        // 時間を進めない
        MV1SetAttachAnimTime(mnModelHandle, mnOldAnimationIndex, mfOldAnimationTime);

        // ブレンド率だけ下げる
        MV1SetAttachAnimBlendRate(
            mnModelHandle,
            mnOldAnimationIndex,
            1.0f - mfAnimBlendRate
        );

        // 完全にブレンド終わったら切り離す
        if (mfAnimBlendRate >= 1.0f)
        {
            MV1DetachAnim(mnModelHandle, mnOldAnimationIndex);
            mnOldAnimationIndex = -1;
        }
    }

}

// モーション切り替え
void SeparateModelAnimation::ChangeAnimation(AnimationState state, float speed, int index)
{
    // 切り替えようとしているモーションがすでに設定されている場合
    if (mnState == state)
    {
        mfSpeed = speed;
        return;     // 何もしない
    }

    // 切り替え先の番号を保持
    mnState = state;
    mfSpeed = speed;

    // ループ情報の初期化
    mbLoop = true;  // 設定が特にない場合はループさせる
    mnLoopFinishState = AnimationState::ANIMATION_MAX;  // ループ終了時のモーションは特になし
    mbLoopFinish = false;

    // １つ前のモーションが有効状態であれば
    if (mnOldAnimationIndex != -1)
    {
        // モーションのデタッチ（取り外す）
        MV1DetachAnim(mnModelHandle, mnOldAnimationIndex);
        mnOldAnimationIndex = -1;
    }

    // 現在のモーション状態を保持する
    mnOldAnimationIndex = mnAnimationIndex;
    mfOldAnimationTime = mfAnimationTime;

    // モーションのアタッチ
    mnAnimationIndex = MV1AttachAnim(mnModelHandle, index, GetAnimationHandle(state), FALSE);

    // 再生時間の初期化
    mfAnimationTime = 0.0f;

    // ブレンド状態を初期化
    // ブレンド率は、古いモーションが有効でない場合は1.0f（ブレンドしない状態）にしておく
    mfAnimBlendRate = (mnOldAnimationIndex == -1 ? 1.0f : 0.0f);
}

// モーションのブレンド設定
void SeparateModelAnimation::SetAnimationBlend(bool isBlend)
{
    if (isBlend)    // ブレンドする場合
    {
        // ブレンド率は、古いモーションが有効でない場合は1.0f（ブレンドしない状態）にしておく
        mfAnimBlendRate = (mnOldAnimationIndex == -1 ? 1.0f : 0.0f);
    }
    else    // ブレンドしない場合
    {
        // ブレンドしない状態にする
        mfAnimBlendRate = 1.0f;

        // ブレンドする必要がないので、古いモーションはデタッチしておく
        if (mnOldAnimationIndex != -1)
        {
            MV1DetachAnim(mnModelHandle, mnOldAnimationIndex);
            mnOldAnimationIndex = -1;
        }
    }
}

// モーション追加
void SeparateModelAnimation::AddAnimation(AnimationState state, std::string filename)
{
    // モーションモデル読み込み
   // int handle = MV1LoadModel(filename.c_str());
    int handle = Master::mpResourceManager->LoadModel(filename.c_str());

    if (handle == -1)
    {
        return;
    }

    // AnimationState と読み込んだハンドルの紐づけ
    AnimationInfo* pInfo = new AnimationInfo();
    pInfo->mState = state;
    pInfo->mnAnimationHandle = handle;
    mAnimationInfoList.push_back(pInfo);

    // NEUTRALモーション（待機モーション）が追加されたらモーション変更処理をしておく
    if (state == AnimationState::ANIMATION_NEUTRAL)
    {
        // 初期状態は待機モーションにしておく
        ChangeAnimation(AnimationState::ANIMATION_NEUTRAL, mfSpeed);
    }
}

// 対応したモーションハンドルの取得
int SeparateModelAnimation::GetAnimationHandle(AnimationState state)
{
    // そもそも空っぽの場合は探さない
    if (mAnimationInfoList.empty())
    {
        return -1;
    }

    for (auto itr = mAnimationInfoList.begin(); itr != mAnimationInfoList.end(); itr++)
    {
        auto temp = *itr;

        // 対応するモーションハンドルがあればそれを返す
        if (temp->mState == state)
        {
            return temp->mnAnimationHandle;
        }
    }

    // 見つからなかったら-1を返す
    return -1;
}