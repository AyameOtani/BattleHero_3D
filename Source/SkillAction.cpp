// ★New★
// スキルアクションを更新するクラス
// 通常攻撃もこのクラスを介して出来るようになるとなお良い。

#include "SkillAction.h"
#include "Player3D.h"

SkillAction::SkillAction(Player3D* pActor)
	: mData()
	, mnNowDataNum(0)
	, mnFrameCount(0)
	, mbIsAction(false)
	, mpActor(pActor)
{

}

SkillAction::~SkillAction()
{

}

void SkillAction::Initialize(SkillActionData data)
{
	mData = data;
	mnNowDataNum = 0;
	mnFrameCount = 0;
	mbIsAction = true;

	mpActor->GetModelPointer()->ChangeAnimation(mData.mStateList.front(), true);
	mpActor->GetModelPointer()->SetLoop(false);
	mpActor->GetModelPointer()->SetLoopFinishState(ANIMATION_MAX);
	mpActor->GetModelPointer()->SetAnimationSpeedScale(mData.mfAnimationSpeedList.front());
}

bool SkillAction::Update()
{
	if (!mbIsAction)
	{
		return false;
	}

	if (mData.mfAnimationChangeRate.at(mnNowDataNum) != 0.0f)
	{
		if (mData.mfAnimationChangeRate.at(mnNowDataNum) >= mpActor->GetModelPointer()->GetAnimationProgressRate())
		{
			return true;
		}
	}
	else if (!mpActor->GetModelPointer()->IsAnimationLoopFinish())
	{
		return true;
	}

	mnFrameCount++;
	if (mnFrameCount >= mData.mnWaitFrameList.at(mnNowDataNum))
	{
		mnFrameCount = 0;
		mnNowDataNum++;
		if (mnNowDataNum >= mData.mStateList.size())
		{
			mbIsAction = false;
			mpActor->GetModelPointer()->ChangeAnimation(ANIMATION_NEUTRAL);
			mpActor->GetModelPointer()->SetAnimationBlend(false);

			return false;
		}
		else
		{
			mpActor->GetModelPointer()->ChangeAnimation(mData.mStateList.at(mnNowDataNum), true);
			mpActor->GetModelPointer()->SetAnimationBlend(false);
			mpActor->GetModelPointer()->SetLoop(false);
			mpActor->GetModelPointer()->SetAnimationSpeedScale(mData.mfAnimationSpeedList.at(mnNowDataNum));
			mpActor->GetModelPointer()->SetLoopFinishState(ANIMATION_MAX);
		}
	}

	return true;
}
