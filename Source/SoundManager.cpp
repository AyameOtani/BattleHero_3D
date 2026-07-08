#include "SoundManager.h"
#include "DxLib.h"
#include "Master.h"

SoundManager::SoundManager()
	: mnNowPlayingBgm((SOUND_BGM)-1)  // 初期は何も再生されていない状態
	, mnNowPlayingSe((SOUND_SE)-1)    // 初期は何も再生されていない状態
{


}

SoundManager::~SoundManager()
{

}

void SoundManager::Initialize()
{
	// BGM の読み込み
	LoadBGM(SOUND_BGM::BGM_TITLE, "Resource/bgm/bgm_title.mp3");					// タイトル用のBGM
	LoadBGM(SOUND_BGM::BGM_SELECT, "Resource/bgm/bgm_select.mp3");					// 選択中
	LoadBGM(SOUND_BGM::BGM_GAME, "Resource/bgm/bgm_battle_random_01.mp3");			// ゲーム用のBGM
	LoadBGM(SOUND_BGM::BGM_GAME_SUB, "Resource/bgm/bgm_battle_random_02.mp3");		// ゲーム用のBGMサブ
	LoadBGM(SOUND_BGM::BGM_RESULT, "Resource/bgm/bgm_result.mp3");					// リザルト用のBGM
	LoadBGM(SOUND_BGM::BGM_TUTORIAL, "Resource/bgm/bgm_tutorial.mp3");					// チュートリアル用のBGM


	// SE の読み込み
	LoadSE(SOUND_SE::SE_SELECT, "Resource/se/se_select.mp3");               // 選択音
	LoadSE(SOUND_SE::SE_ROCK, "Resource/se/se_lockon.mp3");                 // ロックオン
	LoadSE(SOUND_SE::SE_DEATH, "Resource/se/se_death_explosion.mp3");       // 爆発 死亡
	LoadSE(SOUND_SE::SE_BARIA, "Resource/se/se_baria_on.mp3");              // バリア貼る
	LoadSE(SOUND_SE::SE_BARIA_BREAK, "Resource/se/se_baria_break.mp3");     // バリア終わり
	LoadSE(SOUND_SE::SE_BARIA_HIT, "Resource/se/se_baria_hit.mp3");         // バリア当たる
	LoadSE(SOUND_SE::SE_ATTACK, "Resource/se/se_attack_melee.mp3");         // 近接攻撃
	LoadSE(SOUND_SE::SE_WEAPON, "Resource/se/se_weapon_change.mp3");        // 武器切り替え
	LoadSE(SOUND_SE::SE_REROAD, "Resource/se/se_weapon_reload.mp3");        // リロード
	LoadSE(SOUND_SE::SE_DAMAGE, "Resource/se/se_damage_gun.mp3");           // ダメージ 銃
	LoadSE(SOUND_SE::SE_TIMER, "Resource/se/se_timer_count.mp3");           // タイマーカウント
	LoadSE(SOUND_SE::SE_BOOST, "Resource/se/se_boost.mp3");                 // ブースト
	LoadSE(SOUND_SE::SE_WEAPON_SELECT, "Resource/se/se_weapon_select.mp3"); // 武器選択
	LoadSE(SOUND_SE::SE_BACKSPACE, "Resource/se/se_ui_back.mp3");           // 戻る音
	LoadSE(SOUND_SE::SE_ASIOTO, "Resource/se/se_footstep_normal.mp3");      // 足音
	LoadSE(SOUND_SE::SE_LOAD, "Resource/se/se_load.mp3");                   // ロード音
	LoadSE(SOUND_SE::SE_ENTER, "Resource/se/se_decide_enter.mp3");          // 決定音ENTER
	LoadSE(SOUND_SE::SE_CLEAR, "Resource/se/se_stage_clear.mp3");           // クリア
	LoadSE(SOUND_SE::SE_ALL_CLEAR, "Resource/se/se_all_clear.mp3");         // 全部クリア
	LoadSE(SOUND_SE::SE_SELECT_OK, "Resource/se/se_go.mp3");                // GOの時
	LoadSE(SOUND_SE::SE_PUNCH, "Resource/se/se_damage_punch.mp3");          // 近接ダメージ
	LoadSE(SOUND_SE::SE_WALL_HIT, "Resource/se/se_bullet_hit_wall.mp3");    // 弾が壁に当たった時
	LoadSE(SOUND_SE::SE_PAN, "Resource/se/se_gun_fire.mp3");                // 銃の発射
	LoadSE(SOUND_SE::SE_NOIZE, "Resource/se/se_noise_white.mp3");           // ホワイトイズ
	LoadSE(SOUND_SE::SE_ASIOTO2, "Resource/se/se_footstep_metal.mp3");      // 足音建物の上
	LoadSE(SOUND_SE::SE_GAMWSET, "Resource/se/se_result_gameset.mp3");      // スコア表示
	LoadSE(SOUND_SE::SE_GUITER, "Resource/se/se_guitar.mp3");               // ギターのSE
	LoadSE(SOUND_SE::SE_OPEN, "Resource/se/se_ui_open.mp3");                // 表示するとき
	LoadSE(SOUND_SE::SE_WIND, "Resource/se/se_wind.mp3");                   // 走っている風
	LoadSE(SOUND_SE::SE_OH, "Resource/se/se_voice_oh.mp3");                 // オーバーヒート
	LoadSE(SOUND_SE::SE_DOOR_OPEN, "Resource/se/se_door_open.mp3");         // ドア開けるほう
	LoadSE(SOUND_SE::SE_DOOR_CLOSE, "Resource/se/se_door_close.mp3");       // ドア閉まるほう
}


void SoundManager::Finalize()
{
	// BGMの破棄
	for (auto it = mnBgmHandleList.begin(); it != mnBgmHandleList.end(); it++)
	{
		DeleteSoundMem(it->second);
	}

	// SEの破棄
	for (auto it = mnSeHandleList.begin(); it != mnSeHandleList.end(); it++)
	{
		DeleteSoundMem(it->second);
	}
}


void SoundManager::PlayBGM(SOUND_BGM bgm, bool isTop, int volume)
{
	// 現在再生されているBGMと同じかつ
	// 最初からの再生ではないのであれば return する
	if (mnNowPlayingBgm == bgm && !isTop)
	{
		return;
	}

	//  現在再生中のBGMを止める
	// 今流れているBGMを探して停メル
	for (auto it = mnBgmHandleList.begin(); it != mnBgmHandleList.end(); it++)
	{
		if (it->first == mnNowPlayingBgm)
		{
			if (CheckSoundMem(it->second))
			{
				StopSoundMem(it->second);
			}
			break;
		}
	}

	// 新しいBGMのハンドルを取得して再生
	for (auto it = mnBgmHandleList.begin(); it != mnBgmHandleList.end(); it++)
	{
		if (it->first == bgm)
		{
			// 再生する前に音量を設定する
			ChangeVolumeSoundMem(volume, it->second);
			// BGMをループ再生
			PlaySoundMem(it->second, DX_PLAYTYPE_LOOP, TRUE); // ループ再生
			// 現在の再生種類を更新
			mnNowPlayingBgm = bgm;
		}
	}
}


// 音量変えたい
void SoundManager::ChangeVolume(int volume)
{
	// 何も再生していないなら何もしない
	if (mnNowPlayingBgm == BGM_NONE) return;

	// すでに再生中のBGMがあるか確認
	int currentHandle = -1;
	for (auto it = mnBgmHandleList.begin(); it != mnBgmHandleList.end(); it++)
	{
		if (it->first == mnNowPlayingBgm)
		{
			currentHandle = it->second;
			break;
		}
	}

	// 見つかったら音量を変更
	if (currentHandle != -1)
	{
		ChangeVolumeSoundMem(volume, currentHandle);
	}
}

// 音量変えたいSE追加した
void SoundManager::Change_SE_Volume(SOUND_SE se, int volume)
{
	// 指定されたSE探し
	int targetHandle = -1;
	for (auto it = mnSeHandleList.begin(); it != mnSeHandleList.end(); it++)
	{
		if (it->first == se)
		{
			targetHandle = it->second;
			break;
		}
	}

	// 見つかったらその SE の音量だけを変更
	if (targetHandle != -1)
	{
		ChangeVolumeSoundMem(volume, targetHandle);
	}
}


// 指定したSEを個別に止める関数
void SoundManager::StopSE(SOUND_SE se)
{
	// SEのリストから 止めたいSEを探す
	for (auto it = mnSeHandleList.begin(); it != mnSeHandleList.end(); it++)
	{
		if (it->first == se)
		{
			// 再生中かどうか確認して止める
			if (CheckSoundMem(it->second))
			{
				StopSoundMem(it->second);
			}
			break;
		}
	}
}


void SoundManager::PlaySE(SOUND_SE se, int volume)
{
	for (auto it = mnSeHandleList.begin(); it != mnSeHandleList.end(); it++)
	{
		// 一致した種類のSEがあれば
		if (it->first == se)
		{
			// 再生前に音量を設定する
			ChangeVolumeSoundMem(volume, it->second);
			// SEをループ再生
			PlaySoundMem(it->second, DX_PLAYTYPE_BACK); // バックグラウンド再生する
			// 現在の再生種類を更新
			mnNowPlayingSe = se;
		}
	}
}

void SoundManager::LoadBGM(SOUND_BGM bgm, std::string filename)
{
	bool check = false; // 重複して読み込んでいるかどうか
	for (auto it = mnBgmHandleList.begin(); it != mnBgmHandleList.end(); it++)
	{
		if (it->first == bgm)
		{
			check = true;
			break;
		}
	}

	// 重複して読み込まれていたら何もしない
	if (check)
	{
		return;
	}

	// ファイル読み込み
	int handle = LoadSoundMem(filename.c_str());
	if (handle == -1) // -1だと失敗
	{
		return;   // 読み込み失敗したら何もしない
	}

	// 読み込んだハンドルをリストに追加
	// 読み込ん札ハンドルと、タイプ（種類）を一緒に追加
	// std::pair の一個めの変数（種類）がfirst とされている
	// pair の一つ目の要素を表していた
	// strust
	// {
	//  SOUND_BGM bgm;
	//  int handle;
	// }
	// が、std::pair<SOUND_BGM, int>

	mnBgmHandleList.push_back(std::pair<SOUND_BGM, int>(bgm, handle));
}


void SoundManager::LoadSE(SOUND_SE se, std::string filename)
{
	bool check = false; // 重複して読み込んでいるかどうか
	for (auto it = mnSeHandleList.begin(); it != mnSeHandleList.end(); it++)
	{
		if (it->first == se)
		{
			check = true;
			break;
		}
	}

	// 重複して読み込まれていたら何もしない
	if (check)
	{
		return;
	}

	// ファイル読み込み
	int handle = LoadSoundMem(filename.c_str());
	if (handle == -1) // -1だと失敗
	{
		return;   // 読み込み失敗したら何もしない
	}

	mnSeHandleList.push_back(std::pair<SOUND_SE, int>(se, handle));
}


void SoundManager::StopBGM()
{
	for (auto it = mnBgmHandleList.begin(); it != mnBgmHandleList.end(); it++)
	{
		// 今現在再生しているBGMと比較させる
		if (it->first == mnNowPlayingBgm)
		{
			// BGMが再生されているかのif文
			//          もし再生されていたら
			if (CheckSoundMem(it->second))
			{
				StopSoundMem(it->second); // BGMの停止
				break;
			}
		}
	}

	// チュートリアルから戻ったときにBGMが再生されないから
	 mnNowPlayingBgm = BGM_NONE;
}

void SoundManager::PlaySE_Loop(SOUND_SE se, int volume)
{
	for (auto it = mnSeHandleList.begin(); it != mnSeHandleList.end(); it++)
	{
		if (it->first == se)
		{
			// すでに鳴っていたら二重に鳴らさない
			if (CheckSoundMem(it->second))
			{
				return;
			}

			// ボリューム
			ChangeVolumeSoundMem(volume, it->second);
			PlaySoundMem(it->second, DX_PLAYTYPE_LOOP);
			break;
		}
	}
}