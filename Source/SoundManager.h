#pragma once
#include <string>
#include <vector>

class SoundManager
{
public:   // enum, struct などの定義

	// enumで番号をふっている
	enum SOUND_BGM  // BGMの種類
	{
		BGM_NONE = -1,		// BGMを消す

		BGM_TITLE = 0,		// タイトル用のBGM
		BGM_SELECT = 1,		// 選択シーン
		BGM_GAME = 2,		// ゲーム用のBGM
		BGM_RESULT = 3,     // リザルト用のBGM
		BGM_TUTORIAL = 4,	// チュートリアルシーン
		BGM_GAME_SUB = 5,	// ゲーム音楽２


		// 増やしても良い
	};

	enum SOUND_SE  // SEの種類
	{
		SE_SELECT = 0,          // 選択音
		SE_ROCK = 1,            // ロックオン
		SE_DEATH = 2,           // 爆発 死亡
		SE_BARIA = 3,           // バリア貼る
		SE_BARIA_BREAK = 4,     // バリア終わり
		SE_BARIA_HIT = 5,       // バリア当たる
		SE_ATTACK = 7,          // 近接攻撃
		SE_WEAPON = 8,          // 武器切り替え
		SE_REROAD = 9,          // リロード
		SE_DAMAGE = 10,         // ダメージ 銃
		SE_TIMER = 11,          // タイマーカウント
		SE_BOOST = 12,          // ブースト
		SE_WEAPON_SELECT = 13,  // 武器選択
		SE_BACKSPACE = 14,      // 戻る音
		SE_ASIOTO = 15,         // 足音
		SE_LOAD = 16,           // ロード音
		SE_ENTER = 17,          // 決定音 ENTER
		SE_CLICK = 18,          // クリック音     なし
		SE_CLEAR = 19,          // クリア
		SE_ALL_CLEAR = 20,      // 全部クリア
		SE_SELECT_OK = 21,      // GOの時
		SE_PUNCH = 22,          // 近接ダメージ
		SE_WALL_HIT = 23,       // 弾が壁に当たった時
		SE_PAN = 24,            // 銃の発射
		SE_NOIZE = 25,          // ホワイトノイズ
		SE_ASIOTO2 = 26,        // 足音建物の上
		SE_GAMWSET = 27,        // スコア表示
		SE_GUITER = 28,         // ギターのSE
		SE_OPEN = 29,           // 表示するとき
		SE_WIND = 30,           // 走っている風
		SE_OH = 31,             // オーバーヒート
		SE_DOOR_OPEN = 32,      // ドア開けるほう
		SE_DOOR_CLOSE = 33,     // ドア閉まるほう

	};

public:  // 公開

	SoundManager(); // コンストラクタ

	~SoundManager(); // デストラクタ

	// 初期化
	void Initialize();

	// 終了処理
	void Finalize();


public:   // サウンド再生系  こっちは関数


	// BGM再生
	// SOUND_BGM... 鳴らしたいBGMの種類
	// voli\ume... 大きさ　追加した
	// isTop... 最初から再生するかどうか
	// bool isTop = true -> デフォルト引数と呼ばれる書き方。館数を呼びだす際に渡さなくても認識される。
	void PlayBGM(SOUND_BGM bgm, bool isTop = true, int volume = 255);

	// 音量変えるやつ	BGM
	void ChangeVolume(int volume);
	
	// ループするSE
	void PlaySE_Loop(SOUND_SE se, int volume);
	// 音量変えるやつ	SE
	void Change_SE_Volume(SOUND_SE se, int volume);
	void StopSE(SOUND_SE se); // 止める SE SEというかながめのやつ

	// SE再生
	// SEは短い音なので最初から再生のは入れなくて良い
	// 大きさ追した 0306 マスターまでに間に合うとよいがな
	void PlaySE(SOUND_SE se, int volume = 255);

	// BGMの読み込み
	void LoadBGM(SOUND_BGM bgm, std::string filename);

	// SEの読み込み
	void LoadSE(SOUND_SE se, std::string filename);

	// BGMの停止
	void StopBGM();


private:  // メンバ変数

	SOUND_BGM mnNowPlayingBgm;  // 現在再生されているBGMの種類
	SOUND_SE mnNowPlayingSe;  // 現在再生されているSEの種類


	// pair... 構造体みたいな要素？ 二ついれられるやつ
	// 
	// よみこんだBGMハンドルのリスト
	std::vector<std::pair<SOUND_BGM, int>> mnBgmHandleList;
	// よみこんだSEハンドルのリスト
	std::vector<std::pair<SOUND_SE, int>> mnSeHandleList;

};



