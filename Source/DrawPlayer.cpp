#include "DrawPlayer.h"
#include "PlayerAnim.h"

DrawPlayer::DrawPlayer(std::string filename, VECTOR initPos)
	:Object3D(initPos)
	, mfAngle(0.0f)
	, mnNowDraw(DrawSelectID::None)
	, mpModel(nullptr)
{
	// LoadPlayerやってるからいらぬ
	prevTime = GetNowCount();
}

DrawPlayer::~DrawPlayer()
{
	for (auto& pair : mmDrawPlayer)
	{
		delete pair.second;
	}

	mmDrawPlayer.clear();
	mpModel = nullptr;
}

// ここでmodelをよみこむ
void DrawPlayer::LoadPlayer(DrawSelectID id, std::string filename, VECTOR initPos)
{
	// すでにロード済みなら何もしない
	if (mmDrawPlayer.find(id) != mmDrawPlayer.end())
	{
		return;
	}

	// ここでNEWしているから注視
	bool isSeparate = (id != Hero); // プレイヤーじゃなかったらtrue
	Model* model = new Model(filename, initPos, isSeparate);
	mmDrawPlayer[id] = model;
}

void DrawPlayer::SetDrawPlayer(DrawSelectID id)
{
	auto itr = mmDrawPlayer.find(id);
	if (itr == mmDrawPlayer.end())
	{
		return; // 存在しない
	}
	// 同じなら何もしないように
	if (mnNowDraw == id)
	{
		return;
	}

	mpModel = itr->second;
	mnNowDraw = id;

	// モデルを変える旅に回転リセット
	mfAngle = 0.0f;
	// はじめだけモーションを変える
	mpModel->ChangeAnimation(ANIMATION_NEUTRAL);
}

// アニメーション追加
void DrawPlayer::AddAnimation(AnimationState state, std::string filename)
{
	mpModel->AddAnimation(state, filename);
}


void DrawPlayer::Update()
{
	Rotation();

	if (mpModel)
	{
		mpModel->Update();
	}
}

void DrawPlayer::Rotation()
{
	int now = GetNowCount();
	float dt = (now - prevTime) / 1000.0f;
	prevTime = now;

	const float ROTATE_SPEED = 1.2f; // rad / 秒（好きな値）
	mfAngle += ROTATE_SPEED * dt;


	if (mfAngle >= DX_TWO_PI_F)
	{
		mfAngle -= DX_TWO_PI_F;
	}
	mvRotation.y = mfAngle;


	if (mpModel)
	{
		// これがないと変らない
		mpModel->SetRotation(mvRotation);
		mSelectEffect.Update(VGet(-posX, posY, 0.0f));
	}
}

void DrawPlayer::Draw()
{
	if (mpModel)
	{
		mpModel->Draw(); // 描画
	}

	mSelectEffect.Draw();
}

void DrawPlayer::AllLoad()// ここで出したいモデルの読み込み
{
	// 読み込む Heroは今のままでよさげ
	LoadPlayer(Hero, "Resource/3d/Player/Hero.mv1", VGet(posX, posY, 0.0f));
	LoadPlayer(Zombi, "Resource/3d/Zombie/Zombie_Mesh.mv1", VGet(posX, posY, 0.0f));
	LoadPlayer(Guntai, "Resource/3d/Military/Guntai.mv1", VGet(posX, posY, 0.0f));
	LoadPlayer(Bob, "Resource/3d/Bob/Bob.mv1", VGet(posX, posY, 0.0f));
	LoadPlayer(Devil, "Resource/3d/Devil/Devil.mv1", VGet(posX, posY, 0.0f));

	//// 選択画面でTになるのを改善した
	PlayerAnim::LoadSelectAnim(mmDrawPlayer[Zombi],"Resource/3d/Zombie/Zombie_Mesh.mv1");
	PlayerAnim::LoadSelectAnim(mmDrawPlayer[Guntai], "Resource/3d/Military/Guntai.mv1");
	PlayerAnim::LoadSelectAnim(mmDrawPlayer[Bob], "Resource/3d/Bob/Bob.mv1");
	PlayerAnim::LoadSelectAnim(mmDrawPlayer[Devil], "Resource/3d/Devil/Devil.mv1");

	EffectLoad();
}

void DrawPlayer::EffectLoad()
{
	mSelectEffect.Load("Resource/displayEffects/tex_ui_compass.png"); // エフェクトをロード
	mSelectEffect.StartHold(VGet(posX, 1.0f, 0.0f));

	SetUseZBuffer3D(FALSE);
	SetWriteZBuffer3D(FALSE);
	SetUseLighting(TRUE);
}

