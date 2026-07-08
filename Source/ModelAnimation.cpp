#include "DxLib.h"
#include "ModelAnimation.h"

ModelAnimation::ModelAnimation(int modelHandle)
	: mnModelHandle(modelHandle)  // 初期化する
	, mfAnimationTime(0.0f)       // 再生時間０スタート
	, mnAnimationIndex(-1)        // ０スタートになる可能性が高いので -1 にする
	, mfOldAnimationTime(0.0f)
	,mnOldAnimationIndex(-1)
	,mfAnimBlendRate(1.0f)        // 1.0
	, mnState(AnimationState::ANIMATION_MAX) // 最初は最大にしておく
	, mbLoop(true)
	, mnLoopFinishState(AnimationState::ANIMATION_MAX)
	, mbLoopFinish(false)
{
	// アニメーションで移動をしているフレーム番号を検索する
	// root が座標移動している(モデルによって違う)
	int moveAnimFrameIndex = MV1SearchFrame(mnModelHandle, "root");

	// 移動を無効にする
	MV1SetFrameUserLocalMatrix(
		mnModelHandle,
		moveAnimFrameIndex,
		MV1GetFrameLocalMatrix(mnModelHandle, moveAnimFrameIndex)
	);


	// 初期状態は待機モーションにしておく
	ChangeAnimation(AnimationState::ANIMATION_NEUTRAL);
}

ModelAnimation::~ModelAnimation()
{

}

void ModelAnimation::Update()
{
	// アニメーションのブレンド率を進める
	// 今は10% ずつ切り替えている
	if (mfAnimBlendRate < 1.0f)
	{
		mfAnimBlendRate += 0.10f;    // += 0.1f はブレンド速度。自由に変えてもOK
		if (mfAnimBlendRate > 1.0f) // 1.0f より大きくなったら
		{
			mfAnimBlendRate = 1.0; // 1.0f にする
		}
	}

	// アニメーションの更新
	float fAnimationTime = 0.0f; // 初期化
	if (mnAnimationIndex != -1) // -1じゃなかったら
	{
		// 総再生時間の取得
		fAnimationTime = MV1GetAttachAnimTotalTime(mnModelHandle, mnAnimationIndex);

		// アニメーションを進める
		//mfAnimationTime += mfSpeed; // 速さ設定
		mfAnimationTime += mfSpeed; // 0.05ぐらいがよき？ // 追記　保存したやつを足してる

		// ループさせる
		// 総再生時間を超えているなら
		if (mfAnimationTime > fAnimationTime)
		{
			// ループしない設定であれば
			if (!mbLoop)
			{
				// 次のアニメーションが設定されていない場合
				if (mnLoopFinishState == ANIMATION_MAX)
				{
					// アニメーションはこで以上進めず、処理を中断させる
					mbLoopFinish = true;
					return;
				}

				// ループ終了のアニメーションへ変更
				ChangeAnimation(mnLoopFinishState);

				//ブレンドさせない処理を呼びだす
				SetAnimationBlend(false);

				// 変更後の総再生時間の取得
				fAnimationTime = MV1GetAttachAnimTotalTime(mnModelHandle, mnAnimationIndex);
			}

			mfAnimationTime = 0.0f;
		}
		
		// アニメーションを反映
		// このアニメーションをこの再生時間で再生してと指示をしている
		MV1SetAttachAnimTime(mnModelHandle, mnAnimationIndex, mfAnimationTime);

		// ブレンド率を設定 (大事★)
		// 1.0fだったら 100% こっちを適用する
		// 今は10% 反映することになっている
		MV1SetAttachAnimBlendRate(mnModelHandle, mnAnimationIndex, mfAnimBlendRate);
	}

	   
	// 1つ前のアニメーションを更新
	if (mnOldAnimationIndex != -1) // -1じゃなかったら
	{
		// 総再生時間の取得
		fAnimationTime = MV1GetAttachAnimTotalTime(mnModelHandle, mnOldAnimationIndex);

		// アニメーションを進める
		mfOldAnimationTime += mfSpeed; // 追記　保存したやつを足してる

		// ループさせる
		// 総再生時間を超えているなら
		if (mfOldAnimationTime > fAnimationTime)
		{
			mfOldAnimationTime = 0.0f;

		}
		// アニメーションを反映
		// このアニメーションをこの再生時間で再生してと指示をしている
		MV1SetAttachAnimTime(mnModelHandle, mnOldAnimationIndex, mfOldAnimationTime);

		// ブレンド率を設定
		// mfAnimBlendRate が1.0fになっていたら0.0f になり、反映しない
		// 90% からどんどん減っていく
		MV1SetAttachAnimBlendRate(mnModelHandle, mnOldAnimationIndex, 1.0f - mfAnimBlendRate);
	}

}

// アニメーション切り替え
void ModelAnimation::ChangeAnimation(AnimationState state, float speed,int index)
{
	// 切り替えようとしているアニメーションがすでに設定されている場合
	if (mnState == state)
	{
		mfSpeed = speed;
		return;     // 何もしない
	}

	// 切り替え先の番号を保持
	mnState = state;
	mfSpeed = speed; // 速度を保存


	// ループ情報の初期化
	mbLoop = true;  // 設定が特にない場合はループさせる
	mnLoopFinishState = AnimationState::ANIMATION_MAX;  // ループ終了時のアニメーションは特になし
	mbLoopFinish = false;

	// １つ前のアニメーションが有効状態であれば
	if (mnOldAnimationIndex != -1)
	{
		// アニメーションのデタッチ（取り外す）
		MV1DetachAnim(mnModelHandle, mnOldAnimationIndex);
		mnOldAnimationIndex = -1;
	}

	// 現在のアニメーション状態を保持する
	mnOldAnimationIndex = mnAnimationIndex;
	mfOldAnimationTime = mfAnimationTime;

	// アニメーションのアタッチ
	mnAnimationIndex = MV1AttachAnim(mnModelHandle, (int)state);

	// 再生時間の初期化
	mfAnimationTime = 0.0f;

	// ブレンド状態を初期化
	// ブレンド率は、古いモーションが有効でない場合は1.0f（ブレンドしない状態）にしておく
	mfAnimBlendRate = (mnOldAnimationIndex == -1 ? 1.0f : 0.0f);
}

// アニメーション切り替え
// 変えたくなったら引数に強制的に変えるフラグを作っても良い
//void ModelAnimation::ChangeAnimation(AnimationState state)
//{
//	// 切り替えようとしているアニメーションがすでに設定されている場合
//	if (mnState == state)
//	{
//		return; // 何もしない
//	}
//
//	// ループ情報の初期化
//	mbLoop = true; // 設定が特にない場合はループされる
//	mnLoopFinishState = AnimationState::ANIMATION_MAX; // 終了時のアニメーションは特になし
//	mbLoopFinish = false; // アニメーションが切り替わった直後だとモーションが終わっってないからfalse
//	
//
//
//	// 切り替え先の番号を保持    ここで切り替わる
//	mnState = state;    // 切り替え処理を始める
//
//	// 1つ前のアニメーションが有効状態であれば
//	// 再生されていたものがあれば
//	if (mnOldAnimationIndex != -1)
//	{
//		// アニメーションのデタッチ (取り外す)
//		MV1DetachAnim(mnModelHandle, mnOldAnimationIndex);
//		mnOldAnimationIndex = -1; // 無効状態にする
//	}
//	
//	// 現在のアニメーション状態を保持する
//	mnOldAnimationIndex = mnAnimationIndex;
//	mfOldAnimationTime = mfAnimationTime;
//
//	// アニメーションのアタッチ
//	mnAnimationIndex = MV1AttachAnim(mnModelHandle, (int)state);
//	
//	// 再生時間の初期化 切り替えた先は初めから再生したいので 0 に設定
//	mfAnimationTime = 0.0f;
//
//	// ブレンド状態を初期化
//	// ブレンド率は、古いモーションが有効でない場合は
//	// 1.0f （ブレンドしない状態）にしておく
//	mfAnimBlendRate = (mnOldAnimationIndex == -1 ? 1.0f : 0.0f);
//	// ★★★　-1 ですか？　イエス＝1.0f : ノー＝0.0f という意味
//	
//}

// アニメーションのブレンド設定
void ModelAnimation::SetAnimationBlend(bool isBlend)
{
	// ブレンドする場合
	if (isBlend)
	{
		// ブレンド状態を初期化
	    // ブレンド率は、古いモーションが有効でない場合は
	    // 1.0f （ブレンドしない状態）にしておく
		mfAnimBlendRate = (mnOldAnimationIndex == -1 ? 1.0f : 0.0f);
	}
	else     // しない場合
	{
		// ブレンドしない状態にする ブレンドレートを変えている
		mfAnimBlendRate = 1.0f;

		// ブレンドする必要はないので、古いアニメーションはデタッチしておく
		if (mnOldAnimationIndex != -1)
		{
			MV1DetachAnim(mnModelHandle, mnOldAnimationIndex);
			mnOldAnimationIndex = -1;
		}
	}
}
