#pragma once
#include <string>

class Status3D
{
public:
	// プレイヤーのデータ　名前とかいろいろ
	struct PlayerDate
	{
		std::string name; // 名前
		float hp;
		float boost;
		float speed;
		float maxSpeed;
		std::string memo;
	};
	// IDを渡すとスペックを返す関数
	// modelを二重ロードしないためにつくた
	static PlayerDate GetPlayerSpec(int select);



	Status3D(const PlayerDate& spec); // ここの引数にBOostとか増やす予定 ★
	~Status3D();

	// --HP関係--------------------------------------------
	// 最大HPのゲッターセッター
	float Get_Base_MaxHp() const { return mfMaxHp; }
	void Set_Base_MaxHp(float maxhp) { mfMaxHp = maxhp; }
	// Hpのゲッターセッター
	float Get_Base_Hp() const { return mfHp; }


	// --BOOST関係-----------------------------------------
	// 最大BOOSTのゲッターセッター
	float Get_Base_MaxBoost() const { return mfMaxBoost; }
	void Set_Base_MaxBoost(float maxboost) { mfMaxBoost = maxboost; }
	// Boostのゲッター
	float Get_Base_Boost() const { return mfBoost; }

	// --スピード関係--------------------------------------
	float Get_Base_Speed() const { return mfSpeed; }
	void Set_Base_Speed(float speed) { mfSpeed = speed; }

	// --最大速度関係--------------------------------------
	float Get_Base_MaxSpeed() const { return mfMaxSpeed; }
	void Set_Base_Max_Speed(float max) { mfMaxSpeed = max; }


	// 名前のゲッター 武器選択とゲームで使う
	std::string GetName() const { return msSelectPlayer; }
	


	// ダメージ / 回復(デバックのみ)関係
	void ApplyDamage(float applyDamage);
	void ApplyHeal();

	// Boostの増減
	void ApplyAddBoost(float applyAddBoost);
	void ApplySubBoost(float applySubBoost);

	// 最大速度を超えないように
	void Limit_Base_Speed();

private:
	// Hp関係
	float mfMaxHp;
	float mfHp;

	// Boost関係
	float mfBoost;
	float mfMaxBoost;

	// スピード関係
	float mfSpeed;
	float mfMaxSpeed;

	// プレイヤー番号を保存するメンバ
	std::string msSelectPlayer;

	// 選択画面で決めた HP を保持する静的変数
	//static float msSelectedHp;
};