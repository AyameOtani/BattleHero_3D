#include "Trail3D.h"
#include "DxLib.h"

Trail3D::Trail3D()
    : m_lifeTime(0.5f)
    , m_color(0)
    , m_lastTime(-1)
    , m_prevBottom(VGet(0.0f, 0.0f, 0.0f))
    , m_prevTop(VGet(0.0f, 0.0f, 0.0f))
    , m_hasPrev(false)
{
    m_texture = LoadGraph("Resource/2d/Trail.png");
}

Trail3D::~Trail3D()
{
    DeleteGraph(m_texture);
}

void Trail3D::Initialize(float lifeTime, int color)
{
    m_lifeTime = (lifeTime > 0.0f) ? lifeTime : 0.001f;
    m_color = color;
    m_points.clear();
    m_lastTime = -1;
    m_hasPrev = false;
}

void Trail3D::Reset(const VECTOR& bottomPos, const VECTOR& topPos)
{
    m_points.clear();
    m_prevBottom = bottomPos;
    m_prevTop = topPos;
    m_hasPrev = true;
    m_lastTime = -1;
}

void Trail3D::SetColor(int color)
{
    m_color = color;
}

void Trail3D::SetLifeTime(float lifeTime)
{
    m_lifeTime = (lifeTime > 0.0f) ? lifeTime : 0.001f;
}

////-----------------------------------------------------------
//// 毎フレーム：下側と上側の2つの座標を受け取る
////-----------------------------------------------------------
//void Trail3D::Update(const VECTOR& bottomPos, const VECTOR& topPos)
//{
//    // 最初のフレームは前回座標のセットだけ
//    if (!m_hasPrev)
//    {
//        m_prevBottom = bottomPos;
//        m_prevTop = topPos;
//        m_hasPrev = true;
//    }
//
//    VECTOR diff = VSub(bottomPos, m_prevBottom);
//    float dist = VSize(diff);
//    int div = static_cast<int>(dist / 5.0f);
//    if (div < 1) div = 1;
//    if (div > 30) div = 30;
//    if (VSize(diff) > 2.0f)
//    {
//        // 前フレーム -> 今フレーム の間にセグメント追加
//        addSegment(m_prevBottom, m_prevTop, bottomPos, topPos, div);
//        m_prevBottom = bottomPos;
//        m_prevTop = topPos;
//    }
//
//    // 寿命更新
//    updateLife();
//}

void Trail3D::Update(const VECTOR& bottomPos, const VECTOR& topPos)
{
    // 最新の座標を追加
    m_controlBottom.push_back(bottomPos);
    m_controlTop.push_back(topPos);

    // 4点を超えたら古いものを消す
    if (m_controlBottom.size() > MAX_CONTROL_POINTS)
    {
        m_controlBottom.pop_front();
        m_controlTop.pop_front();
    }

    //  4点揃っているときだけ曲線補間を行う
    if (m_controlBottom.size() == 4)
    {
        int div = 20;
        for (int i = 0; i < div; ++i) {
            float t = (float)i / (float)div;

            // Catmull-Rom を計算する処理  ラムダ
            auto CalcCatmull = [&](const std::deque<VECTOR>& v, float t)
            {
                float t2 = t * t;
                float t3 = t2 * t;

                // Catmull-Rom の式
                // 0.5 * ((2*P1) + (-P0+P2)*t + (2*P0-5*P1+4*P2-P3)*t^2 + (-P0+3*P1-3*P2+P3)*t^3)
                // 4つの点を使って隙間の計算をしている
                // ０、２でどっちから来たか　１，３でどっちへ行くかを判断
                VECTOR res;
                res.x = 0.5f * (
                    (2.0f * v[1].x) +
                    (-v[0].x + v[2].x) * t +
                    (2.0f * v[0].x - 5.0f * v[1].x + 4.0f * v[2].x - v[3].x) * t2 +
                    (-v[0].x + 3.0f * v[1].x - 3.0f * v[2].x + v[3].x) * t3
                    );
                res.y = 0.5f * (
                    (2.0f * v[1].y) +
                    (-v[0].y + v[2].y) * t +
                    (2.0f * v[0].y - 5.0f * v[1].y + 4.0f * v[2].y - v[3].y) * t2 +
                    (-v[0].y + 3.0f * v[1].y - 3.0f * v[2].y + v[3].y) * t3
                    );
                res.z = 0.5f * (
                    (2.0f * v[1].z) +
                    (-v[0].z + v[2].z) * t +
                    (2.0f * v[0].z - 5.0f * v[1].z + 4.0f * v[2].z - v[3].z) * t2 +
                    (-v[0].z + 3.0f * v[1].z - 3.0f * v[2].z + v[3].z) * t3
                    );
                return res;
                };

            TrailPoint p;
            p.bottom = CalcCatmull(m_controlBottom, t);
            p.top = CalcCatmull(m_controlTop, t);
            p.life = 1.0f;

            m_points.push_back(p);
        }
    }

    // 寿命更新
    updateLife();

    // MAX_POINTS制限
    if (m_points.size() > MAX_POINTS)
    {
        m_points.erase(m_points.begin(), m_points.begin() + (m_points.size() - MAX_POINTS));
    }
}

//-----------------------------------------------------------
// 内部：セグメント追加（縦の2点ペアを補間して追加）
//-----------------------------------------------------------
void Trail3D::addSegment(
    const VECTOR& startBottom, const VECTOR& startTop,
    const VECTOR& endBottom, const VECTOR& endTop,
    int divNum
)
{
    if (divNum <= 0) divNum = 1;

    VECTOR diffBottom = VSub(endBottom, startBottom);
    VECTOR diffTop = VSub(endTop, startTop);

    // 直前の最後の点と startBottom & startTop が同じなら最初の点をスキップ
    bool skipFirst = false;
    if (!m_points.empty())
    {
        const auto& last = m_points.back();
        if (last.bottom.x == startBottom.x &&
            last.bottom.y == startBottom.y &&
            last.bottom.z == startBottom.z &&
            last.top.x == startTop.x &&
            last.top.y == startTop.y &&
            last.top.z == startTop.z)
        {
            skipFirst = true;
        }
    }

    for (int i = 0; i <= divNum; ++i)
    {
        if (skipFirst && i == 0) continue;

        float rate = static_cast<float>(i) / static_cast<float>(divNum);

        TrailPoint p;
        p.bottom = VAdd(startBottom, VScale(diffBottom, rate));
        p.top = VAdd(startTop, VScale(diffTop, rate));
        p.life = 1.0f;

        m_points.push_back(p);
    }

    if (m_points.size() > MAX_POINTS)
    {
        m_points.erase(
            m_points.begin(),
            m_points.begin() + (m_points.size() - MAX_POINTS)
        );
    }
}

//-----------------------------------------------------------
// 内部：寿命更新
//-----------------------------------------------------------
void Trail3D::updateLife()
{
    if (m_points.empty()) return;

    int now = GetNowCount();
    if (m_lastTime < 0)
    {
        m_lastTime = now;
        return;
    }

    float dt = (now - m_lastTime) / 800.0f;
    m_lastTime = now;

    float dec = dt / m_lifeTime;

    for (auto& p : m_points)
    {
        p.life -= dec;
        if (p.life < 0.0f) p.life = 0.0f;
    }

    // 先頭から寿命切れを削除
    while (!m_points.empty() && m_points.front().life <= 0.0f)
    {
        m_points.erase(m_points.begin());
    }
}

//-----------------------------------------------------------
// 描画：縦の板ポリゴンをならべる
//-----------------------------------------------------------
void Trail3D::Draw() const
{
    const auto& pts = m_points;
    const size_t n = pts.size();
    if (n < 2) return;

    int baseColor = m_color;
    if (baseColor == 0)
    {
        baseColor = GetColor(255, 255, 255);
    }

    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);
    SetUseBackCulling(FALSE);   // 両面表示（縦板なので裏表両方見えるように）


    for (size_t i = 0; i + 1 < n; ++i)
    {
        const TrailPoint& a = pts[i];
        const TrailPoint& b = pts[i + 1];

        if (a.life <= 0.0f && b.life <= 0.0f) continue;

        float life = (a.life + b.life) * 0.5f;
        if (life <= 0.0f) continue;

        int alpha = static_cast<int>(life * 255.0f);
        if (alpha <= 0) continue;

        VECTOR bottomA = a.bottom;
        VECTOR topA = a.top;
        VECTOR bottomB = b.bottom;
        VECTOR topB = b.top;

        SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);

        // 縦の四角形（2枚の三角形）
        //  bottomA ----- bottomB
        //    |            |
        //   topA ------  topB

        // 三角形1枚目 (bottomA, topA, topB)
        // DrawTriangle3D(bottomA, topA, topB, baseColor, TRUE);
        // 三角形2枚目 (bottomA, topB, bottomB)
        // DrawTriangle3D(bottomA, topB, bottomB, baseColor, TRUE);

        float u0 = (float)i / (float)(n - 1);
        float u1 = (float)(i + 1) / (float)(n - 1);
        VERTEX3D v[6];
        // 三角形1
        v[0].pos = bottomA;
        v[0].u = u0; v[0].v = 1.0f;
        v[0].dif = GetColorU8(255, 255, 255, alpha);
        v[1].pos = topA;
        v[1].u = u0; v[1].v = 0.0f;
        v[1].dif = GetColorU8(255, 255, 255, alpha);
        v[2].pos = topB;
        v[2].u = u1; v[2].v = 0.0f;
        v[2].dif = GetColorU8(255, 255, 255, alpha);
        // 三角形2
        v[3].pos = bottomA;
        v[3].u = u0; v[3].v = 1.0f;
        v[3].dif = GetColorU8(255, 255, 255, alpha);
        v[4].pos = topB;
        v[4].u = u1; v[4].v = 0.0f;
        v[4].dif = GetColorU8(255, 255, 255, alpha);
        v[5].pos = bottomB;
        v[5].u = u1; v[5].v = 1.0f;
        v[5].dif = GetColorU8(255, 255, 255, alpha);

        DrawPrimitive3D(
            v,
            6,
            DX_PRIMTYPE_TRIANGLELIST,
            m_texture,
            TRUE   // 半透明有効
        );

    }


    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    SetUseBackCulling(TRUE);



   // // デバッグ用：点を可視化
   // for (const auto& p : m_points)
   // {
   //     if (p.life <= 0.0f) continue;
   // 
   //     int alpha = static_cast<int>(p.life * 255.0f);
   //     SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
   // 
   //     DrawSphere3D(p.bottom, 2.0f, 6, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
   //     DrawSphere3D(p.top, 2.0f, 6, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);
   // }
   // SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
