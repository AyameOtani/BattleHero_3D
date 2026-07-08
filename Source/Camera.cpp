#include "Camera.h"
#include "Master.h"
#include "ObjectManager.h"
#include "Scene3D.h"
#include "InputManager.h"
#include "Effect.h"
#include "Enemy3D.h"

#include <cmath> // cosなどを使うためのインクルード
#include "TutorialManager.h"

Camera::Camera()
	: mfHorizontalAngle(0.0f)
	, mfVerticalAngle(0.0f)
	, mvPosition(VGet(0.0f, 0.0f, 100.0f))     // カメラの座標
	, mvLookAtPosition(VGet(0.0f, 0.0f, 0.0f)) // カメラが見ている方向
	, mpTarget(nullptr)                        // まずは空っぽ
	, mpTargetEnemy(nullptr)                   // まずは空っぽ 敵追加 1117
	, mnRockOnHandle(-1)                       // まずは-1にしてる 
	, mnFarOnHandle(-1)                        // まずは-1にしてる 
	, mnLookOnHandle(-1)                       // まずは-1にしてる 
	, mnRockFarHandle(-1)                      // まずは-1にしてる 
	, mnShakeTime(0)
	, mnShakeTimeCount(0)
	, mfShakeAngle(0.0f)
	, mfShakeTimeCounter(0.0f)
	, mfShakeTime(0.0f)
	, mfShakeWidth(0.0f)
	, mfShakeAngleSpeed(0.0f)
	, mfStepTime(0.0f)
	, mvShakePosition(VGet(0.0f, 0.0f, 0.0f))
{

}
Camera::~Camera()
{
	// 画像ハンドルの破棄
	DeleteGraph(mnRockOnHandle);
	DeleteGraph(mnFarOnHandle);
	DeleteGraph(mnLookOnHandle);
	DeleteGraph(mnRockFarHandle);
}

void Camera::Initialize()
{
	// 赤ロックオン画像の読み込み
	mnRockOnHandle = LoadGraph("Resource/2d/RockOn.png");
	if (mnRockOnHandle == -1)
	{
		printfDx("画像の読み込みに失敗しました");
	}
	// 緑ロックオン画像の読み込み
	mnFarOnHandle = LoadGraph("Resource/2d/Far.png");
	if (mnFarOnHandle == -1)
	{
		printfDx("画像の読み込みに失敗しました");
	}
	// 緑ロックオン画像の読み込み
	mnLookOnHandle = LoadGraph("Resource/2d/Look.png");
	if (mnLookOnHandle == -1)
	{
		printfDx("画像の読み込みに失敗しました");
	}
	// 深い緑ロックオン画像の読み込み
	mnRockFarHandle = LoadGraph("Resource/2d/RockFar.png");
	if (mnRockFarHandle == -1)
	{
		printfDx("画像の読み込みに失敗しました");
	}

	// カメラのクリッピング距離の設定----カメラの映す距離（どこからどこまでを映すか）
	SetCameraNearFar(100.0f, 50000.0f); // 100 ～ 50000までオブジェクトを映す

	// 背景色の設定 (今は灰色)
	SetBackgroundColor(162, 209, 212);

	// カメラ設定を反映  上方向はベクトルYだから
	SetCameraPositionAndTarget_UpVecY(mvPosition, mvLookAtPosition);

	// 更新処理を一度行っておく
	Update();
}

void Camera::Update()
{
	// 今のシーンがチュートリアルじゃないなら処理をする
	if (Master::mpSceneManager->GetCurrentSceneType() != SceneManager::SCENE_TYPE::TUTORIAL_SCENE_3D)
	{
		mfUp = 100.0f;
	}
	else
	{
		mfUp = 140.0f;
	}


	// カメラの向く対象の処理
	if (mpTarget == nullptr)
	{
		// プレイヤーのオブジェクトを探し出している
		mpTarget = Master::mpSceneManager->GetCurrentScene()
			->GetObjectManager()->GetObject3DByTag(Object3D::T_Player3D);
	}
	// ロックオン中にターゲットが消えたらのときの処理
	if (mpTargetEnemy != nullptr)
	{
		// 敵のオブジェクトを探しているs
		auto pEnemyList = Master::mpSceneManager->GetCurrentScene()
			->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);

		bool RockOnEnemy = false; // ロック中の敵がまだいるかのフラグ
		for (int i = 0; i < pEnemyList.size(); i++)
		{
			auto pEnemy = pEnemyList[i];
			if (pEnemy != nullptr)
			{
				Enemy3D* pTarget = dynamic_cast<Enemy3D*>(pEnemy);
				if (pTarget == mpTargetEnemy) // ロックオン中の敵かどうか
				{
					RockOnEnemy = true;
					break; // 見つかったらチェックはいらない
				}
			}
		}
		if (!RockOnEnemy) // ロックオン中の敵がリストに存在しなかったら解除する
		{
			mpTargetEnemy = nullptr;
			SetRockOn(false);
		}
	}

	
	// ロックオン中の敵が消えた場合はフラグを下げる
	if (IsRockOn() && mpTargetEnemy == nullptr)
	{
		SetRockOn(false);
		mbRocking = false;
		mbFaring = false;
		mbNearDistance = false;
	}

	if (mpTarget != nullptr) // ターゲットがいなかったら
	{
		// 基準座標を対象の座標にする
		mvLookAtPosition = mpTarget->GetPosition(); // ターゲットからポジションをとってくる
		mvLookAtPosition.y += 110.0f;
	}
	else
	{
		// 注視点を少し上にずらす
		mvLookAtPosition.y = 110.0f;
	}

	if (mpTarget == nullptr && (Master::mpSceneManager->GetCurrentSceneType() == SceneManager::SCENE_TYPE::SCENE_3D))
	{
		SetRockOn(false);
		mpTargetEnemy = nullptr;

		mvLookAtPosition = VGet(0, 110, 0);
		mvPosition = VGet(0, 200, -300);

		SetCameraPositionAndTarget_UpVecY(mvPosition, mvLookAtPosition);
		return;
	}


	// 注視点
	if (mbKillerMode && mpKillerTarget != nullptr)
	{
		// 現在の敵の座標を一時的に取得
		VECTOR currentPos = mpKillerTarget->GetPosition();

		// 座標の妥当性チェック
		// 座標が  マイナス 異常値ではないか確認 Yが-１００以上(段差マップあるから) かつXが０じゃないなら
		if (currentPos.y > -100.0f && currentPos.x != 0.0f)
		{
			// バックアップを更新
			mvKillerLastPos = currentPos;
		}
		// 異常値ならifに入らない  mvKillerLastPos は直前の正常な位置を維持する
		// 注視点には、常に「信頼できる最後の座標」を使う
		mvLookAtPosition = mvKillerLastPos;

		// キラーモードなら犯人の敵を映す
		// mvLookAtPosition = mpKillerTarget->GetPosition();
		
		mvLookAtPosition.y += 100.0f;
		mfKillerTimer += 1.0f / 60.0f;
	}
	else if (mpTarget != nullptr)
	{
		// 通常時はプレイヤーを映す
		mvLookAtPosition = mpTarget->GetPosition();
		mvLookAtPosition.y += 110.0f;
	}

	// ★New★
	// 画面揺れ処理
	Shake();

	//この中で処理は完結する　分かりやすいように
	{
		VECTOR temp;   // 作業用変数

		// 球面上の座標を求める
		//   ★DX_PI_F は 3.14 と同じ意味
		// 250.0f は注視点からどれだけ離れているかという意味
		// 距離は変えないで回転したい
		const float distance = 250.0f;
	    temp.x = distance * cosf(mfVerticalAngle / 180.0f * DX_PI_F) * sinf(mfHorizontalAngle / 180.0f * DX_PI_F);    // X座標
		temp.y = distance * sinf(mfVerticalAngle / 180.0f * DX_PI_F);                                                 // Y座標
		temp.z = -(distance * cosf(mfVerticalAngle / 180.0f * DX_PI_F) * cosf(mfHorizontalAngle / 180.0f * DX_PI_F)); // Z座標

		//if (!IsSlideCamera()) // 追加 1117 いらない？ 0115
		{
			// 求めた座標と注視点の座標を足した位置がカメラ座標になる
			mvPosition = VAdd(temp, mvLookAtPosition);
		}

		// ロックオン中　かつターゲットがいたら
		if (IsRockOn() && mpTargetEnemy)
		{
			float EnemyY = mpTargetEnemy->GetPosition().y; // 敵のYをゲット
			float PlayerY = mpTarget->GetPosition().y;     // プレイヤーのYをゲット

			float diffY = EnemyY - PlayerY; // 敵の位置から自分の位置を引いてる

			// 敵が上にいるほどカメラを下げたい
			// ここの0.0fは負の数にならないように
			if (diffY > 0.0f)
			{
				mvPosition.y -= diffY * 0.1f; // ここで敵が上がるほど *0.2下げている
				// 上限下限
				if (mvPosition.y < 30.0f)
				{
					mvPosition.y = 30; // 30で床の裏が見えないからおK
				} // 追記　上限を決めちゃうと注視点が110だからバグるからやらないほうが良い。
			}

		}
		// カメラ設定を反映
		SetCameraPositionAndTarget_UpVecY(mvPosition, mvLookAtPosition);
	}

	UpdateRotation();
	Distance(); // 呼びだす
	//DrawNearMark(); // 呼びだす


	// ★New★
	// 画面揺れの分を加算するように変更
	// 画面揺れの分を加算するように変更
	// カメラ設定を反映
	SetCameraPositionAndTarget_UpVecY(VAdd(mvPosition, mvShakePosition), VAdd(mvLookAtPosition, mvShakePosition));
}

// ★New★
// 画面揺れ
void Camera::Shake()
{
	if (mfShakeTimeCounter < mfShakeTime)
	{
		// sinf を利用して揺らし座標を算出
		// note: 一旦Y座標だけを揺らしてみる
		mvShakePosition.y = sinf(mfShakeAngle) * (1.0f - (mfShakeTimeCounter / mfShakeTime)) * mfShakeWidth;
		mvShakePosition.x = 0.0f;
		mvShakePosition.z = 0.0f;

		// 揺らし処理に使用する sinf に渡す角度の変更処理
		mfShakeAngle += mfShakeAngleSpeed * mfStepTime;

		// 揺らす時間を経過させる
		mfShakeTimeCounter += mfStepTime;
	}
	else
	{
		// 揺らされていない場合は揺らし処理による加算座標を０にする
		mvShakePosition = VGet(0.0f, 0.0f, 0.0f);
	}
}


// ★New★
// 画面揺れ設定
void Camera::SetupShake(float time, float width, float angleSpeed, float stepTime)
{
	mfShakeTimeCounter = 0.0f;
	mfShakeTime = time;
	mfShakeWidth = width;
	mfShakeAngleSpeed = angleSpeed;
	mfStepTime = stepTime;
}



void Camera::DrawNearMark()
{
	// 敵情報を取得
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);

	float BigDistance = FLT_MAX; // 距離を比較するために、初めはいっちゃん大きいのを入れる
	Enemy3D* NearEnemy = nullptr; // 近い敵をnullにしてく
	if (!pEnemyList.empty())
	{
		for (int i = 0; i < pEnemyList.size(); i++)// 敵のサイズを数えている
		{
			auto pEnemy = pEnemyList[i];

			if (pEnemy != nullptr)
			{
				// 敵の情報を取得
				Enemy3D* pTarget = dynamic_cast<Enemy3D*>(pEnemy);
				// プレイヤーと敵の距離を求めてる
				float distance = VSize(VSub(mpTarget->GetPosition(), pTarget->GetPosition()));

				if (distance <= 2000.0f) // 一定距離以内ならマーク対象
				{
					// 追跡している敵には青を出さないまたは、ロックオン中じゃないなら青を出す
					if (pTarget != mpTargetEnemy || !IsRockOn())
					{
						VECTOR FilePos = VGet(pTarget->GetPosition().x,
							pTarget->GetPosition().y + mfUp, pTarget->GetPosition().z);

						DrawBillboard3D(FilePos, 0.5f, 0.5f, 220.0f, 0.0f, mnLookOnHandle, TRUE);
					}
				}

				if (distance < BigDistance) //ここでいちばん近い敵を探す
				{
					BigDistance = distance; // ここに代入して、いちばん近い敵をどんどん探す

					NearEnemy = pTarget; // 近い敵をターゲットにする
				}
			}
		}
	}

	if (IsRockOn())
	{
		// ロックオン中だったら、敵を更新しない　自動切り替え防ぐ
	}
	else
	{
		// 一番近い敵を追従用に設定　　ロックオン中じゃなかったら更新
		mpTargetEnemy = NearEnemy;
	}

	//DrawFormatString(600, 600, GetColor(255, 0, 0), "%f,%f,%f", mvPosition.x, mvPosition.y, mvPosition.z);
}

void Camera::UpdateRotation()
{

	// 今のシーンがチュートリアルじゃないなら処理をする
	if (Master::mpSceneManager->GetCurrentSceneType() != SceneManager::SCENE_TYPE::TUTORIAL_SCENE_3D)
	{
		if (!mpTargetEnemy || !mpTarget)
		{
			return; // 敵がいなければ処理しない　例外入らない
		}
	}

	// ロックオン中じゃない　かつ　カウントダウン中じゃないなら動かせる
	if (!IsRockOn() && !GetCountdown())
	{
		int StickX, StickY;
		GetJoypadAnalogInputRight(&StickX, &StickY, DX_INPUT_PAD1);
		const int stickDeadZone = 50;

		// 水平方向
		if (CheckHitKey(KEY_INPUT_LEFT) || StickX < -stickDeadZone)
		{
			mfCurrentSpeedH -= ADD_SPEED;
			if (mfCurrentSpeedH < -MAX_SPEED)	 // ここで-なんかになって
			{
				mfCurrentSpeedH = -MAX_SPEED;
			}
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT) || StickX > stickDeadZone)
		{
			mfCurrentSpeedH += ADD_SPEED;
			if (mfCurrentSpeedH > MAX_SPEED)
			{
				mfCurrentSpeedH = MAX_SPEED;
			}
		}
		else
		{
			// 徐々に遅く
			if (mfCurrentSpeedH > 0)
			{
				mfCurrentSpeedH -= SUB_SPEED;
				if (mfCurrentSpeedH < 0)
				{
					mfCurrentSpeedH = 0;
				}
			}
			else if (mfCurrentSpeedH < 0)
			{
				mfCurrentSpeedH += SUB_SPEED;
				if (mfCurrentSpeedH > 0)
				{
					mfCurrentSpeedH = 0;
				}
			}
		}


		// 垂直方向
		if (CheckHitKey(KEY_INPUT_UP) || StickY < -stickDeadZone)
		{
			mfCurrentSpeedV += ADD_SPEED * 1.7f;
			if (mfCurrentSpeedV > MAX_SPEED) mfCurrentSpeedV = MAX_SPEED;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN) || StickY > stickDeadZone)
		{
			mfCurrentSpeedV -= ADD_SPEED * 1.7f;
			if (mfCurrentSpeedV < -MAX_SPEED) mfCurrentSpeedV = -MAX_SPEED;
		}
		else
		{
			// 徐々に遅く
			if (mfCurrentSpeedV > 0)
			{
				mfCurrentSpeedV -= SUB_SPEED * 1.2f;
				if (mfCurrentSpeedV < 0)
				{
					mfCurrentSpeedV = 0;
				}
			}
			else if (mfCurrentSpeedV < 0)
			{
				mfCurrentSpeedV += SUB_SPEED * 1.2f;
				if (mfCurrentSpeedV > 0)
				{
					mfCurrentSpeedV = 0;
				}
			}
		}

		//  計算した速度を角度に反映
		mfHorizontalAngle += mfCurrentSpeedH; // それをここでやっても+ だけど-5とかだから左に行くはず
		mfVerticalAngle += mfCurrentSpeedV;

		// 角度の正規化clamp
		if (mfHorizontalAngle >= 180.0f) mfHorizontalAngle -= 360.0f;
		if (mfHorizontalAngle < -180.0f) mfHorizontalAngle += 360.0f;

		if (mfVerticalAngle >= 80.0f) mfVerticalAngle = 80.0f;
		if (mfVerticalAngle <= 0.0f)  mfVerticalAngle = 0.0f;

		// チュートリアルイベント発火用 (どちらかの速度が0でなければ入力ありとみなす)
		if (mfCurrentSpeedH != 0.0f || mfCurrentSpeedV != 0.0f)
		{
			TutorialManager::GetInstance()->Event(TutorialManager::Event_Camera);
		}
	}
	else
	{
		// ロックオン中などは慣性速度をリセットしておく
		mfCurrentSpeedH = 0.0f;
		mfCurrentSpeedV = 0.0f;
	}


	// intをboolに変換して揃えて押されたら絶対に返すようにする　1118
	bool KEY_R_Down = InputManager::CheckDownKey(KEY_INPUT_R) != 0; // R
	bool KEY_R_Shift_Down = InputManager::CheckDownKey(KEY_INPUT_RSHIFT) != 0; // 右シフト追加
	bool PAD_R_Down = InputManager::CheckDownPad(XINPUT_BUTTON_RIGHT_THUMB) != 0; // RST押し込み
	
	if (!IsRockOn() && mbNearDistance) // ロックオンのフラグがたっていなかったら、Rを押したら経てるようにする
	{
		// チュートリアルさん
		if (TutorialManager::GetInstance()->IsAction(TutorialManager::Event_RockOn))
		{
			if ((PAD_R_Down) || (KEY_R_Down) || (KEY_R_Shift_Down))
			{
				// チュートリアルさん
				TutorialManager::GetInstance()->Event(TutorialManager::Event_RockOn);

				VECTOR cameraDir = VSub(mvLookAtPosition, mvPosition); // カメラが向いている方向のベクトルがでる
				VECTOR toEnemy = VSub(mpTargetEnemy->GetPosition(), mvPosition); // 敵への向きのベクトルをつくる

				// 正規化する
				cameraDir = VNorm(cameraDir);
				toEnemy = VNorm(toEnemy);

				float vdot = VDot(cameraDir, toEnemy); // ない席を求めている
				float LookAngle = acosf(vdot) * 180.0f / DX_PI_F; // ない席を角度に変更

				// 左に３０、右に３０の範囲だけを入れるなら、６０にする　1117
				float fov = 42.0f * 2; // 好きな視野角に変更
				if (LookAngle <= fov / 2.0f) // 中央のラインから＋－３０の場所が見えたらONにする
				{
					// 敵は視野内
					SetRockOn(true);


					// ロックオン開始時の角度補正
					VECTOR PlayerPos = mpTarget->GetPosition();
					VECTOR EnemyPos = mpTargetEnemy->GetPosition();
					VECTOR EnemyAngle = VSub(EnemyPos, PlayerPos);

					// 敵の方向を角度に変換する
					float targetX = atan2(-EnemyAngle.x, EnemyAngle.z) * 180.0f / DX_PI_F;
					float targetY = atan2(EnemyAngle.y,
						sqrtf(EnemyAngle.x * EnemyAngle.x + EnemyAngle.z * EnemyAngle.z))
						* 180.0f / DX_PI_F;

					// 角度差を最短方向にする　くるぐるしないように追加
					// ロックオンのときに一回だけ行う 1117
					float newX = targetX - mfHorizontalAngle;
					if (newX > 180.0f) newX -= 360.0f;
					if (newX < -180.0f) newX += 360.0f;
					//mfHorizontalAngle += newX; // カメラ角度を一気に補正

					float newY = targetY - mfVerticalAngle;
					if (newY > 180.0f) newY -= 360.0f;
					if (newY < -180.0f) newY += 360.0f;

				}
				else
				{
					SetRockOn(false);
				}
			}
		}
		
	}
	else // たっているときにRが押されたら、フラグを下げる
	{
		if ((PAD_R_Down) || (KEY_R_Down) || (KEY_R_Shift_Down))
		{
			// チュートリアルさん
			TutorialManager::GetInstance()->Event(TutorialManager::Event_RockOff);

			SetRockOn(false); // ロックオンフラグをおろす

			// 視点で違和感をなくす 1117
			{
				if (mfVerticalAngle > 80.0f)
				{
					mfVerticalAngle = 80.0f;
				}
				if (mfVerticalAngle < 0.0f)
				{
					mfVerticalAngle = 0.0f;
				}
			}
		}
	}

	
	
	// ロックオンしていたら
	if (IsRockOn())
	{
		VECTOR PlayerPos = mpTarget->GetPosition(); // プレイヤーの位置をとる
		VECTOR EnemyPos = VGet(mpTargetEnemy->GetPosition().x, mpTargetEnemy->GetPosition().y,
			mpTargetEnemy->GetPosition().z); // 敵の位置を取る
		VECTOR EnemyAngle = VSub(EnemyPos, PlayerPos); //敵との方向をとる ★あとでリストにする

		
		// ラジアンを度数にしてロックオン中の水平方向カメラの処理
		float targetX = atan2(-EnemyAngle.x, EnemyAngle.z) * 180.0f / DX_PI_F;
		float HorizonDistance =sqrtf(EnemyAngle.x * EnemyAngle.x + EnemyAngle.z * EnemyAngle.z);
		float targetY = atan2(EnemyAngle.y, HorizonDistance) * 180.0f / DX_PI_F;


		// 水平方向のアングルが負の数でも、正の数に戻す 0から360の範囲にしている
		// fmodef...AからBを引いたあまり   ぐるぐるなくなった　1118
		mfHorizontalAngle = fmodf(mfHorizontalAngle + 360.0f, 360.0f);
		// 差分計算 ここでぐるぐるなっちゃう
		mfAngleX = targetX - mfHorizontalAngle;
		mfAngleY = targetY - mfVerticalAngle;

		// ここを飛ばすと、180から-180とかに急になっちゃって視点が終わる 1117
		if (mfAngleX > 180.0f) mfAngleX -= 360.0f;
		if (mfAngleX < -180.0f) mfAngleX += 360.0f;
		if (mfAngleY > 180.0f) mfAngleY -= 360.0f;
		if (mfAngleY < -180.0f) mfAngleY += 360.0f;

		// 角度差が小さくなったら固定する
		if (fabs(mfAngleX) < 0.1f && fabs(mfAngleY) < 0.1f)
		{
			// ほぼ一致していたら完全に固定する
			mfHorizontalAngle = targetX;
			mfVerticalAngle = targetY;
		}
		else
		{
			// まだ差があるときは、少しずつ寄せる 20回にわけている
			mfHorizontalAngle += mfAngleX / 20.0f;
			mfVerticalAngle += mfAngleY / 20.0f;
		}

		//// 垂直角度制限
		if (mfVerticalAngle > 80.0f) mfVerticalAngle = 80.0f;
	    if (mfVerticalAngle < 0.0f)  mfVerticalAngle = 0.0f;
	}

}

void Camera::DrawCameraUI()
{
	DrawBill();
	DrawNearMark();
}

void Camera::DrawBill()
{
	// ロックオンしていたら
	if (IsRockOn())
	{
		VECTOR PlayerPos = mpTarget->GetPosition(); // プレイヤーの位置をとる
		VECTOR EnemyPos = VGet(mpTargetEnemy->GetPosition().x, mpTargetEnemy->GetPosition().y,
			mpTargetEnemy->GetPosition().z); // 敵の位置を取る
		VECTOR EnemyAngle = VSub(EnemyPos, PlayerPos); //敵との方向をとる ★あとでリストにする

		// ロックオンした敵に画像を出す処理
		if (mbRocking)
		{
			VECTOR FilePos = VGet(mpTargetEnemy->GetPosition().x,
				mpTargetEnemy->GetPosition().y + mfUp,
				mpTargetEnemy->GetPosition().z);
			// ビルボードでなんか出す　あとあとやる
			DrawBillboard3D(FilePos, 0.5f, 0.5f, 220.0f, 0.0f, mnRockOnHandle, TRUE);
		}
		else if (mbFaring)
		{
			VECTOR FilePos = VGet(mpTargetEnemy->GetPosition().x,
				mpTargetEnemy->GetPosition().y + mfUp,
				mpTargetEnemy->GetPosition().z);
			// ビルボードでなんか出す　あとあとやる
			DrawBillboard3D(FilePos, 0.5f, 0.5f, 220.0f, 0.0f, mnFarOnHandle, TRUE);
		}
		else if (!mbRocking && !mbFaring)
		{
			VECTOR FilePos = VGet(mpTargetEnemy->GetPosition().x,
				mpTargetEnemy->GetPosition().y + mfUp,
				mpTargetEnemy->GetPosition().z);
			// ビルボードでなんか出す　あとあとやる
			DrawBillboard3D(FilePos, 0.5f, 0.5f, 220.0f, 0.0f, mnRockFarHandle, TRUE);
		}
	}
}

void Camera::Distance()
{
	if (!mpTargetEnemy || !mpTarget) return; // 敵がいなければ処理しない
	// ロックオンする敵が一定距離にいるなら
	VECTOR PlayerPos = mpTarget->GetPosition();
	VECTOR EnemyPos = mpTargetEnemy->GetPosition();
	VECTOR toPlayer = VSub(PlayerPos, EnemyPos); // ここで方向ベクトル

	float distance = VSize(toPlayer); // プレイヤーとの距離を入れている

	// ロックオンしている敵のだから個別に見ている
	if (distance <= 1000)
	{
		mbRocking = true;
		mbFaring = false;
		mbNearDistance = true; // 近いからtrue
	}
	else if (distance > 1000 && distance <= 2000)
	{
		mbFaring = true;
		mbRocking = false;
		mbNearDistance = true; // 近いからtrue
	}
	else
	{
		mbRocking = false;
		mbFaring = false; // ここ書くと、離れたときにロックオンがきれちゃう
		mbNearDistance = false; // 遠いからfalse
	}
}

// ゲームを何週かしたときにカメラの位置が前のままになるのを防ぐため
// Scene3D.cppの初期化で呼んでいる
void Camera::Reset()
{
	// 座標と角度を初期化
	mvPosition = VGet(0.0f, 0.0f, 100.0f);
	mvLookAtPosition = VGet(0.0f, 0.0f, 0.0f);
	mfHorizontalAngle = 0.0f;
	mfVerticalAngle = 0.0f;
	mpTarget = nullptr;
	mpTargetEnemy = nullptr;
	SetRockOn(false); // ロックオンフラグをおろす

	// 画面揺れのリセットも呼ぶ
	mfShakeTime = 0.0f;
	mfShakeTimeCounter = 0.0f;
	mfShakeWidth = 0.0f;
	mfShakeAngle = 0.0f;
	mvShakePosition = VGet(0.0f, 0.0f, 0.0f);

	// キラーをリセット
	mbKillerMode = false;
	mpKillerTarget = nullptr;
	mfKillerTimer = 0.0f;
	mbCountdown = false;   // カメラをｗ動かせるように

	// カメラを反映
	SetCameraPositionAndTarget_UpVecY(mvPosition, mvLookAtPosition);
}

// ロックオンしている敵を返す為　1125
Enemy3D* Camera::GetTargetEnemy() const
{
	// mpTargetEnemyがEnemy3Dならそのまま返す
	Enemy3D* RockTargetEnemy = dynamic_cast<Enemy3D*>(mpTargetEnemy);
	return RockTargetEnemy;
}

void Camera::ActivateKillerCamera(Object3D* killer)
{
	mbKillerMode = true;
	mpKillerTarget = killer;

	// 角度を固定
	mfVerticalAngle = 30.0f;

	mbCountdown = true; // カメラを固定
}