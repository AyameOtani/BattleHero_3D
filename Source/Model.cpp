#include "Model.h"
#include "AttachmentModel.h"
#include "WeaponManager.h"
#include "Master.h"

// アニメーション追加
void Model::AddAnimation(AnimationState state, std::string filename)
{
	if (mpSeparateAnimation != nullptr)
	{
		mpSeparateAnimation->AddAnimation(state, filename);
	}
}

// ハンドルを直接受け取るコンストラクタ
Model::Model(int modelHandle, VECTOR initPos, bool isSeparateAnimation)
	: mnHandle(modelHandle)
	, mvPosition(initPos)
	, mbIsSharedHandle(true)      // 共有ハンドルなので true
	, mpAttachment(nullptr)
	, mvRotation(VGet(0.0f,0.0f,0.0f))
	, mfScale(1.0f)
	, mnChangeTextureHandle(-1)
	, mnRootFrameIndex(-1)
	, mmInitializeMatrix(MGetIdent())
{
	// アニメーションクラスの生成
	if (isSeparateAnimation)
	{
		mpSeparateAnimation = new SeparateModelAnimation(mnHandle);
		mpAnimation = nullptr;
	}
	else
	{
		mpAnimation = new ModelAnimation(mnHandle);
		mpSeparateAnimation = nullptr;
	}
}

// コンストラクタ
Model::Model(std::string filename, VECTOR initPos, bool isSeparateAnimation)
	: mvPosition(initPos)
	, mpAttachment(nullptr)
	, mfScale(1.0f)
	, mvRotation(VGet(0.0f, 0.0f, 0.0f))
	, mnChangeTextureHandle(-1)
	, mnRootFrameIndex(-1)              // ★New★
	, mmInitializeMatrix(MGetIdent())   // ★New★
{
	// モデルの読み込み
	//mnHandle = MV1LoadModel(filename.c_str()); ★New★
	mnHandle = Master::mpResourceManager->LoadModel(filename.c_str()); // 変更した

	// 条件分岐を追加
	if (isSeparateAnimation)
	{
		// 分割アニメーションクラスの生成
		mpSeparateAnimation = new SeparateModelAnimation(mnHandle);
		mpAnimation = nullptr;
	}
	else
	{
		// 通常アニメーションクラスの生成
		mpAnimation = new ModelAnimation(mnHandle);
		mpSeparateAnimation = nullptr;
	}
}

// デストラクタ
Model::~Model()
{
	// アニメーショクラスの破棄
	if (mpAnimation != nullptr)
	{
		delete mpAnimation;
	}

	// ★New★
	// 分割アニメーションクラスの破棄
	if (mpSeparateAnimation != nullptr)
	{
		delete mpSeparateAnimation;
	}

	// アタッチモデルのクラス破棄
	if (mpAttachment != nullptr)
	{
		mpAttachment->SetDeleteFlag(true);
	}

	// テクスチャを切り替えている場合はそのテクスチャの破棄
	if (mnChangeTextureHandle != -1)
	{
		DeleteGraph(mnChangeTextureHandle);
	}

	// 共有ハンドルGameManagerで管理していない場合だけ削除する
	if (mbIsSharedHandle == false)
	{
		MV1DeleteModel(mnHandle);
	}

	// 読み込んだモデルの削除
	// note: 読み込んだモデルは勝手に破棄してくれないので、
	// 必要なくなったら手動で破棄する。
	// MV1DeleteModel(mnHandle);
}


// ★New★
// 初期行列の設定
void Model::SetupInitializeMatrix(std::string rootFrameName)
{
	// 指定されたフレーム名が存在するか探す
	mnRootFrameIndex = MV1SearchFrame(mnHandle, rootFrameName.c_str());

	// フレームが有効なインデックスだった場合
	if (ValidRootFrameIndex())
	{
		// MV1GetFrameLocalMatrix でも出来るが、
		// 用途的には MV1GetFrameBaseLocalMatrix が合ってそうなので、そっちを使ってみる
		//mmInitializeMatrix = MV1GetFrameLocalMatrix(mnHandle, mnRootFrameIndex);
		mmInitializeMatrix = MV1GetFrameBaseLocalMatrix(mnHandle, mnRootFrameIndex);
	}
}

// ★New★
// 有効なフレームかどうかの判定
bool Model::ValidRootFrameIndex()
{
	// MV1SearchFrame では -1 か -2 がエラーで帰ってくるので、その判定用
	return (mnRootFrameIndex != -1 && mnRootFrameIndex != -2);
}




// アタッチモデルの座標取得 敵で使ってるから消さない
VECTOR Model::GetAttachmentPosition()
{
	if (mpAttachment != nullptr)
	{
		VECTOR vec = VGet(0.0f, -50.0f, 0.0f);
		// 行列の取得  剣のモデルのマトリックスの取得
		MATRIX matrix = MV1GetFrameLocalWorldMatrix(mpAttachment->GetHandle(), 0);
		// 行列情報をもとに座標変換する
		vec = VTransform(vec, matrix);
		return vec;
	}
	// アタッチメントがない場合は原点を返しておく これはどっちにも必要
	return VGet(0.0f, 0.0f, 0.0f);
}

void Model::Update()
{
	// ★New★
	// 固定した行列を一旦解除する
	if (ValidRootFrameIndex())
	{
		MV1ResetFrameUserLocalMatrix(mnHandle, mnRootFrameIndex);
	}


	if (mpAnimation != nullptr)
	{
		// アニメーションの更新
		mpAnimation->Update();
	}

	// 分割アニメーションの更新
	if (mpSeparateAnimation != nullptr)
	{
		mpSeparateAnimation->Update();
	}

	// ★New★
	// アニメーションで移動している成分だけを初期値に戻すことで、アニメーションでの移動を無効化しているようにみせる。
	// note: 実験的な実装なので、上手く行かないアニメーションもあるかも。
	if (ValidRootFrameIndex())
	{
		auto Matrix = MV1GetFrameLocalMatrix(mnHandle, mnRootFrameIndex);
		MATRIX result = Matrix;
		result.m[3][0] = mmInitializeMatrix.m[3][0];
		//result.m[3][1] = mmInitializeMatrix.m[3][1]; // Y成分だけは一旦反映しないようにしておく（反映してもよいが、見た目が少しおかしくなることが多い）
		result.m[3][2] = mmInitializeMatrix.m[3][2];
		MV1SetFrameUserLocalMatrix(mnHandle, mnRootFrameIndex, result);
	}


	// 座標設定
	MV1SetPosition(mnHandle, mvPosition);

	// 回転設定
	MV1SetRotationXYZ(mnHandle, mvRotation);
}

void Model::Draw()
{
	// モデルの描画
	MV1DrawModel(mnHandle);
}

// アニメーション切り替え
void Model::ChangeAnimation(AnimationState state, float speed)
{
	// ★New★
	// 通常 or 分割のどちらかを使っているかで分岐
	if (mpAnimation != nullptr)
	{
		mpAnimation->ChangeAnimation(state, speed);
	}
	if (mpSeparateAnimation != nullptr)
	{
		mpSeparateAnimation->ChangeAnimation(state, speed);
	}
}

// ループ設定
void Model::SetLoop(bool loop)
{
	// ★New★
	// 通常 or 分割のどちらかを使っているかで分岐
	if (mpAnimation != nullptr)
	{
		mpAnimation->SetLoop(loop);
	}
	if (mpSeparateAnimation != nullptr)
	{
		mpSeparateAnimation->SetLoop(loop);
	}
}

void Model::SetLoopFinishState(AnimationState state)
{
	// ★New★
	// 通常 or 分割のどちらかを使っているかで分岐
	if (mpAnimation != nullptr)
	{
		mpAnimation->SetLoopFinishState(state);
	}
	if (mpSeparateAnimation != nullptr)
	{
		mpSeparateAnimation->SetLoopFinishState(state);
	}
}

// アニメーションのブレンド設定
void Model::SetAnimationBlend(bool isBlend)
{
	// ★New★
	// 通常 or 分割のどちらかを使っているかで分岐
	if (mpAnimation != nullptr)
	{
		mpAnimation->SetAnimationBlend(isBlend);
	}
	if (mpSeparateAnimation != nullptr)
	{
		mpSeparateAnimation->SetAnimationBlend(isBlend);
	}
}

AnimationState Model::GetNowState()
{
	// ★New★
   // 通常 or 分割のどちらかを使っているかで分岐
   // note: （ほぼありえないが）もしどちらも無ければ、特に設定のない最大値を返すようにする
	AnimationState ret = AnimationState::ANIMATION_MAX;

	if (mpAnimation != nullptr)
	{
		ret = mpAnimation->GetNowState();
	}
	if (mpSeparateAnimation != nullptr)
	{
		ret = mpSeparateAnimation->GetNowState();
	}

	return ret;
}

bool Model::IsAnimationLoopFinish()
{
	// ★New★
	// 通常 or 分割のどちらかを使っているかで分岐
	// note: （ほぼありえないが）もしどちらも無ければ、false を返すようにしておく

	bool ret = false;

	if (mpAnimation != nullptr)
	{
		ret = mpAnimation->IsLoopFinish();
	}
	if (mpSeparateAnimation != nullptr)
	{
		ret = mpSeparateAnimation->IsLoopFinish();
	}

	return ret;
}

// アタッチメントを追加
void Model::AddAttachment(std::string filename, std::string attachFrameName)
{
	// アタッチ先のフレーム番号を取得
	int frameIndex = MV1SearchFrame(mnHandle, attachFrameName.c_str());

	// アタッチメントモデルの作成
	// frameIndexは武器を持つところを探してくれたもの
	mpAttachment = new AttachmentModel(filename, mnHandle, frameIndex);
}


void Model::SetScale(float scale)
{
	MV1SetScale(mnHandle, VGet(scale, scale, scale));
}

void Model::SetTexture(std::string filename, int index)
{
	// テクスチャをすでに切り替えている場合はそのテクスチャの破棄
	if (mnChangeTextureHandle != -1)
	{
		DeleteGraph(mnChangeTextureHandle);
	}

	// テクスチャの読み込み
	//mnChangeTextureHandle = LoadGraph(filename.c_str());
	mnChangeTextureHandle = Master::mpResourceManager->LoadGraphics(filename);

	// 読み込んだテクスチャの反映
	MV1SetTextureGraphHandle(mnHandle, index, mnChangeTextureHandle, FALSE);
}

// 今再生しているアニメの時間取得
float Model::GetAnimTime()
{
	if (mpAnimation != nullptr)
	{
		return mpAnimation->GetAnimTime();
	}
	if (mpSeparateAnimation != nullptr)
	{
		return mpSeparateAnimation->GetAnimTime();
	}

	return 0.0f; // アニメーションがなかったら 0
}
// 今再生しているアニメの合計時間取得
float Model::GetTotalTiime()
{
	if (mpAnimation != nullptr)
	{
		return mpAnimation->GetTotalTime();
	}
	if (mpSeparateAnimation != nullptr)
	{
		return mpSeparateAnimation->GetTotalTime();
	}
	return 0.0f; // アニメーションがなかったら 0
}

// アニメーのはやさ
void Model::SetAnimSpeed(float speed)
{
	if (mpAnimation != nullptr)
	{
		mpAnimation->SetAnimSpeed(speed); // ここで今回作った関数を呼ぶ！
	}
	if (mpSeparateAnimation != nullptr)
	{
		// 分割アニメ側にも同じ SetAnimSpeed を作っておけばここも呼べます
		mpSeparateAnimation->SetAnimSpeed(speed);
	}
}