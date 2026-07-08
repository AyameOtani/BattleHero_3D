// ★New★
// スキルアクションデータを管理するクラス
// 一旦分かりやすいようにコードにベタ打ちしているが、
// ファイル読み込みなどで出来るようになるとなお良い。

#include "SkillActionMaster.h"

SkillActionData SkillActionMaster::Sample = {
	// 一連のアクションで使用するモーション
	{
		ANIMATION_ATTACK, ANIMATION_ATTACK3
	},
	// モーション終了後の待ち時間（フレーム数）
	{
		10, 5
	},
	// モーションの速度変更をするときの設定
	{
		1.5f, 1.5f
	},
	// どのタイミングでモーション遷移するかの設定（モーションの進捗率）
	{
		1.0f, 0.0f
	}
};

SkillActionData SkillActionMaster::Sample2 = {
	// 一連のアクションで使用するモーション
	{
		ANIMATION_ATTACK3, ANIMATION_ATTACK3
	},
	// モーション終了後の待ち時間（フレーム数）
	{
		5, 5
	},
	// モーションの速度変更をするときの設定
	{
		1.5f, 1.5f
	},
	// どのタイミングでモーション遷移するかの設定（モーションの進捗率）
	{
		0.5f, 0.0f
	}
};

SkillActionData SkillActionMaster::Sample3 = {
	// 一連のアクションで使用するモーション
	{
		ANIMATION_ATTACK2, ANIMATION_ATTACK
	},
	// モーション終了後の待ち時間（フレーム数）
	{
		0, 3
	},
	// モーションの速度変更をするときの設定
	{
		1.5f, 1.5f
	},
	// どのタイミングでモーション遷移するかの設定（モーションの進捗率）
	{
		0.5f, 0.0f
	}
};