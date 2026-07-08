#pragma once

class  TutorialManager
{
public:
	// 今どのチュートリアルをやっているか1
	enum TutorialStep
	{
		Move =   0,        // 移動とカメラ
		Jump =   1,	       // 上昇　ジャンプ
		Attack = 2,        // 通常攻撃
		Weapon = 3,        // 銃の持ち替え 銃攻撃
		RockOn = 4,        // ロックオン
		Baria =  5,        // バリア

		End, // しゅうりょう
	};

	// プレイヤーとかから通知する
	enum EventType
	{
		Event_Move =   0,      // 移動
		Event_Camera = 1,      // カメラ
		Event_Jump =   2,	   // 上昇
		Event_JumpUp = 3,	   // ジャンプ
		Event_Attack = 4,	   // 通常攻撃
		Event_AttackOff = 5,	   // 通常攻撃
		Event_Bullet = 6,      // 銃攻撃
		Event_Weapon = 7,      // 銃の持ち替え
		Event_RockOn = 8,      // ロックオン
		Event_RockOnGraph = 9,      // ロックオン画像が出たか
		Event_RockOff= 10,      // ロックオン解除
		Event_RockOffGraph = 11,      // ロックオン画像が消えたか
		Event_Baria = 12,      // バリア
		Event_BariaOff =13,      // バリア破壊


		Event_Skip
	};

	 // 明日のうちへ
	// プレイヤーの行動によってのカウントとかもろもｒやつ
	struct TutorialData
	{
		// 移動カメラ
		int moveCount = 0; // 移動してたら＋＋+
		int cameraCount = 0; // 動いていたら＋＋

		// 上昇　ジャンプ
		int jumpCount = 0; // 単押しさせてCheckHitDownしたら取りたい
		int jumpUpCount = 0; // moveと同じで＋＋したい　上昇

		// 攻撃　銃攻撃
		int attackCount = 0;    // 何回攻撃したか
		bool mbAttack = false; // 攻撃あにめ終わったかどうか

		// ロックオン
		int rockOn = 0; // ロックオンしたら１外されたら２とかにする
		int rockOff = 0; // ロックオンしたら１外されたら２とかにする
		bool isRockOn = false;  // ロックオン画像がでたら
		bool isRockOff = false; // ロックオフ画像がでたら

		// 銃の持ち替え
		int weaponSelect = 0; // 武器が変ったら＋１で５改ぐらいやりたい
		int bulletAttack = 0;   // 銃の攻撃


		// バリア
		int baria = 0; // 貼られたら
		bool bariaBreak = false; // 貼られていたらtrue割れてたらfals


		// ステップとフラグとか
		TutorialStep step = Move;   // 今のステップ
		bool isComplete = false;// チュートリアル中かのフラグ
		int stepCount = 1; // カウントするやつ STEPで出す

		int nextStep = 0; // 次のステップにいくためのやつ
	};

	// プレイヤーがやったこととかのやつ　攻撃３かいしたかとかのミッション
	void Event(EventType type);
	// データ取得する関数
	TutorialData GetDate() const { return data;}

	void DrawEvent(); // イベントごとに表示変えるやつ

	bool IsAction(EventType type) const;  // 許可するかのやつ

	void Reset(); // リセットするやつ

	void StringInitialize(); // 文字セット
	void StringFinalize(); // 文字deleteするやつ

	void CountUpdate(); // カウントをUPDATEするやつ


	// シングルトン インスタンスがひとつだけ存在することを保証するやつ
	// コンストラクタを非公開にすることで外部からの作成を防ぐ
	static TutorialManager* GetInstance() // GetInstanceで名前は固定
	{
		// 一回目はnew 二回目以降はreturnで帰る
		if (instance == nullptr)
		{
			instance = new TutorialManager(); // 初めて作られる場合はnewで作られる
		}
		return instance; // 二回目以降はそのまま値が帰る
	}

private:
	TutorialManager() {}; // コンストラクタを非公開にする
	static TutorialManager* instance;

	TutorialData data;

	int mnFontNormal = -1;
	int mnFontBig = -1;

	int nowStep = 1; // ステップの保存	 １から始める

	const int mnCountDownStep = 60; // ステップクリアしたら待つやつ
};


// チュートリアルさん
//TutorialManager::GetInstance()->Event(TutorialManager::Event_Move);