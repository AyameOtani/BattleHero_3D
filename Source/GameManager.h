#pragma once
#include "WeaponManager.h"

class GameManager
{
public:

	// --------------武器関係------------------
	// 保存する構造体
	// メイン武器とサブの武器を入れられる プレイや＾－の武器データ
	struct SelectWeaponData
	{
		WeaponManager::WeaponID mainWeapon = WeaponManager::WeaponID::None;
		WeaponManager::WeaponID subWeapon = WeaponManager::WeaponID::None;
		// NONEで初期化した方が良さげ
	};

	// 敵の武器データ
	struct EnemyWeaponDate
	{
		WeaponManager::WeaponID enemyWeapon = WeaponManager::WeaponID::None;
	};

	// いろいろ公開しているやつら
	static SelectWeaponData WeaponSelect; // 武器のデータ
	static EnemyWeaponDate EnemyWeapon;  // 敵の武器 データ

	// ------------ロード読み込みのやつ ----------------
	struct ResourceData
	{
		// ゲームシ－ンで読み込むやつ
		int stageModel = -1;
		int stageCollModel = -1;
		int skyBoxModel = -1;
		int playerModel = -1;
		std::string playerModelPath = ""; // ここでパスを覚えてモデルが変えられたらわかるように
		// タイトルで読み込むやつ
		int titleStage = -1;
	};
	static ResourceData Resources;



   // ----------------リザルドで使うやつら---------------
	static int mnLastTime; // 時間を保存するやつ
	static int mnLastPlayerHp; // プレイヤーのHPの保存
	static int mnKillEnemyCount; // 敵の数
	
	// -------------選択で使う------------------
	static int mnSelectPlayerDate; // 選択画面のmnSelectを代入　リスタートで使う

	// 名前入力
	struct 	PlayerData
	{
		std::string name = "PLAYER"; // 初期名前
		bool isEntryFinished = false; // 入力済みフラグ
	};

	static PlayerData Players;  // 管理なにやつ



	// シングルトン インスタンスがひとつだけ存在することを保証するやつ
	// コンストラクタを非公開にすることで外部からの作成を防ぐ
	static GameManager* GetInstance() // GetInstanceで名前は固定
	{
		// 一回目はnew 二回目以降はreturnで帰る
		if (instance == nullptr)
		{
			instance = new GameManager(); // 初めて作られる場合はnewで作られる
		}
		return instance; // 二回目以降はそのまま値が帰る
	}


private:
	GameManager() {}; // コンストラクタを非公開にする

	static GameManager* instance;
};

