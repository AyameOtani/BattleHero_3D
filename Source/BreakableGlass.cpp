#include "BreakableGlass.h"
#include <cmath>

BreakableGlass::BreakableGlass() {}

BreakableGlass::~BreakableGlass()
{
    // デストラクタで後始末（モデル解放など）
    Finalize();
}

bool BreakableGlass::Load(const char* intactPath, const char* shardsPath)
{
    // 既にロード済みのものがあれば先に解放してクリーンな状態にする
    Finalize();

    // 割れていないガラス（1枚の板など）モデルをロード
    mIntact = MV1LoadModel(intactPath);
    if (mIntact == -1) return false; // 失敗

    // 破片モデル（フレームに破片が分かれて入っている想定）をロード
    mShards = MV1LoadModel(shardsPath);
    if (mShards == -1)
    {
        // 破片モデルのロードに失敗したら、先に読み込んだ intact も解放して戻る
        MV1DeleteModel(mIntact);
        mIntact = -1;
        return false;
    }

    // intactモデルに、現在設定されている位置(mPos)とスケール(mScale)を反映
    ApplyTransformToIntact();

    // 破片一覧（Piece配列）を作る＆破片モデルを複製して保持する
    BuildPieces();

    // 破壊前状態に戻す（破片無効、寿命や速度初期化など）
    Reset();

    return true;
}

void BreakableGlass::Finalize()
{
    // 破片用に複製したモデルをすべて解放
    for (auto& p : mPieces)
    {
        if (p.model != -1)
        {
            MV1DeleteModel(p.model);
            p.model = -1;
        }
    }
    mPieces.clear();

    // intactモデル解放
    if (mIntact != -1)
    {
        MV1DeleteModel(mIntact);
        mIntact = -1;
    }

    // shardsモデル解放（複製元）
    if (mShards != -1)
    {
        MV1DeleteModel(mShards);
        mShards = -1;
    }

    // 状態も初期化
    mBroken = false;
}

void BreakableGlass::Reset()
{
    // 「割れていない状態」に戻す
    mBroken = false;

    // intactモデルを(位置/スケール)に合わせる
    ApplyTransformToIntact();

    // 破片の物理状態を初期化
    for (auto& p : mPieces)
    {
        p.active = false;        // 破片は描画/更新しない
        p.life = 0.0f;           // 寿命0
        p.pos = mPos;            // とりあえずガラス位置に置いておく
        p.vel = VGet(0, 0, 0);   // 速度0
        p.rot = VGet(0, 0, 0);   // 回転0
        p.rotVel = VGet(0, 0, 0);// 回転速度0

        if (p.model != -1)
        {
            // モデル側にも反映（表示が破片側に切り替わったときに変な場所にいないように）
            MV1SetPosition(p.model, mPos);
            MV1SetRotationXYZ(p.model, p.rot);
            MV1SetScale(p.model, VGet(mScale, mScale, mScale));
        }
    }
}

float BreakableGlass::Rand01() const
{
    // 0.0～1.0 の乱数（GetRand(10000) は 0～10000）
    return (float)(GetRand(10000) / 10000.0f);
}

float BreakableGlass::RandRange(float a, float b) const
{
    // a～b の範囲の乱数を作る
    return a + (b - a) * Rand01();
}

void BreakableGlass::ApplyTransformToIntact()
{
    // intactモデルが未ロードなら何もしない
    if (mIntact == -1) return;

    // intactモデルを、オブジェクトの位置とスケールに合わせる
    MV1SetPosition(mIntact, mPos);
    MV1SetScale(mIntact, VGet(mScale, mScale, mScale));
}

void BreakableGlass::ApplyScaleToPieces()
{
    // 破片モデル全員にスケールだけ反映したい時用
    for (auto& p : mPieces)
    {
        if (p.model == -1) continue;
        MV1SetScale(p.model, VGet(mScale, mScale, mScale));
    }
}

void BreakableGlass::BuildPieces()
{
    // 破片一覧を作り直す
    mPieces.clear();

    // shardsモデルがないなら作れない
    if (mShards == -1) return;

    // MV1のフレーム数（破片がフレームとして入っている前提）
    const int frameNum = MV1GetFrameNum(mShards);

    // 破片フレーム列挙
    // ※0番フレームはルート(全体)になってる事が多いのでスキップ
    for (int fi = 0; fi < frameNum; ++fi)
    {
        if (fi == 0) continue;

        Piece p;
        p.frameIndex = fi; // この破片が担当するフレーム番号

        // ここでローカル座標を保存
        p.local = MV1GetFramePosition(mShards, fi);


        mPieces.push_back(p);
    }

    // 破片を「独立して動かす」ため、shardsモデルを各Pieceごとに複製して持つ
    for (auto& p : mPieces)
    {
        p.model = MV1DuplicateModel(mShards);
        if (p.model != -1)
        {
            // 初期位置/スケールを合わせる（破壊前は使わないが安全のため）
            MV1SetScale(p.model, VGet(mScale, mScale, mScale));
            MV1SetPosition(p.model, mPos);
        }
    }
}

void BreakableGlass::Break(const VECTOR& impactPos, float power)
{
    // 既に割れていたら二重で割らない
    if (mBroken) return;

    // 「割れた」フラグ
    mBroken = true;

    // 各破片に「衝突点から外へ飛ぶ」初速を与える
    for (auto& p : mPieces)
    {
        if (p.model == -1) continue;

        // この破片フレームの位置（モデル内のローカル位置っぽい値）を取得
        VECTOR local = MV1GetFramePosition(p.model, p.frameIndex);

        // ローカル位置 + ガラス本体位置 で「だいたいの世界座標」にする
        // ※厳密には回転やスケール込みの変換が必要だが、ここは簡易版
        VECTOR worldPiecePos = VAdd(mPos, p.local);

        // 破片の初期位置は、その破片の位置
        p.pos = worldPiecePos;

        // 衝突点→破片方向のベクトル（外向き）
        VECTOR dir = VSub(worldPiecePos, impactPos);

        // 長さ（0に近いと正規化できないので例外処理）
        float len = VSize(dir);
        if (len < 0.0001f)
        {
            // ほぼ同一点だった場合はランダム方向に飛ばす
            dir = VGet(RandRange(-1.0f, 1.0f), RandRange(0.2f, 1.0f), RandRange(-1.0f, 1.0f));
            len = VSize(dir);
            if (len < 0.0001f) len = 1.0f;
        }

        // 正規化（長さ1の方向ベクトルにする）
        dir = VScale(dir, 1.0f / len);

        // 飛ぶ速度：powerにランダム係数をかける
        float sp = power * RandRange(0.6f, 1.25f);

        // 速度ベクトル
        p.vel = VScale(dir, sp);

        // 回転はゼロからスタート
        p.rot = VGet(0, 0, 0);

        // 回転速度をランダムで与える（ラジアン想定）
        p.rotVel = VGet(
            RandRange(-7.0f, 7.0f),
            RandRange(-7.0f, 7.0f),
            RandRange(-7.0f, 7.0f)
        );

        // 破片寿命（一定時間で消える）
        p.life = mShardLife;
        p.active = true;

        // モデルに反映（いきなり描画しても正しい位置）
        MV1SetPosition(p.model, p.pos);
        MV1SetRotationXYZ(p.model, p.rot);
        MV1SetScale(p.model, VGet(mScale, mScale, mScale));
    }
}

void BreakableGlass::Update(float dt)
{
    // 何もロードされていないなら更新不要
    if (mIntact == -1) return;

    // 割れていないときは intact を位置に追従させるだけ
    if (!mBroken)
    {
        ApplyTransformToIntact();
        return;
    }

    // 割れている時：破片を物理更新
    for (auto& p : mPieces)
    {
        // activeじゃない破片は更新しない
        if (!p.active || p.model == -1) continue;

        // 寿命減らす
        p.life -= dt;
        if (p.life <= 0.0f)
        {
            // 寿命切れで非表示/停止
            p.active = false;
            continue;
        }

        // 重力（加速度）を速度へ加える: v += g * dt
        p.vel = VAdd(p.vel, VScale(mGravity, dt));

        // 減衰（空気抵抗っぽい）: v *= damping
        // ※mDampingが 0.98 みたいな値だと徐々に遅くなる
        p.vel = VScale(p.vel, mDamping);

        // 位置更新: pos += v * dt
        p.pos = VAdd(p.pos, VScale(p.vel, dt));

        // 地面バウンド（オプション）
        if (mUseGround && p.pos.y < mGroundY)
        {
            // 地面より下に潜ったら地面上に戻す
            p.pos.y = mGroundY;

            // 下向き速度だった場合だけ反発
            if (p.vel.y < 0.0f)
            {
                // 反発係数で跳ね返す: vy = -vy * restitution
                p.vel.y = -p.vel.y * mRestitution;

                // 横方向も少し減衰（滑り止め）
                p.vel.x *= 0.92f;
                p.vel.z *= 0.92f;
            }
        }

        //--回転しながら壊れるなら下記処理使う
        // 回転更新: rot += rotVel * dt
        // p.rot = VAdd(p.rot, VScale(p.rotVel, dt));
        // 回転速度も減衰させる
        // p.rotVel = VScale(p.rotVel, mDamping);

        // モデルへ反映
        MV1SetPosition(p.model, p.pos);
        //MV1SetRotationXYZ(p.model, p.rot);
        MV1SetScale(p.model, VGet(mScale, mScale, mScale));
    }
}

void BreakableGlass::Draw() const
{
    // ロードされてなければ描画しない
    if (mIntact == -1) return;

    // 割れていない場合：intactモデルだけ描画
    if (!mBroken)
    {
        // ガラスっぽく半透明描画（mIntactAlpha 0～255想定）
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, mIntactAlpha);
        MV1DrawModel(mIntact);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        return;
    }

    // 割れている場合：破片だけ描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, mShardAlpha);

    for (const auto& p : mPieces)
    {
        if (!p.active || p.model == -1) continue;

        // 「この破片フレームだけ描画」する
        // ※モデルは複製しているけど、フレーム単位で描画して破片っぽく見せる方式
        MV1DrawFrame(p.model, p.frameIndex);
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
