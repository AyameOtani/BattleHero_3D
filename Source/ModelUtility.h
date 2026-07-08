#pragma once

// 
// ★New★
// 新規追加ファイル
//


// アニメーションの切り替え番号
enum AnimationState
{
    ANIMATION_NEUTRAL = 0,  // 待機
    ANIMATION_RUN,          // 走り
    ANIMATION_JUMP_IN,      // ジャンプ開始
    ANIMATION_JUMP_LOOP,    // ジャンプ中
    ANIMATION_JUMP_OUT,     // 着地時
    ANIMATION_ATTACK,       // 攻撃


    // note: ↑ここから上には追加しない。
    //       Hero.mv1 がこの順番じゃないと機能しないので維持しておく。
    //       ただし、SeparateModelAnimation の方で使ってはいけない訳ではないので注意。



    // ★New★
    // 上にある定義以外で使いたいモーションがあれば、このように下に追加していけばOK。
    ANIMATION_WALKING,      // 歩き
    ANIMATION_DYING,        // 死亡時
    ANIMATION_DROP_KICK,    // ドロップキック




    ANIMATION_MAX
};

// ★New★
// 分割されているアニメーションのデータ
struct AnimationInfo
{
    AnimationState mState;
    int mnAnimationHandle;
};