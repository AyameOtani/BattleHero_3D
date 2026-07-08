#pragma once

#include "SceneManager.h"
#include "SoundManager.h"
#include "Camera.h"
#include "Status3D.h"
#include <memory> // unique_ptr を使うため
#include "ResourceManager.h" // 追加




//
// ゲーム内でひとつだけしか存在しないものを管理するクラス
// 例えば、
// SceneManager 　　だったり、（シーン管理クラス）
// SoundManager 　　だったり、（サウンド管理クラス）
// ResourceManager  だったり、（リソース（画像やモデル）管理クラス）
// を、どこからでも参照できるようにしておきたいので
// こういったクラスを作る
// 

class Master
{
public:  // 公開しているから使える　クラスをまたいで使える

	static SoundManager* mpSoundManager;   // サウンドマネージャーのポインタ
	static SceneManager* mpSceneManager;   // シーンマネージャーのポインタ[

	static ResourceManager* mpResourceManager; // リソースマネージャーのポインタ

	// ３Dで使う
	static Camera* mpCamera;

	// --------------3D用-----------------------

	 // HPを設定するため Scene3DでPlayer3Dのnewをしているから選択シーンでの移りがわからんかったのでとりまMasterに渡す
	//static float mfSelectedHp;
	//static Status3D* mpNextStatus; // 選択画面から渡すためのStatus3Dポインタ こｒはStatusの型
	


	// 1231 これに変更   静的のやつ
	// unique_ptr...所有権を一か所に固定するスマートポoインタ
	// 自動でdeleteされる。Masterが所有者として管理する
	static std::unique_ptr<Status3D> mpNextStatus;

	// playerのモデルを変えるためのSetModel型のポインタのメンバ変数
	static std::string msPlayerModelName;
};



// ---------------メモ---------------------------------------------1231-------------------
// unique_ptr ... uniqueが勇逸のという意味  +   ptrはポインタという意味らしい
// 所有者が必ず一つだけのポインタという意味
// メリットは、同じものを複数の場所がdeleteしないから前みたいに二重にならない
// 所有者がMasterに固定される方メモリリークが防げる
// すこーぷを抜けると自動でdeleteだから安全 

// 前は、MasterもUnitもdeleteしていたから持ってるのが２人だたから二重に消えていたからエラー
