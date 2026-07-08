#pragma once
#include "DxLib.h"
#include "Texture.h"
#include "Bullet.h"
#include <string>
#include "GameScene.h"

// クラスの前方宣言
// include せずに存在だけを知らせる宣言
// Enemyクラスの中のメンバ関数などは呼べない
class Enemy;
class Boss;

class HomingBullet : public Bullet // 継承
{
public:

	HomingBullet(Enemy* enemy, Boss* boss, VECTOR initPos, std::string filename);
	
	virtual ~HomingBullet();

	virtual void Update() override;

	virtual void Draw() override;

	// 移動処理
	void Move();


	// 方向計算
	void CalcDirection();


private:
	Enemy* mpEnemy; // 敵情報
	Boss* mpBoss; // ボスの情報

};
