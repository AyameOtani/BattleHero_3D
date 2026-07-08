#include "DrawWeapon.h"

DrawWeapon::DrawWeapon(std::string filename, VECTOR initPos)
	:Object3D(initPos)
	, mfAngle(0.0f)
	, mnNowDraw(DrawSelectID::None)
	, mpModel(nullptr)
	, mvDirection(VGet(0,0,0))
{
	// LoadWeaponやってるからいらぬ
	prevTime = GetNowCount();
}

DrawWeapon::~DrawWeapon()
{
	for (auto& pair : mmDrawPlayer)
	{
		delete pair.second;
	}

	mmDrawPlayer.clear();
	mpModel = nullptr;
}

void DrawWeapon::LoadWeapon(DrawSelectID id, std::string filename, VECTOR initPos)
{
	// すでにロード済みなら何もしない
	if (mmDrawPlayer.find(id) != mmDrawPlayer.end())
	{
		return;
	}

	// ここでNEWしているから注視
	Model* model = new Model(filename, initPos);
	mmDrawPlayer[id] = model;
}

void DrawWeapon::SetDrawPlayer(DrawSelectID id)
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

	// モデルを変えるたびに回転リセット
	mfAngle = 0.0f;
}

void DrawWeapon::Update()
{
	Rotation();

	if (mpModel)
	{
		mpModel->Update();
	}
}

void DrawWeapon::Rotation()
{
	int now = GetNowCount();
	float dt = (now - prevTime) / 1000.0f;
	prevTime = now;

	const float ROTATE_SPEED = 1.2f; // rad / 秒（好きな値）
	mfAngle += ROTATE_SPEED * dt;




	//mfAngle += 0.02f; // すこしずず回転
	if (mfAngle >= DX_TWO_PI_F)
	{
		mfAngle -= DX_TWO_PI_F;
	}
	mvRotation.y = mfAngle;


	if (mpModel)
	{
		// これがないと変らない
		mpModel->SetRotation(mvRotation);
		mSelectEffect.Update(VGet(-posX, 70.0f, 0.0f));
	}
}

void DrawWeapon::Draw()
{
	if (mpModel)
	{
		mpModel->Draw(); // 描画
	}

	mSelectEffect.Draw();
}

void DrawWeapon::AllLoad()// ここで出したいモデルの読み込み
{
	// 読み込む
	// ここ表示用のは別でつくるのありかも
	LoadWeapon(Shild, "Resource/DrawWeapon/Shield/Shield.mqo", VGet(posX, posY, 0.0f));
	LoadWeapon(WriteGun, "Resource/DrawWeapon/Gun/BlackGun.mqo", VGet(posX, posY, 0.0f));
	LoadWeapon(BlackGun, "Resource/DrawWeapon/Gun/WhiteGun.mqo", VGet(posX, posY, 0.0f));
	LoadWeapon(BlueGun, "Resource/DrawWeapon/Gun/BlueGun.mqo", VGet(posX, posY, 0.0f));
	LoadWeapon(YARI, "Resource/DrawWeapon/Spear/Spear.mqo", VGet(posX, posY, 0.0f));
	LoadWeapon(GT, "Resource/DrawWeapon/GT/Guitar.mqo", VGet(posX, posY, 0.0f));
	


	EffectLoad();
}

void DrawWeapon::EffectLoad()
{
	mSelectEffect.Load("Resource/2d/RockOn.png"); // エフェクトをロード
	mSelectEffect.StartHold(VGet(posX, 70.0f, 0.0f));

	SetUseZBuffer3D(FALSE);
	SetWriteZBuffer3D(FALSE);
	SetUseLighting(TRUE);
}

