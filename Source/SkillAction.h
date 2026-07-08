#pragma once
// ★New★
// スキルアクションを更新するクラス
// 通常攻撃もこのクラスを介して出来るようになるとなお良い。

#include <vector>

#include "ModelUtility.h"

class Player3D;

struct SkillActionData
{
	std::vector<AnimationState> mStateList;		// 一連のアクションで使用するモーション
	std::vector<int> mnWaitFrameList;			// モーション終了後の待ち時間
	std::vector<float> mfAnimationSpeedList;	// モーションの速度変更をするときの設定
	std::vector<float> mfAnimationChangeRate;	// どのタイミングでモーション遷移するかの設定
};

class SkillAction
{
public:
	SkillAction(Player3D* pUser);
	~SkillAction();

	void Initialize(SkillActionData data);
	bool Update();

	bool IsAction() { return mbIsAction; }

private:
	SkillActionData mData;
	int mnNowDataNum;
	int mnFrameCount;
	bool mbIsAction;

	Player3D* mpActor;
};