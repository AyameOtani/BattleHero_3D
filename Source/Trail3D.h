#pragma once

#include "DxLib.h"
#include <vector>
#include <deque> // デックというらしい　戦闘にも末尾にも追加削除できるやつ

class Trail3D
{
public:
    Trail3D();
    ~Trail3D();

    // 初期化（lifeTime = 軌跡が消えるまでの秒数）
    void Initialize(float lifeTime, int color);

    // 軌跡のリセット（プレイヤー初期位置と、上側の初期位置）
    void Reset(const VECTOR& bottomPos, const VECTOR& topPos);

    // 毎フレーム呼ぶ：
    // bottomPos = プレイヤーの座標
    // topPos    = プレイヤー座標 Y+200 など、縦方向の先端
    void Update(const VECTOR& bottomPos, const VECTOR& topPos);

    // 描画
    void Draw() const;

    void SetColor(int color);
    void SetLifeTime(float lifeTime);

private:
    struct TrailPoint
    {
        VECTOR bottom;  // 下側の点（プレイヤー側）
        VECTOR top;     // 上側の点（Y+200 側）
        float  life;    // 0.0f ～ 1.0f
    };

    void addSegment(
        const VECTOR& startBottom, const VECTOR& startTop,
        const VECTOR& endBottom, const VECTOR& endTop,
        int divNum
    );
    void updateLife();

private:
    std::vector<TrailPoint> m_points;
    float  m_lifeTime;  // 全体寿命（秒）
    int    m_color;     // DxLib のカラー値

    int    m_lastTime;      // GetNowCount 用
    VECTOR m_prevBottom;    // 前フレームの下側座標
    VECTOR m_prevTop;       // 前フレームの上側座標
    bool   m_hasPrev;       // 前フレーム座標を持っているかどうか

    static constexpr size_t MAX_POINTS = 1000;

    int m_texture;
private:
    // 先輩に教わった
    std::deque<VECTOR> m_controlBottom; // 制御点 最新の４つの点を保存するのに適
    std::deque<VECTOR> m_controlTop;    // 制御点 
    static constexpr size_t MAX_CONTROL_POINTS = 4; // 常に4点保持
};