#include "Enemy3D.h"
#include "Model.h"
#include "Master.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Wall.h"
#include "Stage.h"
#include "InputManager.h"
#include "Player3D.h"
#include "Effect.h"
#include "Effect2.h"
#include "Effect3.h"
#include "Utility.h"
#include "Floor.h"
#include "Bullet3D.h"
#include "Effect4.h"
#include "GameManager.h"


Enemy3D::Enemy3D(int type, std::string filename, VECTOR initPos,
	float hp, float boost, float speed, float maxSpeed)
	: Unit(initPos, hp, boost, speed, maxSpeed)
	, HitFlag(false)
	, mbIsWallWeapon(false)
{
	SetTag(Object3D::T_Enemy3D); // タグ

	mpModel = new Model(filename, initPos);


	mpHpBar = new HpBar(this); // mpHpBarにthisを入れる
	mpBoostBar = new BoostBar(this);

	// モデルを作った直後にWeaponManagerを作る
	// 気に入ってる黒い銃を持ってもらう
	mpWeapon = new WeaponManager(mpModel->GetHandle());


	SetFontSize(20);

	// 初めにもcoolタイム入れる
	mfFlyMaxCount = 3000.0f + GetRand(2500); // 2000から2500
	// 個体差
	// 決めた数値に　＋1から（なんか）までをrandomに足す
	mRunDist += GetRand(200);
	mShotDist += GetRand(300);
	mJumpDist += GetRand(500);

	// 速さを代入
	mfSpeed = speed;
	mfMaxSpeed = maxSpeed;
	mnType = type; // 代入

	// 改良 0205
	// 武器選択
	auto weapon = GameManager::GetInstance()->EnemyWeapon;
	// 敵タイプ 武器も変えた
	switch (mnType)
	{
	case 1:
		mType = EnemyType::Attacker;
		mfSpeed *= 1.5f;
		weapon.enemyWeapon = (WeaponManager::WeaponID::SEEDGun);
		break;

	case 2:
		mType = EnemyType::Shooter;
		mShotDist += 600;
		weapon.enemyWeapon = (WeaponManager::WeaponID::BlackGun);
		break;

	case 3:
		mType = EnemyType::Coward;
		weapon.enemyWeapon = (WeaponManager::WeaponID::BlueGun);
		break;

	default:
		mType = EnemyType::Attacker;
		weapon.enemyWeapon = (WeaponManager::WeaponID::BlackGun);
		break;
	}
	// ここでロードしないとセットされない
	mpWeapon->SelectLoadWeapon(weapon.enemyWeapon, true);
	// なしじゃないなら選択したやつをセット
	if (weapon.enemyWeapon != WeaponManager::WeaponID::None)
	{
		mpWeapon->Set_Base_Weapon(weapon.enemyWeapon);
	}
	else // ない場合
	{
		//盾をロードして装備させる
		mpWeapon->SelectLoadWeapon(WeaponManager::WeaponID::Shield, true);
		mpWeapon->Set_Base_Weapon(WeaponManager::WeaponID::Shield);
	}



	// 軌跡の初期化
	int TrailColor = GetColor(255, 255, 0);
	g_Trail.Initialize(0.35f, TrailColor); // 寿命と色
	// フレームの位置に軌跡を出す
	VECTOR BottomPos = mpWeapon->GetFrameWorldPosition("剣下");
	VECTOR TopPos = mpWeapon->GetFrameWorldPosition("銃口1");
	g_Trail.Reset(BottomPos, TopPos);
}

Enemy3D::~Enemy3D()
{
	delete mpWeapon; // 追加
	if (mpModel != nullptr)
	{
		delete mpModel;
	}

	delete mpHpBar;// 仮に二重になったらこっちを消す(今はプレイヤーにもポインタあを持たせてやってる)
	delete mpBoostBar;
}


void Enemy3D::Update()
{
	// player情報の取得
	auto pPlayerList =
		Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(Object3D::T_Player3D);
	// プレイヤーがいなかったら無敵にする
	if (pPlayerList.empty())
	{
		SetTime(1000);
	}


	mvOldPosition = mvPosition;

	if (mpModel != nullptr)
	{
		mpModel->Update();
	}


	// デバックのフラグをプレイや－から取る
	DebugMode();

	// 今のシーンがチュートリアルじゃないなら処理をする
	if (Master::mpSceneManager->GetCurrentSceneType() != SceneManager::SCENE_TYPE::TUTORIAL_SCENE_3D)
	{
		// 攻撃クールタイムのカウント
		if (mfNeetTime > 0.0f)
		{
			mfNeetTime -= 1.0f;
		}

		// 移動処理
		Move();

		// 回転処理の呼び出し
		RotationByMove();

		// 当たり判定系の呼び出し
		FloorHit3D();
		WallHit3D();

		// 敵同士のあたり判定
		EnemytoEnemyHit();

		// 壁２D判定とか２Dのやつ
		Hit2D();


		if (mbJumpStart)
		{
			Jump();
		}
		else if ((mbShot && mpWeapon->GetNowCou() <= 0.0f))
		{
			BulletAttack();
		}
		else if (mbAttackDistance && Get_ENeetTime() <= 0) // 150いないならの時にtrueにしたフタグ
		{
			Attack();
		}

	}



	if (mfSpeed >= mfMaxSpeed)
	{
		mfSpeed = mfMaxSpeed;
	}

	// 上昇してよいかのカウント
	if (!mbOkFly)
	{
		mfFlyCount++;
		if (mfFlyCount >= mfFlyMaxCount)
		{
			mbOkFly = true;
			FlyReset();  // ランダムの時間を設定
		}
	}

	// 自分の武器マネージャーに、今装備している武器のリロード状態を聞く
	mbReload = mpWeapon->GetNowRel();
	if (mbReload)
	{
		mpWeapon->AddCount();
		if (mpWeapon->Get_Base_Bullet() >= mpWeapon->Get_Base_BulletMax())
		{
			mbReload = false;
		}
	}



	// 点滅のタイマー
	mCurrentPhoto += 10.0f;
	if (mCurrentPhoto > 50.0f)
	{
		mFlushTimer = !mFlushTimer;
		mCurrentPhoto = 0;
	}



	// 無敵時間中の点滅処理
	// 無敵かつまだゲームが終わってないなら
	if (GetTime() > 0.0f && GetTime() < 500.0f)
	{
		// 5フレームに一度 描画をスキップ
		if ((int)(GetTime() / 5) % 2 == 0)
		{
			isVisible = false;
		}
		else
		{
			isVisible = true;
		}
	}
	else if (GetTime() == 0.0f)
	{
		isVisible = true;
	}



	// フレームの位置に軌跡を出す
	// なくても武器の位置から出るからおｋ
	VECTOR BottomPos = mpWeapon->GetFrameWorldPosition("剣下");
	VECTOR TopPos = mpWeapon->GetFrameWorldPosition("銃口1");
	// 軌跡の更新
	int TrailColor = GetColor(255, 255, 0);
	g_Trail.Update(BottomPos, TopPos);




	mpModel->SetPosition(mvPosition); // モデルに今の座標をセットする
	mpHpBar->Update();
	mpBoostBar->Update();

	Unit::Update(); // これを呼ばないと、減らぬ
}

void Enemy3D::DrawEnemyUI()
{
	mpHpBar->EnemyHpBar(); // HP－の描画
	mpBoostBar->EnemyBoostBar(); // BOOSTの描画
}

// 軌跡描画
void Enemy3D::DrawEnemyTrail3D()
{
	// アニメの時間とる
	float now = mpModel->GetAnimTime();
	float total = mpModel->GetTotalTiime();

	float startTime = total * (1.0f / 5.0f); // 全体の２０％
	float endTime = total * (3.5f / 5.0f); // 全体の７０％

	// 攻撃の特定の時間帯だけ描画する
	if (GetNearAttack() && now >= startTime && now <= endTime)
	{
		g_Trail.Draw();
	}
}


void Enemy3D::DrawDebug()
{
	if (mbdebugMode)
	{
		// 敵情報の取得（複数当たっても良いように）
		auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
			->GetObject3DListByTag(Object3D::T_Enemy3D);

		// ここで初期化と設定をしている
		int mnAllEnemy = 0;
		// 敵の数を数えている
		for (int i = 0; i < pEnemyList.size(); i++)
		{
			auto pEnemy = pEnemyList[i];
			// 敵が存在していたら
			if (pEnemy != nullptr)
			{
				// 敵の数を++して生きている敵を数える
				mnAllEnemy++;
			}
		}


		// デバックように敵の残数を書いている
		DrawFormatString(20, 80, GetColor(255, 255, 255), "敵の残数: %d", mnAllEnemy);

		if (!pEnemyList.empty())
		{
			// 敵のサイズを数えている
			for (int i = 0; i < pEnemyList.size(); i++)
			{
				auto pEnemy = pEnemyList[i];
				if (pEnemy != nullptr)
				{
					if (GetHp() >= 0)
					{
						// dynamic_cast... 動的変換
						Enemy3D* pTarget = dynamic_cast<Enemy3D*>(pEnemy);

						// デバック用HP
						// HPと敵のをかけると順番にでる
						DrawFormatString(10, 120 + 20 * i, GetColor(255, 255, 255),
							"敵%d のHP: %f,"
							"BOOST: %f,"
							"無敵: %f,"
							"CT: %d,"
							"Dis: %.1f,"
							"弾: %f,"
							"Rel: %.1f",
							i,
							pTarget->GetHp(),
							pTarget->GetBoost(),
							pTarget->GetTime(),
							(int)pTarget->Get_ENeetTime(),
							pTarget->GetDistance(),
							pTarget->mpWeapon->Get_Base_Bullet(),
							pTarget->mpWeapon->GetNowCou()
						);


						int x = 200;
						int y = 500 + 60 * i; //　複数いるかもだからずらす
						int color = GetColor(255, 0, 0);
						// 上昇カウントの表示 と 個体差の表示
						// DrawFormatString(x, 400 + 20 * i, GetColor(255, 255, 0), "Max: %.1f  Now: %.1f", pTarget->GetFlyMaxCount(), pTarget->GetFlyCount());
						// DrawFormatString(x + 300, 440 + 20 * i, GetColor(255, 255, 0), "スピード%.1f  Max: %.1f", pTarget->GetSpeed(), pTarget->GetMaxSpeed());

						//// タイプによって変えてる
						//switch (pTarget->mType)
						//{
						//case EnemyType::Attacker:
						//	DrawFormatString(x, y, color, "攻撃特化");
						//	break;
						//case EnemyType::Shooter:
						//	DrawFormatString(x, y, color, "銃撃型");
						//	break;
						//case EnemyType::Coward:
						//	DrawFormatString(x, y, color, "逃げるやつ");
						//	break;
						//}


					}
				}
			}
		}



		// デバック表示用 基本のカプセル　床用
		DrawCapsule3D(mvPosition, VAdd(mvPosition, VGet(0.0f, 80.0f, 0.0f)),
			50.0f,
			8,
			GetColor(0, 255, 0),
			GetColor(0, 255, 0), false);

		// 壁
		DrawCapsule3D(VAdd(mvPosition, VGet(0.0f, 100.0f, 0.0f)),
			VAdd(mvPosition, VGet(0.0f, 160.0f, 0.0f)),
			40.0f,
			8,
			GetColor(0, 0, 255),
			GetColor(0, 0, 255), false);


		// 天井
		DrawCapsule3D(VAdd(mvPosition, VGet(0.0f, 220.0f, 0.0f)),
			VAdd(mvPosition, VGet(0.0f, 250.0f, 0.0f)),
			20.0f,
			8,
			GetColor(255, 0, 0),
			GetColor(255, 0, 0), false);


		// デバッグ可視化  緑のRay
		DrawLine3D(VAdd(mvPosition, VGet(0.0f, 60.0f, 0.0f)),
			VAdd(mvPosition, VGet(0.0f, -2000.0f, 0.0f)),
			GetColor(0, 255, 255));


		// ながい武器でも当たるようにした
		// なくても武器の位置から出るからおｋ
		VECTOR muzzlePos = mpWeapon->GetFrameWorldPosition("銃口1");
		// ここで壁と武器の当たり判定のデバック
		VECTOR muzzlePos1 = mpWeapon->Get_Base_WeaponPosition();
		// 壁に当たっているかどうかで色を変える  黄だと攻撃OK　赤なら壁当たり
		unsigned int debugColor = mbIsWallWeapon ? GetColor(255, 0, 0) : GetColor(255, 255, 0);
		DrawSphere3D(muzzlePos1, 55.0f, 8, debugColor, GetColor(255, 255, 255), FALSE);
		DrawSphere3D(muzzlePos, 50.0f, 8, debugColor, GetColor(255, 255, 255), FALSE);

		// 武器を包み込むような球体
		DrawSphere3D(
			muzzlePos,
			30.0f,
			8,
			GetColor(255, 255, 255),
			GetColor(255, 255, 255),
			false
		);
	}

}


void Enemy3D::Draw()
{
	DrawDebug(); // デバックもじとか // 点滅のときのフラぎ追加した
	if (mpModel != nullptr && isVisible)
	{
		mpModel->Draw();
	}
}

void Enemy3D::FloorHit3D()
{
	// -----------------床の3Dモデルとのあたり判定 床★  ------追加 11/7----------------------------///////
	VECTOR hitPosDown = VGet(0.0f, 0.0f, 0.0f); // 下方向用
	VECTOR hitPosUp = VGet(0.0f, 0.0f, 0.0f);   // 上方向用  10/29 (元はhitPosだけ)

	// 複数に 0121
	auto obj = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Stage3D);
	for (auto stage : obj)
	{
		Stage* pStage = dynamic_cast<Stage*>(stage);
		if (pStage != nullptr)
		{
			// ステージとプレイヤーのカプセルが当たっている場合
			if (pStage->CheckHit_Capsule(mvPosition, VAdd(mvPosition, VGet(0.0f, 80.0f, 0.0f)), 50.0f))
			{
				float LinePos = 33.0f; // プレイヤーの中心から離れている距離を設定

				// 五本の線の追加
				VECTOR LineSet[5]
				{
					VGet(0.0f, 0.0f, 0.0f), // 中央
					VGet(LinePos, 0.0f, 0.0f), // 右側
					VGet(-LinePos, 0.0f, 0.0f), // 左側
					VGet(0.0f, 0.0f, LinePos), // 前
					VGet(0.0f, 0.0f, -LinePos)  // 後
				};

				float MaxY = -FLT_MAX; // いちばん小さい数字入れている
				bool isHit = false; // 一本でも当たったかのフラグをON

				for (int i = 0; i < 5; i++)
				{
					VECTOR start = VAdd(mvPosition, VAdd(LineSet[i], VGet(0.0f, 60.0f, 0.0f)));
					VECTOR end = VAdd(mvPosition, VAdd(LineSet[i], VGet(0.0f, -40.0f, 0.0f)));

					VECTOR hitPos = pStage->CheckHit_Line(start, end);

					// デバック用
					//DrawLine3D(start, end, GetColor(0.0f, 255.0f, 100.0f));
					if (hitPos.x != 0.0f || hitPos.y != 0.0f || hitPos.z != 0.0f)
					{
						isHit = true;
						if (hitPos.y > MaxY)
						{
							MaxY = hitPos.y;
						}
					}
				}

				// 一本でも当たっていたらＴＲＵＥになる
				if (isHit)
				{
					mfGroundY = MaxY;
					if (!mbJump) // ジャンプ中じゃなったら合わせる // スタートだとそのときしかみないから
					{
						mvPosition.y = mfGroundY;
					}
				}

				mbHitUp = false; // 天井に当たっていないフラグをＯＦＦ
			}
			// ステージとプレイヤーのカプセルが当たっている場合 200から250らへんが当たっていたら
			if (pStage->CheckHit_Capsule(
				VAdd(mvPosition, VGet(0.0f, 220.0f, 0.0f)),
				VAdd(mvPosition, VGet(0.0f, 250.0f, 0.0f)), 20.0f))
			{
				// 上方向の当たり（天井用）プレイヤーが下にいる
				VECTOR hitPosUp = pStage->CheckHit_Line(
					VAdd(mvPosition, VGet(0.0f, 220.0f, 0.0f)),
					VAdd(mvPosition, VGet(0.0f, 250.0f, 0.0f))
				);

				mbHitUp = true;
				if (hitPosUp.y != 0.0f)
				{
					// mvPositionの基準は足元だから、ちょっと下げる
					float hitUp = hitPosUp.y - 250.0f;
					mvPosition.y = hitUp;

					mfJumpPower = -0.1f; // 少し下げて天井に当たらないようにしている
				}
			}
			// モデルに当たっていない場合
			if ((!pStage->CheckHit_Capsule(
				VAdd(mvPosition, VGet(0.0f, 220.0f, 0.0f)),
				VAdd(mvPosition, VGet(0.0f, 250.0f, 0.0f)), 40.0f)) &&
				(!pStage->CheckHit_Capsule(mvPosition, VAdd(mvPosition, VGet(0.0f, 80.0f, 0.0f)), 50.0f)))
			{
				mbHitUp = false;

				// プレイヤーから下にＬＩＮＥやる
				VECTOR start = VAdd(mvPosition, VGet(0.0f, 60.0f, 0.0f));
				VECTOR end = VAdd(mvPosition, VGet(0.0f, -2000.0f, 0.0f));

				VECTOR hitPos = pStage->CheckHit_Line(start, end);
				// デバック
				//DrawLine3D(start, end, GetColor(0.0f, 255.0f, 100.0f));
				// 床が見つかったら更新
				mfGroundY = hitPos.y;

				// 床が段差じゃなかったら落下状態に入る
				const float GroundHight = 100;
				if (!mbJump) //ジャンプ中じゃなかったら
				{
					if (mvPosition.y - mfGroundY > GroundHight)
					{
						mbJumpStart = true; // ここをmbJumpから変えた★hp氏★★★　ここまじで大事過ぎる
						mbFall = true;
					}
				}
			}
		}
	}
}


void Enemy3D::WallHit3D()
{
	// 改良後の当たり判定　壁の３Dモデル 11/4
	// 本来の移動ベクトルから移動しようとしている所を引いている
	VECTOR moveVecWall = VSub(mvPosition, mvOldPosition);
	VECTOR newPos = mvPosition; // 仮の移動先を書いている
	auto wall3DList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Stage3D);

	std::vector<VECTOR> hitNormals; // ここにXZだけ保存したいので入れる

	// どの壁に当たっているか
	for (auto obj : wall3DList) // wall3DList の中のオブジェクトをひとつずつチェックする
	{
		Stage* wall = dynamic_cast<Stage*>(obj); // ダイナミックキャスト

		VECTOR hitPos, hitNormal; // 衝突した結果を受け取るための変数

		// 壁に当たっているかの判定
		if (wall->CheckHit_Capsule_Wall(VAdd(newPos, VGet(0, 100, 0)),
			VAdd(newPos, VGet(0, 160, 0)), 40.0f, hitPos, hitNormal))
		{
			// 当たっていたらXZだけ保存
			VECTOR mvXZ = VGet(hitNormal.x, 0.0f, hitNormal.z); // XとZの要素だけ取り出す
			mvXZ = VNorm(mvXZ); //正規化
			hitNormals.push_back(mvXZ); // 複数に当たっている可能性があるので末尾に追加していく

			// コンテナの外側もスライドするようにする
			VECTOR imvXZ = VScale(mvXZ, -1.0f);
			hitNormals.push_back(imvXZ);
		}
	}
	// 移動前に壁に当たっていた場合だけここに来る
	// 壁に当たっていたらスライド計算
	if (!hitNormals.empty())
	{
		// hitNormals に複数法線があるとき
		VECTOR slideVec = moveVecWall;
		for (auto osu : hitNormals) // 当たった壁ごとにループ
		{
			// めり込み方向の成分を取り除く
			float meri = VDot(slideVec, osu); // VDot　内積
			if (meri < 0.0f) // めり込んでいる方向のみ処理  壁に向かっている場合のみ
			{
				slideVec = VSub(slideVec, VScale(osu, meri)); // めりこみ成分を削る
			}
		}
		VECTOR KPos = VAdd(mvOldPosition, slideVec); // スライド後の仮の位置

		// スライド後も衝突するか
		bool slideHit = false; //初期化
		for (auto obj : wall3DList)// 配列やリストの中身を一個ずつ取り出して使うという繰り返し処理
		{
			Stage* wall = dynamic_cast<Stage*>(obj); // ダイナミックキャスト

			VECTOR hitPos, hitNormal; // 結果を受け取るための変数
			if (wall->CheckHit_Capsule_Wall(
				VAdd(KPos, VGet(0, 100, 0)),
				VAdd(KPos, VGet(0, 160, 0)), 40.0f, hitPos, hitNormal))
			{
				slideHit = true;
			}
		}

		// 衝突しなければ移動、していたら前座標で停止する
		if (!slideHit)
		{
			mvPosition.x = KPos.x;
			mvPosition.z = KPos.z;
			// 追加 0114 
			// フラグを逆にして、進む方向も逆にしている ★
			if (HitFlag == false)
			{
				HitFlag = true;
				if (rand() % 2 == 0)
				{
					mnMoveCount = 1;
				}
				else
				{
					mnMoveCount = 2;
				}
			}
			else
			{
				HitFlag = false;

				if (rand() % 2 == 0)
				{
					mnMoveCount = 1;
				}
				else
				{
					mnMoveCount = 2;
				}
			}
		}
		else // ここが原因で角で止まる
		{
			VECTOR move = moveVecWall;
			VECTOR oldPos = mvOldPosition;
			float radius = 40.0f; // 半径

			// X方向
			VECTOR nextX = VAdd(oldPos, VGet(move.x, 0, 0));
			bool hitX = false;	// Xは通れるかのフラグ
			VECTOR normX = VGet(0, 0, 0); // X衝突時の法線を保存
			for (auto obj : wall3DList)
			{
				Stage* wall = dynamic_cast<Stage*>(obj);
				VECTOR hitPos, hitNormal;
				if (wall->CheckHit_Capsule_Wall(VAdd(nextX, VGet(0, 90, 0)),
					VAdd(nextX, VGet(0, 160, 0)), radius, hitPos, hitNormal))
				{
					hitX = true;	 // ああ立ったTRUE
					normX = hitNormal; // 法線を代入
					break;
				}
			}

			if (!hitX)
			{
				mvPosition.x = nextX.x;
			}
			else
			{
				// 壁の法線方向に押し出す
				mvPosition.x = oldPos.x + (normX.x * 0.15f);
			}

			// --- Z方向 ---
			VECTOR basePos = mvPosition; // Xの押し戻しが反映された座標をベースにする
			VECTOR nextZ = VAdd(basePos, VGet(0, 0, move.z));
			bool hitZ = false;
			VECTOR normZ = VGet(0, 0, 0);
			for (auto obj : wall3DList)
			{
				Stage* wall = dynamic_cast<Stage*>(obj);
				VECTOR hp, hn;
				if (wall->CheckHit_Capsule_Wall(VAdd(nextZ, VGet(0, 90, 0)), VAdd(nextZ, VGet(0, 160, 0)), radius, hp, hn))
				{
					hitZ = true;
					normZ = hn; // 法線を拾う
					break;
				}
			}
			if (!hitZ)
			{
				mvPosition.z = nextZ.z;
			}
			else	 // こっちも押し出し
			{
				mvPosition.z = oldPos.z + (normZ.z * 0.15f);
			}
		}
	}
}

void Enemy3D::Move()
{
	AnimationState state = mpModel->GetNowState();
	if (state == ANIMATION_ATTACK
		|| state == ANIMATION_JUMP_OUT)
	{
		return; // 特定のモーション中なら何もしない
	}

	// 移動方向をきめる -> 速度をかける
	//VECTOR moveVec = VGet(0.0f, 0.0f, 0.0f); // 移動方向
	mvMoveVec = VGet(0.0f, 0.0f, 0.0f);
	VECTOR upMoveVector = VGet(0.0f, 0.0f, 0.0f); // カメラの上方向（奥方向）ベクトル
	VECTOR leftMoveVector = VGet(0.0f, 0.0f, 0.0f); // カメラの左方向ベクトル

	// カメラの向きから移動ベクトルを求める
	{
		// 上方向への移動ベクトルは、カメラの視点方向からY成分を抜いたもの
		// 注視点からカメラ自身の座標を引いているだけ
		upMoveVector = VSub(Master::mpCamera->GetLookAtPosition(), Master::mpCamera->GetPosition());
		upMoveVector.y = 0.0f;

		// 左方向への移動ベクトルは、上方向への移動ベクトルと、ｙ軸のプラス方向へのベクトルに垂直な方向 (外積)
		// VCross: ベクトル同士の外積を計算してくれるもの ★★★
		leftMoveVector = VCross(upMoveVector, VGet(0.0f, 1.0f, 0.0f));
		leftMoveVector.y = 0.0f;

		// 移動ベクトルは移動量を加味しないので、正規化しておく（ベクトルの長さを１にすること）
		// VNorm: 正規化してくれる便利なもの
		upMoveVector = VNorm(upMoveVector);
		leftMoveVector = VNorm(leftMoveVector);
	}

	// プレイヤー追跡　11/5-----------------------------------//
	auto pPlayer = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DByTag(Object3D::T_Player3D); // プレイヤーの情報をとる
	VECTOR moveDir = VGet(0.0f, 0.0f, 0.0f); // 今フレームの移動方向

	if (pPlayer != nullptr)
	{
		VECTOR playerPos = pPlayer->GetPosition(); // ここでプレイヤーのポジションを取ってくる
		VECTOR toPlayer = VSub(playerPos, mvPosition); // ここで方向ベクトルを出している
		toPlayer.y = 0.0f; // ここで、プレイヤーを追いかけて空に来ないようにしている
		mfDistanceDebug = VSize(toPlayer); // プレイヤーとの距離を入れている

		// クールタイムが残っていたら、NEUTRALにする
		// 逃げるやつじゃないならニュートラル
		if (Get_ENeetTime() > 0 && mType != EnemyType::Coward)
		{
			// 攻撃後のクールタイム中は停止アニメーション
			mpModel->SetLoopFinishState(ANIMATION_NEUTRAL);
			return;             // この関数の処理をここで抜ける
		}

		// 敵のYが１０００だったらブーストで追いかけるとかもありっぽいな

		if (mbFlyJump)
		{
			// プレイヤーと近くなったら
			if (mfDistanceDebug <= 800)
			{
				mbFlyJump = false; // 解除している
			}
			else
			{
				VECTOR dir = VNorm(toPlayer); // 向きはplayerにむけさせるため
				moveDir = dir;
				mfTargetAngle = atan2f(dir.x, dir.z);
				// 移動させる   速さを変えてる
				 // VAdd...ベクトルの加算 ここで移動速度や距離を変える 
				mvPosition = VAdd(mvPosition, VScale(dir, mfSpeed * 3.0f));

			}
		}

		// 遠かったら入るようにする
		if (playerPos.y >= 0 && !mbFlyJump &&
			mfDistanceDebug >= 4000 && mfDistanceDebug <= 5200.0f
			&& mbOkFly) // フラグがfalseだったら
		{
			mbFlyJump = true;
			mbJumpStart = true; // ジャンプをさせる　関数に入らないとFlyも発動しないから
			mbOkFly = false; // 上昇に入ったらtreu;
			return;
		}


		// これ以降の処理を、Flyをfalseじゃないとはいらないようにするのもありあり
		if (!mbFlyJump)
		{
			// 逃げる敵
			// 近づいたらにげるけど、近すぎたら殴ってくる
			if (mType == EnemyType::Coward && !mbJump
				&& mfDistanceDebug > 150 && !mbJump && mfDistanceDebug < mShotDist)
			{
				mbShot = false;
				mbAttackDistance = false;
				mbJumpStart = false;


				VECTOR escape = VNorm(VSub(mvPosition, playerPos)); // プレイヤーと逆方向
				moveDir = escape;

				mpModel->ChangeAnimation(ANIMATION_RUN);
				mfTargetAngle = atan2f(moveDir.x, moveDir.z);

				mvPosition = VAdd(mvPosition, VScale(moveDir, mfSpeed * 1.6f));
				return; // 他の行動をさせない
			}




			// 近かったら攻撃 下のだとランダムによっては通らなくなるから注意
			// 追記　150にしたほうがよいな
			if (mfDistanceDebug < 150 && !mbJump)

			{
				SetNearAttack(true); // 近接フラグをON
				mbAttackDistance = true; // 攻撃をONにするためのフラグ
				mbJumpStart = false;
				VECTOR dir = VNorm(toPlayer); // 向きはplayerにむけさせるため
				moveDir = dir;
				mfTargetAngle = atan2f(dir.x, dir.z);
			}


			// 少し近かったら、走る
			else if (mfDistanceDebug < mRunDist && !mbJump)
			{
				mbAttackDistance = false;
				mbShot = false;
				mbJumpStart = false;
				VECTOR dir = VNorm(toPlayer); // ここで正規化
				moveDir = dir; // 代入

				if (!mbJump) // ジャンプ中に走るモーションになるのを防いでいる
				{
					// 走るアニメーションに変更
					mpModel->ChangeAnimation(ANIMATION_RUN);
				}

				// プレイヤーの方向を向くようにY軸回転を求めている
				mfTargetAngle = atan2f(dir.x, dir.z);
				// 移動させる
				// VAdd...ベクトルの加算 ここで移動速度や距離を変える 
				mvPosition = VAdd(mvPosition, VScale(dir, mfSpeed));
			}



			// 銃で攻撃
			else if (mfDistanceDebug < mShotDist && !mbJump)
			{
				VECTOR dir = VNorm(toPlayer); // ここで正規化
				moveDir = dir; // 代入
				if (!mbJump) // ジャンプ中に走るモーションになるのを防いでいる
				{
					// 走るアニメーションに変更
					mpModel->ChangeAnimation(ANIMATION_RUN);
					mbJumpStart = false;
				}

				// プレイヤーの方向を向くようにY軸回転を求めている
				mfTargetAngle = atan2f(dir.x, dir.z);
				// 狙撃の人でとまるから動かす
				mvPosition = VAdd(mvPosition, VScale(dir, mfSpeed));


				// リロード中じゃないなら
				if (!mbReload)
				{
					mbShot = true; // 弾攻撃をON
					mbAttackDistance = false;
					mbJumpStart = false;
				}
				else // プレイヤー追いかける
				{
					mbAttackDistance = false;
					mbShot = false;
					mbJumpStart = false;
				}
			}



			// ジャンプする
			// ジャンプちゅうでも、距離に関係なく近づいてほしいからmbJumpも追加
			else if ((mfDistanceDebug > mShotDist && mfDistanceDebug < mJumpDist) || mbJump)

			{
				// ジャンプ中じゃない　かつジャンプ始めていないなら
				if (!mbJump && !mbJumpStart)
				{
					mbJumpStart = true;  // JumpをON
				}
				mbShot = false;

				VECTOR dir = VNorm(toPlayer); // 向きはplayerにむけさせるため
				moveDir = dir;
				mfTargetAngle = atan2f(dir.x, dir.z);

				// 移動させる   速さを変えてる
				 // VAdd...ベクトルの加算 ここで移動速度や距離を変える 
				mvPosition = VAdd(mvPosition, VScale(dir, mfSpeed * 1.2f));
			}


			// 追跡しない
			else if (!mbJump)
			{
				if (!mbJump) // ジャンプ中に走るモーションになるのを防いでいる
				{
					// 走るアニメーションに変更
					mpModel->ChangeAnimation(ANIMATION_RUN);
				}

				mbJumpStart = false;
				mbAttackDistance = false;
				mbShot = false;

				// 敵の移動処理
				if (HitFlag == false)
				{
					if (mnMoveCount == 1) // ２分の１のカウントが１なら
					{
						mvMoveVec = VAdd(mvMoveVec, upMoveVector); // 奥方向
					}
					else
					{
						mvMoveVec = VAdd(mvMoveVec, leftMoveVector); // 左方向
					}
				}
				else
				{
					if (mnMoveCount == 1) // ２分の１のカウントが１なら
					{
						mvMoveVec = VAdd(mvMoveVec, VScale(upMoveVector, -1.0f)); // 手前方向
					}
					else
					{
						mvMoveVec = VAdd(mvMoveVec, VScale(leftMoveVector, -1.0f)); // 右方向
					}
				}
				moveDir = VNorm(mvMoveVec); //　移動ベクトルを代入する
			}
			else
			{
				//DrawFormatString(200, 500, GetColor(255, 0, 0), "エルス文入った");
			}



		}
	}



	// 移動している状態であれば
	bool isMove2 = (mvMoveVec.x != 0.0f || mvMoveVec.z != 0.0f);
	if (isMove2)
	{

		// 移動方向を正規化しておく
		mvMoveVec = VNorm(mvMoveVec);

		// 新しい回転をセット 目標の回転率を求める
		// 移動方向の二つの値をいれて回転の値を求める
		mfTargetAngle = atan2f(mvMoveVec.x, mvMoveVec.z);

		// 移動させる
		// VAdd...ベクトルの加算
		// ここで移動速度や距離を変える 


		if (GetHp() == 1) // 死にそうなら速くする
		{
			mvPosition = VAdd(mvPosition, VScale(moveDir, mfSpeed * 2.0f));
		}
		else
		{
			mvPosition = VAdd(mvPosition, VScale(moveDir, mfSpeed));
		}
	}

}

void Enemy3D::Hit2D()
{
	// -----------------壁の画像とのあたり判定 壁★  -------------------------------------------------------------------///////
	// 壁との当たり判定を行う
	auto walls = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Wall3D);
	if (!walls.empty())
	{
		std::vector<Wall*> hitCounter; // 当たっている壁を数える ★★★★

		// 当たっている壁を調べる
		// ヒント： 現状の処理では１毎の壁しか最終的には判定されないので２枚（以上）に当たっていた処理を考える
		for (int i = 0; i < walls.size(); i++)
		{
			Wall* wall = dynamic_cast<Wall*>(walls.at(i)); // ダイナミックキャストする
			if (wall != nullptr)
			{
				std::vector<VERTEX3D> vertex = wall->GetVertex();

				bool hit = false; // ★★★★

				// プレイヤーを包み込むようなカプセル型の判定と、壁の三角形ポリゴンとの当たり判定を行う
				if (HitCheck_Capsule_Triangle(
					mvPosition,
					VAdd(mvPosition, VGet(0.0f, 200.0f, 0.0f)),
					80.0f,
					vertex.at(0).pos, vertex.at(1).pos, vertex.at(2).pos) ||

					HitCheck_Capsule_Triangle(
						mvPosition,
						VAdd(mvPosition, VGet(0.0f, 200.0f, 0.0f)),
						80.0f,
						vertex.at(3).pos, vertex.at(1).pos, vertex.at(2).pos)
					)
				{
					hit = true;// ★★★★
				}

				if (hit) // hitがtrueなら　　// ★★★★
				{
					hitCounter.push_back(wall); // 当たっている壁を記録する

					// フラグを逆にして、進む方向も逆にしている ★
					if (HitFlag == false)
					{
						HitFlag = true;

						if (rand() % 2 == 0)
						{
							mnMoveCount = 1;
						}
						else
						{
							mnMoveCount = 2;
						}
					}
					else
					{
						HitFlag = false;

						if (rand() % 2 == 0)
						{
							mnMoveCount = 1;
						}
						else
						{
							mnMoveCount = 2;
						}
					}
				}
				else
				{
					mbWallHit = false;
				}
			}
		}
		// 当たっている壁の数が１つだったら壁沿いベクトルを計算  // ★★★★
		// 当たっている壁の数が２つある場合は前のポジションで固定してみる
		if (hitCounter.size() == 1)
		{
			mbWallHit = true;
			std::vector<VERTEX3D> vertex = hitCounter[0]->GetVertex();

			// 壁に沿って行くようなベクトル取得
			VECTOR slide = VGet(0.0f, 0.0f, 0.0f); // 壁沿いベクトル
			float a = VDot(VScale(mvMoveVec, -1.0f), vertex.at(0).norm); // 移動ベクトルの反対のベクトルと、壁の法線との内積を求める
			slide = VAdd(mvMoveVec, VScale(vertex.at(0).norm, a));       // 壁沿いベクトルを計算

			// 求めた内積と壁沿いベクトルは同じ方向
			// 一旦座標を戻してから壁に沿って歩くようにする

			 // 追加11/4　壁方向に走った時にガタつくのを解消 ★
			mvPosition.x = mvOldPosition.x;
			mvPosition.z = mvOldPosition.z;
			mvPosition = VAdd(mvPosition, VScale(slide, 10.0f));

		}
		else if (hitCounter.size() >= 2)
		{
			mbWallHit = true;
			// 追加11/4　壁方向に走った時にガタつくのを解消 ★
			mvPosition.x = mvOldPosition.x;
			mvPosition.z = mvOldPosition.z;
		}
	}

	// 床との当たり判定 チャレンジ ★★★
	auto floors = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Floor3D);
	if (!floors.empty())
	{
		std::vector<Floor*> hitCounter; // 当たっている床を数える ★★★★

		// 当たっている壁を調べる
		// ヒント： 現状の処理では１毎の壁しか最終的には判定されないので２枚（以上）に当たっていた処理を考える
		for (int i = 0; i < floors.size(); i++)
		{
			Floor* floor = dynamic_cast<Floor*>(floors.at(i)); // ダイナミックキャストする
			if (floor != nullptr)
			{
				std::vector<VERTEX3D> vertex = floor->GetVertex();

				bool hit = false; // ★★★★

				// プレイヤーを包み込むようなカプセル型の判定と、壁の三角形ポリゴンとの当たり判定を行う
				if (HitCheck_Capsule_Triangle(
					VAdd(mvPosition, VGet(0.0f, 220.0f, 0.0f)),
					VAdd(mvPosition, VGet(0.0f, 250.0f, 0.0f)),
					40.0f,
					vertex.at(0).pos, vertex.at(1).pos, vertex.at(2).pos) ||

					HitCheck_Capsule_Triangle(
						VAdd(mvPosition, VGet(0.0f, 220.0f, 0.0f)),
						VAdd(mvPosition, VGet(0.0f, 250.0f, 0.0f)),
						40.0f,
						vertex.at(3).pos, vertex.at(1).pos, vertex.at(2).pos)
					)
				{
					hit = true;
					// 床に沿って行くようなベクトル取得                           人
					VECTOR slide = VGet(0.0f, 0.0f, 0.0f); // 床沿いベクトル   ========     
					float a = VDot(VScale(mvMoveVec, -1.0f), vertex.at(0).norm); // 移動ベクトルの反対のベクトルと、壁の法線との内積を求める
					slide = VAdd(mvMoveVec, VScale(vertex.at(0).norm, a));       // 壁沿いベクトルを計算

					// 求めた内積と壁沿いベクトルは同じ方向
					// 一旦座標を戻してから壁に沿って歩くようにする
					mvPosition = mvOldPosition;
					mvPosition = VSub(mvPosition, VScale(slide, -10.0f));
				}

				if (hit)
				{
					mbFloorHit = true;
				}
				else
				{
					mbFloorHit = false;
				}
			}
		}
	}

	if (mbFloorHit && mbWallHit) // 壁にも床にも当たっていたらとめる
	{
		mvPosition = mvOldPosition;
	}
}

// 移動による回転処理
void Enemy3D::RotationByMove()
{
	// 現在の回転値から目標の回転値の差分を求める
	float  subAngle = mfTargetAngle - mfAngle;

	// ある方向からある方向の差が180以上（以下）にならないはずなので
	// 差の値が180以上（以下）になっていたら矯正する
	if (subAngle < -DX_PI_F)
	{
		subAngle += DX_TWO_PI_F;
	}
	else if (subAngle > DX_PI_F)
	{
		subAngle -= DX_TWO_PI_F;
	}

	// 角度の差分を徐々に０に近づける
	if (subAngle > 0.0f)
	{
		subAngle -= ROTATE_SPEED;
		if (subAngle < 0.0f)
		{
			subAngle = 0.0f;
		}
	}
	// マイナス値だった場合
	else if (subAngle < 0.0f)
	{
		subAngle += ROTATE_SPEED;
		if (subAngle > 0.0f)
		{
			subAngle = 0.0f;
		}
	}

	// 今向いてほしい角度を産出
	mfAngle = mfTargetAngle - subAngle;

	// 回転値を設定
	// 行きたい方向＋円周率
	mvRotation.y = mfAngle + DX_PI_F;
	// モデルに伝える
	// これがないと変らない
	mpModel->SetRotation(mvRotation);
}


// 追加 11/5
// 攻撃処理
void Enemy3D::Attack()
{
	// ここにクールタイムを設定すると、始まって、当たっていない状態でも、攻撃が終わったと判定されちゃうから
	// あたり判定の処理がバグる

	// 攻撃モーションの呼び出し
	mpModel->ChangeAnimation(ANIMATION_ATTACK);
	// ループはさせない
	mpModel->SetLoop(false);

	if (!mbJump) // ジャンプ中に歩きモーションになるのを防いでいる 10/27 ★
	{
		// モーション後は待機アニメーションに戻す
		mpModel->SetLoopFinishState(ANIMATION_NEUTRAL);
		//mbAttackDistance = false; // ここ書くと攻撃が入らなくなる
	}

	// 現在のアニメーション情報を取得
	AnimationState now = mpModel->GetNowState();


	// 敵情報の取得（複数当たっても良いように）
	auto pPlayerList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Player3D);

	// 攻撃モーションでなければ
	if (now != ANIMATION_ATTACK)
	{
		if (!pPlayerList.empty())
		{
			// 敵のサイズを数えている
			for (int i = 0; i < pPlayerList.size(); i++)
			{
				auto pPlayer = pPlayerList[i];

				if (pPlayer != nullptr)
				{
					pPlayer->SetAttackFlag(false);
				}
			}
		}
		mbAttackSE = false;
		SetNearAttack(false);// 近接OFF
	}
	// 攻撃モーション中 かつ　近接がONなら
	else if (now == ANIMATION_ATTACK && GetNearAttack())
	{
		if (!mbAttackSE)
		{
			// SE再生
			Master::mpSoundManager->PlaySE(SoundManager::SE_ATTACK, 180); // シュってやつ
			mbAttackSE = true; // すぐtrueにしてる
		}

		if (!pPlayerList.empty())
		{
			// 敵のサイズを数えている
			for (int i = 0; i < pPlayerList.size(); i++)
			{
				bool WallWeaponHit = false;	// 武器と壁が当たっているかのフラグ
				auto pPlayer = pPlayerList[i];
				// ダイナミックキャストでも良い
				//Object3D* pObjs = dynamic_cast<Object3D*>(pObj.at(i));

				if (pPlayer != nullptr)
				{
					// ながい武器でも当たるようにした
					// なくても武器の位置から出るからおｋ
					VECTOR weaponBase = mpWeapon->Get_Base_WeaponPosition();
					VECTOR muzzlePos = mpWeapon->GetFrameWorldPosition("銃口1");
					// 当たり判定を行う
					bool isHit = HitCheck_Sphere_Capsule(
						muzzlePos,  // スフィアの中心座標
						30.0f,                             // スフィアの半径
						pPlayer->GetPosition(),               // カプセルの座標１（下側）
						VAdd(pPlayer->GetPosition(), VGet(0.0f, 150.0f, 0.0f)), // カプセルの座標２（上側）
						40.0f                              // カプセルの半径
					);

					// 敵のHPを取得
					// dynamic_cast... 動的変換
					Player3D* pTarget = dynamic_cast<Player3D*>(pPlayer);

					// 壁に武器がめり込んでいるかチェック
					auto wall3DList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Stage3D);
					for (auto obj : wall3DList)
					{
						Stage* wall = dynamic_cast<Stage*>(obj);
						VECTOR hitPos, hitNormal;
						// 手元か剣先がめり込んでたr
						if (wall->CheckHit_Capsule_Wall(weaponBase, weaponBase, 55.0f, hitPos, hitNormal)
							|| wall->CheckHit_Capsule_Wall(muzzlePos, muzzlePos, 50.0f, hitPos, hitNormal))
						{
							WallWeaponHit = true;
							break;
						}
					}
					mbIsWallWeapon = WallWeaponHit; // フラグ代入

					if (isHit)// isHit が true なら
					{
						// 壁に埋まってたらダメージスキップ
						if (WallWeaponHit) continue;

						if (!pTarget->PlayerBariaAndCheck()) // もしバリアが有効じゃなかったら 1112
						{
							if (!pTarget->IsAttackFlag()) // 攻撃取得の関数を呼ぶ
							{
								// 追加11/5
								Set_ENeetTime(60.0f); // クールタイムの設定  300で５秒ぐらいかも

								// 自分が攻撃したってのを渡す
								pTarget->Damage(mpWeapon->Get_Base_Attack(), this); // Unitのダメージ処理呼ぶ
								pTarget->SetAlpha(); // 最大値をセット
							}
						}
						else
						{
							if (!mbBariaSE) // 連続にならぬように
							{
								// エフェクトを出 バリア三角  武器が当たった位置だから武器に出している
								new Effect4(muzzlePos, "Resource/2d/BariaMark2.png.");
								// SE再生
								Master::mpSoundManager->PlaySE(SoundManager::SE_BARIA_HIT, 180); // バリアと当たった時
								mbBariaSE = true;
							}
						}
					}
					else // 当たっていなかったら
					{
						pTarget->SetAttackFlag(false);
						mbBariaSE = false;
					}
				}
			}
		}
	}
}

// ジャンプ処理 ★★★★★★
void Enemy3D::Jump()
{
	// スペースキーが押された かつ まだジャンプしていない場合
	if (!mbJump && mbJumpStart)
	{
		mbJump = true; // フラグをtrueにする
		mfJumpPower = JUMP_POWER; // ジャンプ力の初期設定
		mfTimeCount = 0.0f; // 時間カウントリセット

		// スタミナゲージ初期化（必要ならここは省略してもOK）
		// mfSkyGauge = GAUGE_POWER;

		// ジャンプ開始アニメーション
		mpModel->ChangeAnimation(ANIMATION_JUMP_IN);
		mpModel->SetLoop(false);
		mpModel->SetLoopFinishState(ANIMATION_MAX);
	}

	// --- ジャンプ中の処理 ---
	if (mbJump)
	{
		mfTimeCount++;

		// --- スタミナ減少 ---
		if (mbFlyJump)
		{
			SubBoost(2.0f);
		}
		else
		{
			SubBoost(1.0f);  //通常時
		}


		// --- アニメーション切替 ---
		if (mpModel->IsAnimationLoopFinish() || mbFall)
		{
			mpModel->ChangeAnimation(ANIMATION_JUMP_LOOP);
			mpModel->SetLoop(false);
			mpModel->SetLoopFinishState(ANIMATION_MAX);
			mpModel->SetAnimationBlend(false);
		}


		//mvPosition = VAdd(mvPosition, VGet(0.0f, mfJumpPower, 0.0f));
		//mfJumpPower -= 0.98f; // 重力


		// 上昇中じゃないならふつうの重力　ジャンプのときに入る
		if (!mbFall && !mbFlyJump)
		{
			mvPosition = VAdd(mvPosition, VGet(0.0f, mfJumpPower, 0.0f));
			mfJumpPower -= 0.96f; // 重力 調整
			FloorHit3D();

			if (mbFloorHit)
			{
				mbJump = false;
			}
		}
		// 上昇中なら銃力を弱める
		else if (!mbFall && mbFlyJump)
		{
			mvPosition = VAdd(mvPosition, VGet(0.0f, mfJumpPower, 0.0f));
			mfJumpPower -= 0.55f; // 重力 調整
			FloorHit3D();

			if (mbFloorHit)
			{
				mbJump = false;
			}
		}
		else
		{
			mvPosition.y -= 18.0f; // 落下処理
		}


		// --- 地面到達 --- 
		if (mvPosition.y <= mfGroundY)  // ★ヒットポジションにするジャンプ中途徒歩で分けるO
		{
			mbJump = false;
			mbFall = false; // 当たっていない自動落下をfalseにする
			mbJumpStart = false; // ここやらないと無限にジャンプする
			mvPosition.y = mfGroundY;
			mfJumpPower = JUMP_POWER;
			mfTimeCount = 0.0f;
			mbFlyJump = false;

			// 着地アニメーション
			mpModel->ChangeAnimation(ANIMATION_JUMP_OUT);
			mpModel->SetLoop(false);
			mpModel->SetLoopFinishState(ANIMATION_NEUTRAL);
		}


		if (mbFlyJump && GetBoost() > 0.0f)
		{
			mfJumpPower += 0.3f;

			mJumpEffectDistance += fabs(mfJumpPower);
			if (mJumpEffectDistance >= 100.0f)
			{
				new Effect(
					VAdd(mpModel->GetPosition(), VGet(0.0f, 150.0f, 0.0f)),
					"Resource/2d/Damage2.png"
				);
				mJumpEffectDistance = 0.0f;
			}
		}

	}
	// --- 落下処理（スタミナ切れなど） ---
	else if (GetBoost() <= 0.0f && mbJump && !mbFlyJump)
	{
		mvPosition = VAdd(mvPosition, VGet(0.0f, mfJumpPower, 0.0f));
		mfJumpPower -= 0.98f;

	}
}

// playerの表示非表示のゲッターセッターをとる。
void Enemy3D::DebugMode()
{
	// 敵情報の取得（複数当たっても良いように）
	auto pPlayerList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Player3D);

	if (!pPlayerList.empty())
	{
		// 敵のサイズを数えている
		for (int i = 0; i < pPlayerList.size(); i++)
		{
			// ダイナミックキャストでも良い
			Player3D* pPlayer = dynamic_cast<Player3D*>(pPlayerList.at(i));

			if (pPlayer != nullptr)
			{
				mbdebugMode = pPlayer->GetDebugMode();
			}
		}
	}
}

// 敵同士がぶつかった時の処理
void Enemy3D::EnemytoEnemyHit()
{
	// 敵情報の取得 当たった時の判定を敵同士でやるから
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Enemy3D);

	if (!pEnemyList.empty())
	{
		for (int i = 0; i < pEnemyList.size(); i++)
		{
			Object3D* pEnemy = dynamic_cast<Enemy3D*>(pEnemyList.at(i));
			if (pEnemy != nullptr)
			{
				if (pEnemy == this)
				{
					continue; // 自分同士でもはいっちゃうからここで処理
				}

				// 敵同士カプセル カプセル
				bool isHit = HitCheck_Capsule_Capsule(
					GetPosition(),                                   // 自分 下
					VAdd(GetPosition(), VGet(0.0f, 150.0f, 0.0f)),   // 自分 上
					40.0f,                                           // 自分 半径

					pEnemy->GetPosition(),                                 // 相手 下
					VAdd(pEnemy->GetPosition(), VGet(0.0f, 150.0f, 0.0f)), // 相手 上
					40.0f                                                  // 相手 半径
				);

				if (isHit)
				{
					// 相手 から 自分 の方向
					VECTOR pushDir = VSub(this->GetPosition(), pEnemy->GetPosition());
					const float pushPower = 1.0f; // 押し出し量（調整用）
					pushDir = VNorm(pushDir); // 正規化


					pushDir.y = 0; // ここで、上下に押さないようにしている
					mvPosition = VAdd(mvPosition, VScale(pushDir, pushPower));

					WallHit3D(); // ここで判定して壁にめりこまないようにしている

				}
			}
		}
	}
}


void Enemy3D::BulletAttack()
{
	if (!mbHitUp)
	{
		// クールタイムをBullet3Dで設定したからそれが０だったら攻撃モーションに入るように
		if (mpWeapon->Get_Base_Bullet() > 0 && Get_ENeetTime() <= 0) // 弾があったら
		{
			// 攻撃モーションの呼び出し
			mpModel->ChangeAnimation(ANIMATION_ATTACK);
			// ループはさせない
			mpModel->SetLoop(false);

			if (!mbJump) // ジャンプ中に歩きモーションになるのを防いでいる 10/27 ★
			{
				// モーション後は待機アニメーションに戻す
				mpModel->SetLoopFinishState(ANIMATION_NEUTRAL);
			}
			Set_ENeetTime(60.0f); // クールタイムの設定  300で５秒ぐらいかも
		}

		// 現在のアニメーション情報を取得
		AnimationState now = mpModel->GetNowState();
		bool WallWeaponHit = false;	// 武器と壁が当たっているかのフラグ

		// 敵（プレイヤー）情報の取得（複数当たっても良いように）
		auto pPlayerList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
			->GetObject3DListByTag(Object3D::T_Player3D);

		// 攻撃モーションでなければ
		// ここに入らないのが問題
		if (now != ANIMATION_ATTACK /* && mbAttackStart*/)
		{
			if (!pPlayerList.empty())
			{
				// プレイヤーのサイズを数えている
				for (int i = 0; i < pPlayerList.size(); i++)
				{
					auto pPlayer = pPlayerList[i];

					if (pPlayer != nullptr)
					{
						pPlayer->SetAttackFlag(false);
					}
				}
			}
		}

		// 攻撃モーション中なら
		else
		{
			// ながい武器でも当たるようにした
			// なくても武器の位置から出るからおｋ
			VECTOR weaponBase = mpWeapon->Get_Base_WeaponPosition();
			VECTOR muzzlePos = mpWeapon->GetFrameWorldPosition("銃口1");
			// 壁に武器がめり込んでいるかチェック
			auto wall3DList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Stage3D);
			for (auto obj : wall3DList)
			{
				Stage* wall = dynamic_cast<Stage*>(obj);
				VECTOR hitPos, hitNormal;
				// 手元か剣先がめり込んでたr
				if (wall->CheckHit_Capsule_Wall(weaponBase, weaponBase, 55.0f, hitPos, hitNormal)
					|| wall->CheckHit_Capsule_Wall(muzzlePos, muzzlePos, 50.0f, hitPos, hitNormal))
				{
					WallWeaponHit = true;
					break;
				}
			}
			mbIsWallWeapon = WallWeaponHit; // フラグ代入


			mfShotCounter++;
			if (mfShotCounter >= SHOT_FRAME && mpWeapon->Get_Base_Bullet() > 0)
			{
				mfShotCounter = 0;
				mpWeapon->SubBullet(1); // 弾を減らす 1

				// プレイヤーの向きの取得
				for (int i = 0; i < pPlayerList.size(); i++)
				{
					auto pPlayer = pPlayerList[i];
					if (pPlayer != nullptr)
					{
						VECTOR Pos = VGet(mvPosition.x + 20.0f, mvPosition.y + 130.0f, mvPosition.z + 10.0f);

						VECTOR playerPos = pPlayer->GetPosition(); // ここでプレイヤーのポジションを取ってくる
						VECTOR toPlayer = VSub(playerPos, mvPosition); // ここで方向ベクトルを出している
						float distance = VSize(toPlayer); // プレイヤーとの距離を入れている
						VECTOR dir = VNorm(toPlayer); // 正規化


						Player3D* target = dynamic_cast<Player3D*>(pPlayer);

						if (WallWeaponHit) continue;


						// SE再生
						Master::mpSoundManager->PlaySE(SoundManager::SE_PAN, 150); //銃ダメージ

						// ここは適用のBulletの今ストも必要
						// 弾生成
						new Bullet3D(
							Object3D::T_Enemy3D,
							this,       // 誰が打ったかの特定
							target,
							Pos,                  // 弾の初期位置
							"Resource/Weapon/弾/弾1.mqo",    // 弾のモデル
							dir,                           // 弾の進行方向
							mpWeapon->Get_Base_Attack(),   // 武器に応じたダメージ
							"Resource/2d/BulletEffect.png" // エフェクトの画像
						);


						if (mpWeapon->Get_Base_Bullet() == 0)
						{
							mbReload = true; // りロードを開始している
							// 弾が０だったら攻撃モーションにもならない
						}
					}
				}
			}
		}
	}
}

void Enemy3D::FlyReset()
{
	// coolタイムをランダムにする
	mfFlyCount = 0.0f;
	mfFlyMaxCount = 2000.0f + GetRand(3000); // 2000から5000

}