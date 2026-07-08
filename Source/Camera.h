#pragma once
#include "DxLib.h"
#include <vector>

class Object3D;
class Enemy3D;

class Camera
{
public:
	Camera();
	~Camera();

	void Initialize();      // 初期化
	void Update();          // 更新
	void UpdateRotation(); // 回転処理
	void Distance(); // プレイヤーと敵の距離を求めるために追加
	void Reset(); // シーン変えがあったときにカメラ位置もリセットするため

	void DrawNearMark(); // 近いときに表示するため　updateから移動

	VECTOR GetPosition() { return mvPosition; } // 座標の取得
	VECTOR GetLookAtPosition() { return mvLookAtPosition; }// 注視点取得

	// ロックオンカメラ　のゲッターセッター
	bool IsRockOn() { return mbRockOn; } // ロックオンしているか
	void SetRockOn(bool look) { mbRockOn = look; } // ロックオンをセットするか

	
	// ロックオン中の敵を返すゲッター1125
	Enemy3D* GetTargetEnemy() const;
	// カメラが見ている方向を返す
	float GetHorizonAngle() const { return mfHorizontalAngle; }

	// カウントダウンのゲッターセッター
	bool GetCountdown() const { return mbCountdown; }
	void SetCountdown(bool count) { mbCountdown = count; } // Scene3Dでセット

	void DrawBill(); // ビルボード系の呼び出し
	void DrawCameraUI(); // ビルボーとNearの呼び出し


	// ★New★
    // 画面揺れ
	void Shake();
	void SetupShake(float time, float width, float angleSpeed, float stepTime = 1.0f);
	VECTOR GetShakePosition() { return mvShakePosition; } // 揺れのゲッター


	// プレイヤーが死んだときの演出のやつ
	void ActivateKillerCamera(Object3D* killer);
	

private:
	float mfHorizontalAngle;  // 水平方向アングル  ここでもカメラの座標が変わる（縦座標はそのまま）
	float mfVerticalAngle;    // 垂直方向アングル  ここでもカメラの座標が変わる（横方向はそのまま）

	VECTOR mvPosition;        // カメラ座標
	VECTOR mvLookAtPosition;  // カメラの注視点座標
	Object3D* mpTarget; // カメラを向ける対象
	Object3D* mpTargetEnemy; // カメラを向ける対象 追加

	// ビルボード用
	int mnRockOnHandle; // ロックオンしている敵に対して出す　赤で銃がほぼ確実にあたる
	int mnFarOnHandle;  // ロックオンしている敵に対して出す　緑で銃は運　当たる確率下がる
	int mnLookOnHandle; // ロックオン出来る敵に対して、水色出す
	int mnRockFarHandle; // ロックオンしているが2000以上離れている

	// 追加　ロックオン用
	bool mbRockOn = false; // ロックオンしているか
	float mfAngleX = 0.0f;
	float mfAngleY = 0.0f;

	// 距離関係のフラグ 画像出すため
	bool mbRocking = false; // distanceが1000以内ならのフラグ
	bool mbFaring = false; //distanceが2000以上　かつ　1000以内なら
	bool mbNearDistance = false; // 近い距離にいるときだけ、ロックオンできるように

	// カウントダウンに使うメンバ
	bool mbCountdown = false;

	// 追記　0115 多分使わん
	// std::vector<Object3D*> mpTargetEnemies; // 敵をvectorにして、どの敵でも出るように


	// 画面振れ
	int mnShakeTime;
	int mnShakeTimeCount;

	float mfShakeAngle;
	float mfShakeTimeCounter;
	float mfShakeTime;
	float mfShakeWidth;
	float mfShakeAngleSpeed;
	float mfStepTime;
	VECTOR mvShakePosition;


	// 追加したいメンバ変数（private/protected）
	Object3D* mpKillerTarget = nullptr; // プレイヤーを倒した敵を保持する
	bool mbKillerMode = false;          // キラーカメラ発動中か
	float mfKillerTimer = 0.0f;         // ズーム演出用タイマー

	float mfUp = 0.0f; // 敵の頭上に表示するやつ

private:
	VECTOR mvKillerLastPos; // ターゲットが消えた後も映し続けるための座標
	float mfCameraSpeed = 4.0; // カメラが動く速さ

private:
	float mfCurrentSpeedH = 0.0f; // 現在の水平回転速度
	float mfCurrentSpeedV = 0.0f; // 現在の垂直回転速度

	const float MAX_SPEED = 5.0f;     // 最大速度
	const float ADD_SPEED = 0.22f;   // 加速度
	const float SUB_SPEED = 0.22f;  // 減速速度
};



//------------------------------------------//
//
//        
//
//           カメラ
//- - - - ( VECTOR mvPosition ) - - - - - - -水平方向アングル (点線の軌道)
//
//
//                     物体
//        ( VECTOR mvLookAtPosition )
// 
//------------------------------------------//
//
//                    カメラ
//            ( VECTOR mvPosition )
//                      |
//                                    垂直方向アングル (点線の軌道)
//                      |
//
//                     物体
//        ( VECTOR mvLookAtPosition )
//------------------------------------------//
//
//            カメラ
//     ( VECTOR mvPosition )
//
//
//
//
//                     物体
//        ( VECTOR mvLookAtPosition )

