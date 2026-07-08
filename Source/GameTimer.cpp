#include "GameTimer.h"
#include "Master.h"

GameTimer::GameTimer()
{
    prevTime = GetNowCount();

    mnFontBig = CreateFontToHandle(
        NULL, // "k8x12L" これ戦国のときの
        60,
        3
    );
}


GameTimer::~GameTimer()
{
    DeleteFontToHandle(mnFontBig);
}

// 時間を取り直す
void GameTimer::Reset()
{
    prevTime = GetNowCount();
}

void GameTimer::Update()
{
    // デルタタイム計算
    int now = GetNowCount(); // 開始から経過時間を1/1000で返すもの 
    float dt = (now - prevTime) / 1000.0f; // 前frameから今フレームまでに経過した時間
    prevTime = now;

    // すでに０なら何もしない
    if (filledSeg <= 0 && partial <= 0.0f)
    {
        filledSeg = 0;
        partial = 0.0f;
        return;
    }

    // 1秒あたりに減る枚数
   // ゲージの最大数/ 秒数 = １秒で減る枚数(今は1,2)
    float segPerSec = SEGMENTS / TotalTime;
    // 端数に加算
    partial -= segPerSec * dt;

    // 端数 から 枚数へ繰り上げ
    while (partial < 0.0f && filledSeg > 0)
    {
        partial += 1.0f; // 減る時に、1.0で最大になるから
        filledSeg--; // 枚数を減らす

        if (filledSeg <= 0)
        {
            filledSeg = 0;
            partial = 0.0f;
            break;
        }
    }
}

void GameTimer::Initialize()
{
    filledSeg = (int)SEGMENTS;
    partial = 0.0f; // ここを0.0にしておかないと時間+1からカウントが始まる
    prevTime = GetNowCount();
    mnRemainTime = (int)TotalTime;
}


void GameTimer::Draw()
// まあ妥協
{
    float centerX = 150; // 中央X
    float centerY = 150; // 中心Y
    float stringcenter = centerY - 30.0f; // 文字を真ん中にしたいから３０ぐらい引く
    int color = GetColor(80, 80, 80);

    //背景
    DrawCircleAA(
        centerX,
        centerY,
        97.0, // 少し小さくしている
        128,
        color,
        TRUE,
        4.0f
    );

    // 本体ゲージ
    DrawRadialGauge(
        centerX,
        centerY,
        100.0f,
        filledSeg,
        partial,
        (int)SEGMENTS,
        GetColor(40, 40, 40), // ゲージの色はこっち
        GetColor(255, 255, 255), // 背景はコメントにしてあるから関係なし
        -90.0f
    );

    // 外枠（見た目用）
    DrawCircleAA(
        centerX,
        centerY,
        110.0f,
        128,
        GetColor(40, 40, 40),
        FALSE,
        4.0f
    );

    // 時間を真ん中に表示するためのやつら
    // 残り秒数を計算 今お子まで進んだか＊１セグメントあたりの秒数
    float remainTime = ((float)filledSeg + partial) * (TotalTime / SEGMENTS);
    // 少数の切り上げ ceilf... floatの数値を切り上げらいし
    mnRemainTime = static_cast<int>(std::ceil(remainTime));
    // 文字列の描画幅を計算する
    int widht = GetDrawFormatStringWidthToHandle(
        mnFontBig,
        "%d",
        mnRemainTime
    );

   
    float DrawX = centerX - widht / 2; // 150を中心としたときの左上の座標

    int WriteColor = GetColor(255, 255, 255); // 文字色白
    int RedColor = GetColor(250, 90, 90); // 赤

    if (mnRemainTime != mnChengeCount)
    {
        if (mnRemainTime <= 10 && mnRemainTime > 0) // 3,2,1 のとき
        {
            Master::mpSoundManager->PlaySE(SoundManager::SE_TIMER, 120); // タイマー
        }
        else if (mnRemainTime == 0)
        {
            Master::mpSoundManager->PlaySE(SoundManager::SE_SELECT_OK, 140); // タイマー
        }
        mnChengeCount = mnRemainTime; // 前回カウント更新
    }

   if (mnRemainTime <= 10)
   {
       // 指定したフォント　サイズで表示
       DrawFormatStringToHandle(
           (int)DrawX,
           (int)stringcenter,
           RedColor,
           mnFontBig,
           "%d",
           mnRemainTime // 表示する秒数 少数切り上げint
       );
   }
   else
   {
       // 指定したフォント　サイズで表示
       DrawFormatStringToHandle(
           (int)DrawX,
           (int)stringcenter,
           WriteColor,
           mnFontBig,
           "%d",
           mnRemainTime // 表示する秒数 少数切り上げint
       );
   }


}

// 円を書く関数
void GameTimer::DrawRadialGauge(float cx,float cy, float r,
    int filledSeg, float partial, int segments,
    unsigned int colFill, unsigned int colBack, float startDeg = -90.0f)
{
    if (segments < 1) segments = 1;                 // 最大数が１より少なかったら１にする
    if (filledSeg < 0) filledSeg = 0;               // 埋まっている三角形の数が０以下なら０にする
    if (filledSeg > segments) filledSeg = segments; // 枚数が最大数を変えたら最大数にする
    if (partial < 0.0f) partial = 0.0f;             // 埋まり具合が０以下なら０にする
    if (partial > 1.0f) partial = 1.0f;             // 埋まり具合が１以上なら１にする

    stepDeg = 360.0f / segments; // 初期化 最大数を円の最大角度(360.0f)で割る

    //// 背景（未充填）
    //for (int i = 0; i < segments; ++i) // i = 今書いている扇
    //{
    //    float a0 = DegToRad(startDeg + stepDeg * i);           // 今書いている扇がどこから始まるか
    //    float a1 = DegToRad(startDeg + stepDeg * (i + 1));     // 今書いている扇がどこで終わるか +1で次の扇の位置を終わりにしている
    //    float x0 = cx + r * cosf(a0), y0 = cy + r * sinf(a0);  // 扇の端っこになる円周上の位置 始点
    //    float x1 = cx + r * cosf(a1), y1 = cy + r * sinf(a1);  // 扇の端っこになる円周上の位置 終点
    //    DrawTriangleAA(cx, cy, x0, y0, x1, y1, colBack, TRUE); // ここで背景色の三角の集合体を描画
    //}

    // 充填（完全に埋まる分）
    for (size_t  i = 0; i < filledSeg; ++i)
    {
        float a0 = DegToRad(startDeg + stepDeg * i);
        float a1 = DegToRad(startDeg + stepDeg * (i + 1));
        float x0 = cx + r * cosf(a0), y0 = cy + r * sinf(a0);
        float x1 = cx + r * cosf(a1), y1 = cy + r * sinf(a1);
        DrawTriangleAA(cx, cy, x0, y0, x1, y1, colFill, TRUE); // 埋まった部分のゲージ描画
    }

    // 端数分（部分的に埋まる1枚）
    if (partial > 0.0f && filledSeg < segments)
    {
        float a0 = DegToRad(startDeg + stepDeg * filledSeg);
        float aP = DegToRad(startDeg + stepDeg * (filledSeg + partial));
        float x0 = cx + r * cosf(a0), y0 = cy + r * sinf(a0);
        float xP = cx + r * cosf(aP), yP = cy + r * sinf(aP);
        DrawTriangleAA(cx, cy, x0, y0, xP, yP, colFill, TRUE); // 一枚のpartialの埋まり具合の描画
    }
}

