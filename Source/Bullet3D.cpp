#include "Bullet3D.h"
#include "Model.h"
#include "Master.h"
#include "SceneManager.h"
#include "ObjectManager.h"
#include "Scene.h"
#include "Enemy3D.h"
#include "Effect.h"
#include "Wall.h"
#include "Stage.h"
#include "BulletMark.h"
#include "Player3D.h"
#include "Effect4.h"
#include "BulletEffect.h"
#include "DeathEffect.h"
#include "Effect2.h"

Bullet3D::Bullet3D(Object3D::Tag3D shotManTag, Unit* owner, Object3D* target, VECTOR initPos,
	std::string filename, VECTOR Direction, float attack, std::string effect)
	: Object3D(initPos)
	,mtShotMon(shotManTag) // 誰が撃ったかの代入
	,mpOwner(owner) // 誰が撃ったかの代入
	, mvDirection(VNorm(Direction)) // 正規化もする
	, mpTarget(target) // ターゲット代入
{
	SetTag(Object3D::T_Bullet3D);

	mpModel = new Model(filename, initPos);

	mbIsHoming = false; /// ホーミングしない
	if (mtShotMon == Object3D::T_Player3D)   //  撃ったのがプレイヤー
	{
		// ロックオンしていたら
		if (mpTarget && Master::mpCamera->IsRockOn())
		{
			float distance = VSize(VSub(mpTarget->GetPosition(), mvPosition));
			if (distance <= 2000.0f) //2000以内ならホーミング
			{
				mbIsHoming = true;
			}
		}
	}
	if (mtShotMon == Object3D::T_Enemy3D)
	{
		if (mpTarget)
		{
			float distance = VSize(VSub(mpTarget->GetPosition(), mvPosition));
			if (distance <= 1800.0f)
			{
				mbIsHoming = true;
			}
		}
	}

	msEffectFile = effect;

	mfAttack = attack; // 攻撃力を代入
	// 初期の弾の向き
	float angleY = atan2(mvDirection.x, mvDirection.z); // XZ平面上で弾がどっちを向いているか
	mpModel->SetRotation(VGet(0.0f, angleY + (float)DX_PI / 2.0f, 0.0f));
}

Bullet3D::~Bullet3D()
{
	delete mpModel;
}

void Bullet3D::Update()
{
	Move(); // 移動
	HitWall(); // 壁とのあたり判定
	CalcDirection(); // 追跡するやつ
	


	// 撃った人によってあたり判定を別々で呼び出し
	if (mtShotMon == Object3D::T_Player3D)
	{
		HitEnemy(); // 敵へのあたり判定
	}
	else if (mtShotMon == Object3D::T_Enemy3D)
	{
		HitPlayer(); // プレイヤーへのあたり判定
	}

	mpModel->Update();
}

void Bullet3D::Draw()
{
	//// 弾を包み込むような球体
	//DrawSphere3D(
	//	mvPosition,
	//	20.0f,
	//	8,
	//	GetColor(255, 0, 0),
	//	GetColor(255, 0, 0),
	//	false
	//);

	mpModel->Draw();
}

void Bullet3D::Move()
{
	mvPosition = VAdd(mvPosition, VScale(mvDirection, mfSpeed));

	// 移動距離を計算
	mfMoveSpeed += mfSpeed;
	if (mfMoveSpeed >= MOVE_DISTANCE)
	{
		SetDeleteFlag(true);
	}

	float moved = mfSpeed;
	mEffectDistance += moved;
	if (mEffectDistance >= 15.0f)
	{   
		// 重いから調整
		new BulletEffect(mvPosition, msEffectFile);
		mEffectDistance = 0.0f;
	}

	mpModel->SetPosition(mvPosition); // 座標を動かす

}

// 敵とのあたり判定 playerが使う
void Bullet3D::HitEnemy()
{
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Enemy3D);

	for (auto enemy : pEnemyList) // &はいらなさそ
	{
		Enemy3D* pEnemy = dynamic_cast<Enemy3D*>(enemy);
		if (!pEnemy) continue; // 見つからなかっったら抜ける

		bool isHit = HitCheck_Sphere_Capsule(
			mvPosition,
			20.0f,
			pEnemy->GetPosition(),
			VAdd(pEnemy->GetPosition(), VGet(0, 150, 0)),
			40.0f
		);


		if (isHit && !pEnemy->IsAttackFlag())
		{
			pEnemy->Damage(mfAttack, mpOwner);

			// 弾は当たったら消す
			SetDeleteFlag(true);
		}
	}
}

// プレイヤーとのあたり判定 敵が使う
void Bullet3D::HitPlayer()
{
	auto pPlayerList = Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(Object3D::T_Player3D);
	// 敵のクールタイムを設定するから敵の情報もとってくる
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Enemy3D);

	for (auto obj : pPlayerList)
	{
		Player3D* pPlayer = dynamic_cast<Player3D*>(obj);
		if (!pPlayer) continue;

		bool isHit = HitCheck_Sphere_Capsule(
			mvPosition,
			20.0f,
			pPlayer->GetPosition(),
			VAdd(pPlayer->GetPosition(), VGet(0, 150, 0)),
			40.0f
		);

		// ここでバリアとのあたり判定も入れたい
		if (isHit)
		{
			if (!pPlayer->IsAttackFlag())
			{
				if (!pPlayer->PlayerBariaAndCheck())
				{
					if (pPlayer->GetTime() <= 0.0f)
					{
						pPlayer->Damage(mfAttack, mpOwner);
					}
					pPlayer->SetAlpha(); // 最大値をセット
				}
				else
				{
					HitBaria(); // バリアを呼びだすやつを呼び出し
					//DrawFormatString(300, 500, GetColor(255, 0, 0), "バリア発動");
				}
			}

			SetDeleteFlag(true); // 弾消す
		}
	}
}


// バリアと当たったら消す処理
void Bullet3D::HitBaria()
{
	auto pBariaList = Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(Object3D::T_Baria3D);

	for (auto obj : pBariaList)
	{
		Baria* pBaria = dynamic_cast<Baria*>(obj);
		if (!pBaria) continue;

		if (pBaria->IsActive())
		{

			bool isHit = HitCheck_Sphere_Sphere(
				mvPosition,
				20.0f,
				pBaria->GetPosition(),
				100.0f
			);

			//// バリアを包み込むような球体
			//DrawSphere3D(
			//	pBaria->GetPosition(),
			//	110.0f,
			//	8,
			//	GetColor(255, 255, 0),
			//	GetColor(255, 255, 0),
			//	false
			//);

			if (isHit) // 一応ここが呼ばれている
			{
				SetDeleteFlag(true); // たまに当たったら消す処理

				// SE再生
				Master::mpSoundManager->PlaySE(SoundManager::SE_BARIA_HIT, 220); // バリアと当たった時
				new Effect4(mvPosition, "Resource/2d/BariaMark2.png");
			}
		}

	}
}


void Bullet3D::HitWall()
{
	// 壁画像とのあたり判定
	auto pWallList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Wall3D);

	for (auto obj : pWallList)
	{
		Wall* wall = dynamic_cast<Wall*>(obj);
		if (!wall) continue;
		std::vector<VERTEX3D> vertex = wall->GetVertex(); // 情報の取得

		if (HitCheck_Capsule_Triangle(
			mvPosition,
			mvPosition,
			20.0f,
			vertex.at(0).pos, vertex.at(1).pos, vertex.at(2).pos) ||

			HitCheck_Capsule_Triangle(
			mvPosition,
			mvPosition,
			20.0f,
			vertex.at(3).pos, vertex.at(1).pos, vertex.at(2).pos)
			)

		{
			// 三角形の頂点
			VECTOR ver0 = vertex.at(0).pos;
			VECTOR ver1 = vertex.at(1).pos;
			VECTOR ver2 = vertex.at(2).pos;
			// エッジ
			VECTOR eg1 = VSub(ver1, ver0);
			VECTOR eg2 = VSub(ver2, ver0);
			// 外積 から 法線 // 正規化もしている
			VECTOR makeNormal = VNorm(VCross(eg1, eg2));

			// SE再生
			Master::mpSoundManager->PlaySE(SoundManager::SE_WALL_HIT, 120);

			new BulletMark("Resource/2d/BulletMark.png", mvPosition, makeNormal);
			new Effect2(mvPosition, "Resource/2d/Weapon.png"); // エフェクト出すようにした

			SetDeleteFlag(true);
			return;
		}
	}


	// 壁モデルとのあたり判定  追加 ステージにしたから変更
	auto pWallModels = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Stage3D);
	for (auto wallmode : pWallModels)
	{
		Stage* Wall = dynamic_cast<Stage*>(wallmode);
		if (!Wall) continue;
		VECTOR hitPos, hitNormal; // 今回は使わないかも

		if (Wall->CheckHit_Capsule_Wall(mvPosition,
			VAdd(mvPosition, VGet(0, 0, 0)), 20.0f, hitPos, hitNormal))
		{
			hitNormal = VNorm(hitNormal); // 正規化
			
			// SE再生
			Master::mpSoundManager->PlaySE(SoundManager::SE_WALL_HIT, 120);

			// 銃痕作成
			new BulletMark("Resource/2d/BulletMark.png", hitPos, hitNormal);

			SetDeleteFlag(true);
			return;
		}
	}
}


//void Bullet3D::CalcDirection()
//{
//	// 追加 0113 ロックオン解除後にも追跡するバグを修正
//	if (!Master::mpCamera->IsRockOn()) // ロックオンしていなかったらのif
//	{
//		// 一応ここもnullにしておく
//		mbIsHoming = false; // 弾が追跡するかのフラグをOFF
//		mpTargetEnemy = nullptr; // 弾が追いかけるターゲットをnullptr
//		return;
//	}
//
//	if (!mbIsHoming || !mpTargetEnemy)
//	{
//		return; // ターゲットがいなかったらホーミングさせないように
//	}
//	VECTOR toEnemy = VSub(mpTargetEnemy->GetPosition(), mvPosition); // 弾から敵へのベクトル
//	mvDirection = VNorm(toEnemy); // 正規化
//
//	// 弾の向きを合わせる
//	float angleY = atan2f(mvDirection.x, mvDirection.z);
//	mpModel->SetRotation(VGet(0.0f, angleY + DX_PI_F / 2.0f, 0.0f)); // Y軸回転
//}


// カメラがロックオンかは敵とかプレイヤーで見る
void Bullet3D::CalcDirection()
{
	// ホーミングじゃない　かつターゲットがnullだったら抜ける
	if (!mbIsHoming || !mpTarget)
	{
		return;
	}

	VECTOR targetPos = mpTarget->GetPosition();
	targetPos.y += 100.0f; // 胴体らへんを狙うように
	VECTOR targetAngle = VSub(targetPos, mvPosition);
	mvDirection = VNorm(targetAngle);

	// 弾の向きを合わせる
	float angleY = atan2f(mvDirection.x, mvDirection.z);
	mpModel->SetRotation(
	VGet(0.0f, angleY + DX_PI_F / 2.0f, 0.0f)
	);
}