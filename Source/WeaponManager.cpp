#include "WeaponManager.h"
#include "AttachmentModel.h"
#include "Master.h"
#include "Model.h"


WeaponManager::WeaponManager(int ModelHandle)
	: mnModelHandle(ModelHandle)
	, mpCurrentWeapon(nullptr)
	, mnNowWeapon(WeaponID::None)
{

}


WeaponManager::~WeaponManager()
{
     // ここで二重になるから書かないほうがよい
}


// 追加したもの 0101
// ID ファイル名フレーム名をとっている  読み込んで登録している Attachment改造
void WeaponManager::Lord_Base_Weapon(WeaponID id, std::string filename,
	std::string attachFrameName, float attack, float bulletMax, bool isEnemy)
{
	// アタッチ先のフレーム番号を取得
	int frameIndex = MV1SearchFrame(mnModelHandle, attachFrameName.c_str());

	// 武器をアタッチ先のフレームにつける処理
	// この武器は誰のフレームに追従する　という感じ
	AttachmentModel* weapon = new AttachmentModel(filename, mnModelHandle, frameIndex);
	weapon->SetVisible(false); // 作った武器を非表示にしている ここで表示すると重なっちゃう
	// 作った武器をIDとして登録している


	// 武器ごとに回転を変えたい
	weapon->SetWeaponRotation(RotateWeapon(id, isEnemy));



	WeaponData date{}; // 点々がでるから{}追加した
	date.model = weapon; //モデルと攻撃力をそれぞれ入れている
	date.attack = attack;
	date.bulletMax = bulletMax; // 弾数の代入
	date.NowBullet = bulletMax; // 最初だけ最大数

	// 初めはリロードなし
	date.reload = false;
	date.reloadCount = 0.0f;

	mmWeapons[id] = date; // ここで設定しているからSetWeaponが呼べる
}



void WeaponManager::Set_Base_Weapon(WeaponID id)
{
	// find... 見つかったら指して見つからなかったらendを返すやつ
	auto itr = mmWeapons.find(id);

	// findで探しても見つからなかったら何もしないで終わru
	if (itr == mmWeapons.end())
	{
		return; // 存在しないID
	}
	// 今の武器と装備しようとした武器が同じだったらreturnする
	// second... IDに対応している武器　mapの右側なのでAttachmentModel* を指している
	// ちなみにfirstはmapの左のIDを指してる
	if (mpCurrentWeapon == itr->second.model)
	{
		mbTheSameWeapon = true; // 同じ武器だったらのフラグON
		return;
	}

	if (mpCurrentWeapon)
	{
		mpCurrentWeapon->SetVisible(false);
	}
	mbTheSameWeapon = false; // 同じ武器のフラグを解除

	// ここで指定したIDの武器を 現在装備中の武器として設定いしている
	// 存在しないIDを指したらerrorなったからitrのほうが安全っぽい
	mpCurrentWeapon = itr->second.model;
	mpCurrentWeapon->SetVisible(true);
	
	mfAttack = itr->second.attack; // 攻撃力の代入
	mfBulletMax = itr->second.bulletMax; // 弾数の代入?
	mnNowWeapon = id;

	// ここで今の弾を代入しちゃったら切り換えたときに終わるからまだまち
}


// アタッチモデルの座標取得 元のやつを改造
VECTOR WeaponManager::Get_Base_WeaponPosition()
{
	//新しいアタッチの座標取得   武器モデルのワールド行列からとる
	if (mpCurrentWeapon != nullptr)// 武器の位置を返す
	{
		// 今装備している武器のポジションを返している
		return mpCurrentWeapon->GetWorldPosition(); // AttachmentModel の位置
	}

	// アタッチメントがない場合は原点を返しておく これはどっちにも必要
	return VGet(0.0f, 0.0f, 0.0f);
}

// GetWorldPosition() は GetWeaponPosition() を呼ぶだけでOK
VECTOR WeaponManager::GetWorldPosition() const
{
	if (mpCurrentWeapon)
	{
		return mpCurrentWeapon->GetWorldPosition();
	}
	return VGet(0.0f, 0.0f, 0.0f); // なにもなかったら原点で返している
}

//
// アタッチしている武器のふーれむを帰る
VECTOR WeaponManager::GetFrameWorldPosition(const std::string& frameName) const
{
	// 装備していなかったら原点を返す
	if (!mpCurrentWeapon)
	{
		return VGet(0, 0, 0);
	}

	// アタッチメントframeの位置を聞くため 位置が帰ってkる
	return mpCurrentWeapon->GetFrame(frameName);
}

// 球数を減らすやつ
void WeaponManager::SubBullet(int dan)
{
	auto itr = mmWeapons.find(mnNowWeapon);
	if (itr == mmWeapons.end())
	{
		return;
	}

	itr->second.NowBullet -= dan;
	if (itr->second.NowBullet <= 0)
	{
		itr->second.NowBullet = 0; // 0いかになったら０にする
		itr->second.reload = true; // リロードをtrueにする
		itr->second.reloadCount = 0; // 数を入れる
	}
}


// いま装備している武器の弾の数を返すやつ
float WeaponManager::Get_Base_Bullet()
{
	auto itr = mmWeapons.find(mnNowWeapon);
	if (itr == mmWeapons.end())
	{
		return 0.0f;
	}

	return itr->second.NowBullet;
}
// 最大数を返す
float WeaponManager::Get_Base_BulletMax()
{
	auto itr = mmWeapons.find(mnNowWeapon);
	if (itr == mmWeapons.end())
	{
		return 0.0f;
	}

	return itr->second.bulletMax;
}






// リロード中に増やすゲージ
void WeaponManager::AddCount()
{
	auto itr = mmWeapons.find(mnNowWeapon);
	if (itr == mmWeapons.end())
	{
		return;
	}

	for (auto& weapon : mmWeapons)
	{
		if (weapon.second.reload)
		{
			weapon.second.reloadCount++;

			if (weapon.second.reloadCount >= MAX_RELOAD)
			{
				weapon.second.reload = false;
				weapon.second.reloadCount = 0;
				weapon.second.NowBullet = weapon.second.bulletMax;
			}
		}
	}

}




float WeaponManager::GetReloadCount(WeaponID id) const
{
	auto itr = mmWeapons.find(id);
	if (itr == mmWeapons.end())
	{
		return 0.0f;
	}

	return itr->second.reloadCount;
}

// いま装備している武器のリロードカウンタ緒を返す
float WeaponManager::GetNowCou()
{
	auto itr = mmWeapons.find(mnNowWeapon);
	if (itr == mmWeapons.end())
	{
		return 0.0f;
	}

	return itr->second.reloadCount;
}



// 指定下武器のステータスを返してくれるやつ
float WeaponManager::Get_Sub_Bullet(WeaponID id) const
{
	auto itr = mmWeapons.find(id);
	if (itr == mmWeapons.end())
	{
		return 0.0f;
	}
	return itr->second.NowBullet;
}

float WeaponManager::Get_Sub_BulletMax(WeaponID id) const
{
	auto itr = mmWeapons.find(id);
	if (itr == mmWeapons.end())
	{
		return 0.0f;
	}
	return itr->second.bulletMax;
}

// いまついkあした
bool WeaponManager::GetReload(WeaponID id) const
{
	auto itr = mmWeapons.find(id);
	if (itr == mmWeapons.end())
	{
		return 0.0f;
	}

	return itr->second.reload;
}

// いま持っている武器がリロード中かどうかを取得
bool WeaponManager::GetNowRel()
{
	auto itr = mmWeapons.find(mnNowWeapon);
	if (itr == mmWeapons.end())
	{
		return 0.0f;
	}

	return itr->second.reload;
}




std::string WeaponManager::GetFrameName(WeaponID id, bool isEnemy) const
{
	//　敵っだったらWP
	if (isEnemy)
	{
		return "wp";
	}


	if (Master::msPlayerModelName == "Resource/3d/Player/Hero.mv1")
	{
		return "wp";
	}

	if (Master::msPlayerModelName == "Resource/3d/Zombie/Zombie_Mesh.mv1")
	{
		return "mixamorig5:RightHandThumb4";
	}

	if (Master::msPlayerModelName == "Resource/3d/Military/Guntai.mv1")
	{
		return "mixamorig:RightHandThumb4";
	}

	if (Master::msPlayerModelName == "Resource/3d/Bob/Bob.mv1")
	{
		return "mixamorig10:RightHandThumb4";
	}

	if (Master::msPlayerModelName == "Resource/3d/Devil/Devil.mv1")
	{
		return "mixamorig:RightHandThumb4";
	}

	// 一応例外の場合もやっとく
	return "";
}


// 武器のデータを一括管理する
// ここは、void とかの部分が WeaponManager::WeaponSpec これで、それのgetBaseSpacだから長くなっちゃった
WeaponManager::WeaponSpec WeaponManager::GetBaseSpec(WeaponID id)
{
	switch (id)
	{
	case Shield:
		// 回転設定 攻撃力と球数
		return { "盾", "Resource/Weapon/Shield/Shield.mv1", 280.0f, 50.0f,"盾で殴るスタイル"};

	case BlackGun:
		return { "黒い銃", "Resource/Weapon/Gun/BlackGun.mv1", 300.0f, 20.0f,"黒くてカッコイイ銃"};

	case SEEDGun:
		return { "銃剣", "Resource/Weapon/Gun/WhiteGun.mv1", 500.0f, 5.0f, "標準的な銃剣"};

	case BlueGun:
		return { "青い銃", "Resource/Weapon/Gun/BlueGun.mv1", 380.0f, 12.0f, "小さめの銃剣"};

	case YARI:
		return { "木の槍", "Resource/Weapon/Spear/Spear.mv1", 400.0f, 15.0f, "縄文時代の槍"};

	case GT:
		return { "ギター", "Resource/Weapon/GT/Guitar.mv1", 360.0f, 30.0f, "赤いギター  殴るスタイル\n弾が音符になって音が鳴る"};

	default:
		return { "盾", "Resource/Weapon/Shield/Shield.mv1", 280.0f, 50.0f,"盾で殴るスタイル" };
	}
}


// 選択された武器をロードする
void WeaponManager::SelectLoadWeapon(WeaponID id, bool isEnemy)
{
	if (id == WeaponID::None)
	{
		return;
	}

	if (mmWeapons.find(id) != mmWeapons.end())
	{
		return; // すでにロード済み
	}

	// GetBaseSpec からデータを取得 ここをやるから盾とか設定するのは一回で良い
	WeaponSpec spec = GetBaseSpec(id);

	std::string attachFrame = GetFrameName(id, isEnemy);
	if (attachFrame.empty())
	{
		return;
	}

	// 取得した数値とパスを使ってロード
	Lord_Base_Weapon(
		id,
		spec.modelPath,
		attachFrame,
		spec.attack,
		spec.bulletMax,
		isEnemy
	);
}


// 武器の回転
VECTOR WeaponManager::RotateWeapon(WeaponID id, bool isEnemy) const
{
	// 基本は回転なしにしとく
	VECTOR rotate = VGet(0, 0, 0);

	//　敵っだったらWP
	if (isEnemy)
	{
		return rotate;
	}

	if (Master::msPlayerModelName == "Resource/3d/Zombie/Zombie_Mesh.mv1")
	{
		if (id == Shield || id == GT)
		{
			rotate = VGet(0, 80, 90); // 盾なら180
		}
		else
		{
			rotate = VGet(0.0f, 0.0f, (float)DX_PI + 1.0f); // 銃なら90とかもろもろ
		}
	}

	if (Master::msPlayerModelName == "Resource/3d/Military/Guntai.mv1")
	{
		if (id == Shield || id == GT)
		{
			rotate = VGet(0, 80, 90); // 盾なら180
		}
		else
		{
			rotate = VGet(0.0f, 0.0f, (float)DX_PI + 1.0f); // 銃なら90とかもろもろ
		}
	}

	if (Master::msPlayerModelName == "Resource/3d/Bob/Bob.mv1")
	{
		if (id == Shield)
		{
			rotate = VGet(0, 80, 90); // 盾なら180
		}
		else if (id == GT)
		{
			rotate = VGet(90, 180, 180); //ギターなら
		}
		else
		{
			rotate = VGet(180.0f, 30.0f, (float)DX_PI + 1.0f); // 銃なら90とかもろもろ
		}
	}

	if (Master::msPlayerModelName == "Resource/3d/Devil/Devil.mv1")
	{
		if (id == Shield || id == GT)
		{
			rotate = VGet(0, 80, 90); // 盾なら180
		}
		else
		{
			rotate = VGet(0.0f, 0.0f, (float)DX_PI + 1.0f); // 銃なら90とかもろもろ
		}
	}


	return rotate;
}