#pragma once
#pragma once
#include "Scene.h"
#include <vector>
#include "Graph.h"

class LoadingScene : public Scene
{
public:
    LoadingScene(); // コンスト
    ~LoadingScene(); // デスト

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

private:
    // Async は非同期処理の事
    void StartAsyncLoads(); // 非同期でロードする関数
    void UpdateProgress(); // 進歩バーの設定など 0と1とか

private:
    int mLoadGraph = -1; // 背景画像
    int mSpinnerGraph = -1; // 回転するやつの画像

    // double の方が正確だが分かりやすいようにfloatにした 0121
    float mSpinnerAngle = 0.0; // スピナーの線を回転させるやつ

    // 非同期ロード対象（本数ベース進捗）
    std::vector<int> mHandles; // バー用に登録するやつ　追加するからVECTOR
    std::vector<bool> mDone; // バーの進歩で使う
    int mTotalCount = 0; // 読み込むやつの最大数を保存する

    // 表示用進捗（0.0～1.0）
    float mTargetProgress = 0.0f; // 読み込むやつの最大数ベースの進歩で使う
    float mViewProgress = 0.0f; // 進歩中のゲージで使う

    // コードで使っているハンドル ここでpushBackしている
    int mnStageHandle = -1;
    int mnStageCollHandle = -1;
    int mnSkyBoxHandle = -1;
    int mnPlayerHandle = -1;

    bool mLoadFailed = false; // rp－ディングに失敗したらメッセージを出す用のフラグ


    // ローディングの時に出す画像
    int mnHandle_Graph = -1;
    int mnHandle_Write = -1; // 白い画像

    int mnHandle_P = 0; // プレイやー入れる奴
};

