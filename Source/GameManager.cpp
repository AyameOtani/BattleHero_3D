#include "GameManager.h"

// シングルトンの実体
GameManager* GameManager::instance = nullptr;

// 選択中の武器データの実体
GameManager::SelectWeaponData GameManager::WeaponSelect;

// 敵の武器の実体
GameManager::EnemyWeaponDate GameManager::EnemyWeapon;

// 時間の実体
int GameManager::mnLastTime;
// プレイヤーのHPの実態
int GameManager::mnLastPlayerHp;
// 敵のカウント
int GameManager::mnKillEnemyCount;

// 選択番号を保存するやつ
int GameManager::mnSelectPlayerDate;

// ロードの実体
GameManager::ResourceData GameManager::Resources;

// プレイヤーのやつ
GameManager::PlayerData GameManager::Players;
