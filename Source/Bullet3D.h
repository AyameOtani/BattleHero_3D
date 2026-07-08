#pragma once
#include "Object3D.h"
#include "WeaponManager.h"
#include <string>

class Model;
class Enemy3D;
class Player3D;
class Unit;

class Bullet3D : public Object3D
{
public:
	// 敵の情報も受取るため追加
	Bullet3D(
		Object3D::Tag3D shotManTag, // 誰が撃ったかのタグ
		Unit* owner,                // 誰が打ったか特定  追加
		Object3D* target,           // ホーミングするターゲット
		VECTOR initPos,             // 位置
		std::string filename,       // モデル名前　弾のやつ
		VECTOR Direction,           // 角度　ホーミングのため
		float attack,                // 攻撃力をいれるため
		std::string effect           // 武器を見てエフェクトやってる
	);


	~Bullet3D();

	void Update();
	void Draw();
	void Move();

	void HitWall(); // 壁とのあたり判定
	void HitBaria(); // バリアと当たったら消す関数

	void HitEnemy(); // 敵とのあたり判定
	void HitPlayer();// プレイヤーとのあたり判定

	void CalcDirection(); // ホーミング弾の為に追加　1125

private:

	VECTOR mvDirection; // 向き
	// 現在の目標角度
	float mfAngle = 0.0f;

	float mfMoveSpeed = 0.0f; // 動いた量で消すため
	float mfSpeed = 20.0f; // 速さ

	Model* mpModel;

private: // ロックオンした敵のため

	Object3D::Tag3D mtShotMon; // だれが撃ったかをタグで見分ける
	Object3D* mpTarget = nullptr; // ターゲット
	Unit* mpOwner;               // 誰が撃ったかをポインタで持つ


	bool mbIsHoming; // 弾が追尾するかのフラグ
	const float MOVE_DISTANCE = 2000.0f; // 動いた距離
	float mEffectDistance = 0.0f; // 毎フレームだすと重いからこのタイマーで制御
	float mfAttack; // 攻撃力を保存するやつ

	// 敵からのロックオンはカメラ追尾なし。
    // ランダムでロックオンするかを決める　それか、プレイヤーのYが0.0じゃなかったらロックオン？
	// 一人からのロックオン　弾のコンストラクタの追加
	// ロックオンされていたら画面を変える

	std::string msEffectFile = ""; // エフェクトの画像

};