#pragma once
#include <DxLib.h>
#include <vector>

//============================================================
// BreakableGlass クラス
//
// ・intact(通常モデル) と shards(破片モデル) を読み込む
// ・Break() を呼ぶとガラスが割れて破片が飛び散る
// ・Update() で物理計算（重力・減衰・寿命管理）
// ・Draw() で描画（半透明ガラス or 破片表示）
//============================================================
class BreakableGlass
{
public:
    BreakableGlass();
    ~BreakableGlass();

    // intactPath : 割れていない状態のモデル
    // shardsPath : 破片がフレームに分かれているモデル
    bool Load(const char* intactPath, const char* shardsPath);

    // モデル解放などの後始末
    void Finalize();

    // 元の状態（割れていない状態）に戻す
    void Reset();

    // ガラスを割る
    // impactPos : 衝突した位置（破片が飛ぶ基準点）
    // power     : 飛び散る強さ
    void Break(const VECTOR& impactPos, float power);

    // 毎フレーム更新（物理計算）
    void Update(float dt);

    // 描画
    void Draw() const;

    // 割れているかどうか
    bool IsBroken() const { return mBroken; }

    //==============================
    // transform（位置・サイズ）
    //==============================

    // 位置を設定
    void SetPosition(const VECTOR& pos)
    {
        mPos = pos;
        ApplyTransformToIntact(); // intactモデルにも反映
    }

    // 現在位置を取得
    const VECTOR& GetPosition() const { return mPos; }

    // 拡大縮小率を設定
    void SetScale(float s)
    {
        mScale = s;
        ApplyTransformToIntact(); // intact側に反映
        ApplyScaleToPieces();     // 破片側にも反映
    }

    // 現在のスケール取得
    float GetScale() const { return mScale; }

    //==============================
    // 見た目（透明度）
    //==============================

    // 割れていない時の透明度（0～255）
    void SetIntactAlpha(int a) { mIntactAlpha = ClampAlpha(a); }

    // 割れた後（破片）の透明度（0～255）
    void SetShardAlpha(int a) { mShardAlpha = ClampAlpha(a); }

    //==============================
    // 物理パラメータ
    //==============================

    // 重力の強さ（例：VGet(0,-980,0)）
    void SetGravity(const VECTOR& g) { mGravity = g; }

    // 破片が消えるまでの時間（秒）
    void SetLife(float sec)
    {
        // 短すぎると見えないので最低0.01秒
        mShardLife = (sec < 0.01f) ? 0.01f : sec;
    }

    // 減衰率（空気抵抗みたいなもの）
    // 1.0fに近いほど止まりにくい
    void SetDamping(float d)
    {
        // 0～1の範囲に制限
        mDamping = (d < 0.0f) ? 0.0f : (d > 1.0f ? 1.0f : d);
    }

    //==============================
    // オプション：地面バウンド
    //==============================

    // enable     : 有効/無効
    // groundY    : 地面の高さ
    // restitution: 跳ね返り係数（0～1）
    void SetGroundBounce(bool enable, float groundY = 0.0f, float restitution = 0.35f)
    {
        mUseGround = enable;
        mGroundY = groundY;
        mRestitution = restitution;
    }

private:
    //================================================
    // 1つの破片データ
    //================================================
    struct Piece
    {
        int model = -1;          // 破片用の複製モデル
        int frameIndex = -1;     // 描画するフレーム番号

        VECTOR pos = VGet(0, 0, 0); // 現在位置
        VECTOR vel = VGet(0, 0, 0); // 移動速度

        VECTOR rot = VGet(0, 0, 0);     // 現在回転（XYZラジアン）
        VECTOR rotVel = VGet(0, 0, 0);  // 回転速度

        float life = 0.0f;   // 残り寿命
        bool active = false; // 表示・更新するかどうか

        VECTOR local = VGet(0, 0, 0); // <-- 追加: フレームのローカル座標
    };


    // α値を 0～255 に収める
    static int ClampAlpha(int a)
    {
        if (a < 0) return 0;
        if (a > 255) return 255;
        return a;
    }

    // 0.0～1.0 の乱数
    float Rand01() const;

    // a～b の範囲の乱数
    float RandRange(float a, float b) const;

    // shardsモデルから破片一覧を作る
    void BuildPieces();

    // intactモデルに位置・スケールを反映
    void ApplyTransformToIntact();

    // 破片モデルすべてにスケールを反映
    void ApplyScaleToPieces();


    //================================================
    // モデル関連
    //================================================
    int mIntact = -1; // 割れていないモデル
    int mShards = -1; // 破片モデル（複製元）

    //================================================
    // transform
    //================================================
    VECTOR mPos = VGet(0, 0, 0); // ガラスの位置
    float  mScale = 1.0f;       // 拡大縮小率

    // 割れているかどうか
    bool mBroken = false;

    //================================================
    // 見た目
    //================================================
    int mIntactAlpha = 140; // intact時の透明度
    int mShardAlpha = 200;  // 破片時の透明度

    //================================================
    // 物理パラメータ
    //================================================
    VECTOR mGravity = VGet(0.0f, -980.0f, 0.0f); // 重力
    float  mShardLife = 1.5f; // 破片が消えるまでの時間（秒）
    float  mDamping = 0.985f; // 減衰率

    //================================================
    // 地面バウンド用
    //================================================
    bool  mUseGround = false; // 地面判定を使うか
    float mGroundY = 0.0f;    // 地面の高さ
    float mRestitution = 0.35f; // 跳ね返り係数

    // 破片配列
    std::vector<Piece> mPieces;
};
