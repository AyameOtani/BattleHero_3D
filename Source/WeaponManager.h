#pragma once
#include "DxLib.h"
#include <string>
#include <map>

// 前方宣言
class AttachmentModel;

class WeaponManager
{
public:
	// 武器を管理するenuｍ
	enum WeaponID
	{
		None = 0, // 空

		Shield = 1,    // シールドSEED
		BlackGun = 2,  // 銃黒
		SEEDGun = 3,   // 銃SEED
		BlueGun = 4,   // 蒼い銃
		YARI = 5,      // やり
		GT = 6,        // ギター

		WeaponMax, // 最大
	};

	// データ 今この武器の攻撃力とか残りの弾数とか保存しとる
	struct WeaponData
	{
		AttachmentModel* model;
		float attack;
		float bulletMax; // 弾追加0202
		float NowBullet; // 今の弾数

		// 武器ごとに見たいから個別に持つべきっぽい
		bool reload; // リロード中か
		float reloadCount; // リロードカウント
	};


	// 武器のスペック
	struct WeaponSpec
	{
		std::string name; // 名前
		std::string modelPath; // パス
		float attack; // 攻撃力
		float bulletMax; // 弾かず最大
		std::string memo; // 一言メモ
	};

	// IDを渡すとスペックを返す関数
	// modelを二重ロードしないためにつくた
	static WeaponSpec GetBaseSpec(WeaponID id);


public:
	// 指定したIDの武器データを返す（nullptrなら存在しない）
	const WeaponData* GetWeaponData(WeaponID id) const
	{
		auto it = mmWeapons.find(id);
		if (it != mmWeapons.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	// コンストラクタ
	WeaponManager(int ModelHandle); // Modelからハンドルをとれるようにする
	~WeaponManager(); // デストラクタ
	//void Update(); // 更新
	//void Draw();   // 描画

	// -------------武器関係---------------------------Modelから移動
	// 武器を登録する プレイヤーのコンストラクタで呼ぶるもり
	void Lord_Base_Weapon(WeaponID id, std::string filename, std::string attachFrameName,
		float attack, float bulletMax, bool isEnemy);
	// 武器を切り替えるための関数 IDでよべるようにした
	void Set_Base_Weapon(WeaponID id);
	// IDを返すゲッター
	WeaponID Get_Base_WeaponID() const { return mnNowWeapon; }
	// 今装備している武器の位置を取得する新しい方式 プレイヤーで使ってる
	VECTOR Get_Base_WeaponPosition();
	// frameの名前を取得するやつ 敵だったら確定でwpにアタッチしたいから改造
	std::string GetFrameName(WeaponID id, bool isEnemy) const;
	// 選択した武器だけロード	敵かも追加
	void SelectLoadWeapon(WeaponID id, bool isEnemy);


	// 武器をモデルと選択した武器ごとに回転を変える関数
	VECTOR RotateWeapon(WeaponID id, bool isEnemy) const;



	// ------------武器ごとに弾数をきめている --------------
	// 攻撃力のゲッター プレイヤーとかから参照する
	float Get_Base_Attack() const { return mfAttack; }
	// 最大弾数のゲッターセッター プレイヤーから参照
	float Get_Base_BulletMax();
	// 今の弾数を返せるやつ 今装備しているやつ
	float Get_Base_Bullet();



	// 追加
	bool GetReload(WeaponID id) const; //リロード中かを個別で返せる　mainとsub
	float GetReloadCount(WeaponID id) const; // 今のリロード時間を返せるやつ
	void AddCount(); // カウント減らすやつ　Playerで呼んでる
	bool GetNowRel(); //今持っている武器がリロード中か同かの取得
	float GetNowCou(); // いま装備している武器のカウントを返す
	// 変えたい武器が同じ武器のときはなにも出さないようにするためのゲッターセッター
	// 同じ武器だったらtrueになる
	bool GetSameWeapon() const { return mbTheSameWeapon; }
	void SetSameWeapoon(bool sm) { mbTheSameWeapon = sm; }



	// 球数を減らせるように
	void SubBullet(int dan);
	// サブもメインも攻撃力とかのステを取得できるやつ
	float Get_Sub_Bullet(WeaponID id) const;
	float Get_Sub_BulletMax(WeaponID id) const;
	float GetMaxRel() const { return MAX_RELOAD; }



	// --------------位置------------------------------------
	// ワールド座標の取得 攻撃判定 まだ敵で使用
	VECTOR GetWorldPosition() const;
	// 武器のふーれむ位置を取得
	VECTOR GetFrameWorldPosition(const std::string& frameName) const;




	// こっちに武器にアタッチするのを付けr手ば良いのではs


private:
	int mnModelHandle;      // モデルが持っていたハンドル
	// 複数武器するため 追加した新年
	// 武器をID モデルの形で保存するため まとめて管理できるのがmapらしい
	std::map<WeaponID, WeaponData> mmWeapons;  // IDでまとめて管理しているmap メンバマップだからMM
	AttachmentModel* mpCurrentWeapon = nullptr; // 今装備している武器のポインタ
	WeaponID mnNowWeapon; // ゲッターで返す用のIDをほぞんする変数 IDを返すように

	// 攻撃保存
	float mfAttack = 0.0f;
	// 弾数保存用
	float mfBulletMax = 0.0f;
	// 同じ武器かのフラグ
	bool mbTheSameWeapon = false;

	static constexpr float MAX_RELOAD = 300; // リロード時間の最大数 ここ変えたい

};


// メモ
// std::map...IDと値をセットで保存できるもの
// 例えば数字とポインタとか、文字列とかもできるから神

