#include "PlayerAnim.h"
#include "Player3D.h"

// モデルごとに読み込むアニメーションを変えている
void PlayerAnim::Load(Player3D* player, const std::string& modelPath)
{
    // ゾンビ
    if (modelPath == "Resource/3d/Zombie/Zombie_Mesh.mv1")
    {
        player->AddAnimation(ANIMATION_NEUTRAL, "Resource/3d/Zombie/Ch10_nonPBR@Zombie Idle.mv1");
        player->AddAnimation(ANIMATION_WALKING, "Resource/3d/Zombie/Ch10_nonPBR@Walking.mv1");
        player->AddAnimation(ANIMATION_ATTACK, "Resource/3d/Zombie/Ch10_nonPBR@Zombie Attack.mv1");
        player->AddAnimation(ANIMATION_RUN, "Resource/3d/Zombie/Ch10_nonPBR@Zombie Run.mv1");

        player->AddAnimation(ANIMATION_JUMP_IN, "Resource/3d/Zombie/Mutant JumpIn.mv1");
        player->AddAnimation(ANIMATION_JUMP_LOOP, "Resource/3d/Zombie/Mutant JumpLoop.mv1");
        player->AddAnimation(ANIMATION_JUMP_OUT, "Resource/3d/Zombie/Mutant JumpOut.mv1");
        return;
    }

    // 軍隊
    if (modelPath == "Resource/3d/Military/Guntai.mv1")
    {
        player->AddAnimation(ANIMATION_NEUTRAL, "Resource/3d/Military/Neutral.mv1");
        player->AddAnimation(ANIMATION_WALKING, "Resource/3d/Military/Running.mv1");
        player->AddAnimation(ANIMATION_RUN, "Resource/3d/Military/Running.mv1");
        player->AddAnimation(ANIMATION_ATTACK, "Resource/3d/Military/Attack.mv1");

        player->AddAnimation(ANIMATION_JUMP_IN, "Resource/3d/Military/Jump_In.mv1");
        player->AddAnimation(ANIMATION_JUMP_LOOP, "Resource/3d/Military/Jump_Loop.mv1");
        player->AddAnimation(ANIMATION_JUMP_OUT, "Resource/3d/Military/Jump_Out.mv1");
        return;
    }

    // ボブ
    if (modelPath == "Resource/3d/Bob/Bob.mv1")
    {
        player->AddAnimation(ANIMATION_NEUTRAL, "Resource/3d/Bob/Idle.mv1");
        player->AddAnimation(ANIMATION_WALKING, "Resource/3d/Bob/Run.mv1");
        player->AddAnimation(ANIMATION_RUN, "Resource/3d/Bob/Run.mv1");
        player->AddAnimation(ANIMATION_ATTACK, "Resource/3d/Bob/Attack.mv1");

        player->AddAnimation(ANIMATION_JUMP_IN, "Resource/3d/Bob/JumpIn.mv1");
        player->AddAnimation(ANIMATION_JUMP_LOOP, "Resource/3d/Bob/JumpLoop.mv1");
        player->AddAnimation(ANIMATION_JUMP_OUT, "Resource/3d/Bob/JumpOut.mv1");
        return;
    }

    // 肉塊の悪魔
    if (modelPath == "Resource/3d/Devil/Devil.mv1")
    {
        player->AddAnimation(ANIMATION_NEUTRAL, "Resource/3d/Devil/Idle.mv1");
        player->AddAnimation(ANIMATION_WALKING, "Resource/3d/Devil/Run.mv1");
        player->AddAnimation(ANIMATION_RUN, "Resource/3d/Devil/Run.mv1");
        player->AddAnimation(ANIMATION_ATTACK, "Resource/3d/Devil/Attack.mv1");

        player->AddAnimation(ANIMATION_JUMP_IN, "Resource/3d/Devil/JumpIn.mv1");
        player->AddAnimation(ANIMATION_JUMP_LOOP, "Resource/3d/Devil/JumpLoop.mv1");
        player->AddAnimation(ANIMATION_JUMP_OUT, "Resource/3d/Devil/JumpOut.mv1");
        return;

    }
}

void PlayerAnim::LoadSelectAnim(Model* model, const std::string& modelPath)
{
    if (!model) return;

    if (modelPath == "Resource/3d/Zombie/Zombie_Mesh.mv1")
    {
        model->AddAnimation(ANIMATION_NEUTRAL,"Resource/3d/Zombie/Ch10_nonPBR@Zombie Idle.mv1");
    }
    else if (modelPath == "Resource/3d/Military/Guntai.mv1")
    {
        model->AddAnimation(ANIMATION_NEUTRAL,"Resource/3d/Military/Neutral.mv1");
    }
    else if (modelPath == "Resource/3d/Bob/Bob.mv1")
    {
        model->AddAnimation(ANIMATION_NEUTRAL,"Resource/3d/Bob/Idle.mv1");
    }
    else if (modelPath == "Resource/3d/Devil/Devil.mv1")
    {
        model->AddAnimation(ANIMATION_NEUTRAL,"Resource/3d/Devil/Idle.mv1");
    }
}