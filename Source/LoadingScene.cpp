#include "DxLib.h"
#include "LoadingScene.h"
#include "Scene3D.h"
#include "SceneManager.h"
#include "Master.h"
#include <algorithm>
#include "GameManager.h"
#include "Utility.h"

LoadingScene::LoadingScene() : Scene() {}
LoadingScene::~LoadingScene() {}

void LoadingScene::Initialize()
{
    // ローディング画面だけ先に読む
    mLoadGraph = LoadGraph("Resource/Scene/sky.png");
    mSpinnerGraph = LoadGraph("Resource/2d/Load.png");

    // 画像
    mnHandle_Graph = LoadGraph("Resource/Manual/Hp.png");
    if (mnHandle_Graph == -1)
    {
        printfDx("説明画像がない");
    }
    mnHandle_Write = LoadGraph("Resource/Manual/Write.png");
    if (mnHandle_Write == -1) // 白い画像
    {
        printfDx("白い画像がない");
    }

    // 一旦表示
    ClearDrawScreen();
    DrawGraph(0, 0, mLoadGraph, TRUE);
    ScreenFlip();
    ProcessMessage();


     // 画像入れる
    mnHandle_P = Graph::VictoryPhoto(Master::msPlayerModelName);

    StartAsyncLoads();


    // BGM再生 ループさせたいからfalseに
    // ゲームをリスタートするときに流したいから入れた
     Master::mpSoundManager->PlaySE(SoundManager::SE_LOAD, 200);
    SetFontSize(35); // 文字の大きさ
}

void LoadingScene::StartAsyncLoads()
{
    // このあと新しくロードを開始する
    mLoadFailed = false;
    mHandles.clear();
    mDone.clear();
    // 非同期モード
    SetUseASyncLoadFlag(TRUE);


    // 空    まだ持っていない時だけ新しくロード  もうあるならそんまま使う
    if (GameManager::Resources.skyBoxModel == -1)
    {
        mnSkyBoxHandle = MV1LoadModel("Resource/3d/SkyBox/SkyBox.x");
    }
    else
    {
        mnSkyBoxHandle = GameManager::Resources.skyBoxModel;
    }


    // ステージ   まだ持っていない時だけ新しくロード  もうあるならそんまま使う
    if (GameManager::Resources.stageModel == -1)
    {
        mnStageHandle = MV1LoadModel("Resource/mapModel/model_map_game.mqo");
    }
    else
    {
        mnStageHandle = GameManager::Resources.stageModel;
    }


    // ステージのｋリジョン   まだ持っていない時だけ新しくロード  もうあるならそんまま使う
    if (GameManager::Resources.stageCollModel == -1)
    {
        mnStageCollHandle = MV1LoadModel("Resource/mapModel/model_map_game.mqo");
    }
    else
    {
        mnStageCollHandle = GameManager::Resources.stageCollModel;
    }


    // ぴれいやーのやつ毎回破棄する そうせんとアニメがバグる
    // 今持っているモデルを消す
    if (GameManager::Resources.playerModel != -1)
    {
        MV1DeleteModel(GameManager::Resources.playerModel);
        GameManager::Resources.playerModel = -1;
    }
    // 常に新しくロードする
    mnPlayerHandle = MV1LoadModel(Master::msPlayerModelName.c_str());
    // パスを覚えておく
    GameManager::Resources.playerModelPath = Master::msPlayerModelName;




    // 本数ベース進捗用に登録
    mHandles.push_back(mnStageHandle);
    mHandles.push_back(mnStageCollHandle);
    mHandles.push_back(mnSkyBoxHandle);
    mHandles.push_back(mnPlayerHandle);


    SetUseASyncLoadFlag(FALSE); // 非同期読み込みを行わないFALSE

    mTotalCount = (int)mHandles.size(); // 最大数をmHandlesを代入
    // assign... あたらしい値に置き換える
    mDone.assign(mTotalCount, false);

    // 初期化
    mTargetProgress = 0.0f;
    mViewProgress = 0.0f;
    mSpinnerAngle = 0.0;
}

void LoadingScene::Update()
{
    UpdateProgress();

    // 全完了していて、失敗もしていないなら次へ
    if (!mLoadFailed && mViewProgress >= 0.999f)
    {
        Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::SCENE_3D);
    }
    Scene::Update();
}

void LoadingScene::UpdateProgress()
{
    // カウントが０以下だったら読み込む最大数を１にする 最低保証
    if (mTotalCount <= 0)
    {
        mTargetProgress = 1.0f;
    }
    int finished = 0; // 進歩バーのため　代美子mンだファイルをカウント
    // 一つ一つ調べている　完了したか失敗したかとか
    for (int i = 0; i < mTotalCount; ++i)
    {
        if (mDone[i])
        {
            finished++;
            continue;
        }

        const int h = mHandles[i];
        const int r = CheckHandleASyncLoad(h);

        if (r == FALSE)
        {
            // 完了
            mDone[i] = true;
            finished++;
        }
        else if (r == -1)
        {
            // 失敗（ファイル無し/メモリ不足など）
            mLoadFailed = true;
        }
        // r == TRUE はまだロード中
    }

    // “本数ベース”の目標進捗（0～1）
    mTargetProgress = (mTotalCount > 0) ? (float)finished / (float)mTotalCount : 1.0f;

    // 見た目を滑らかにする（補間）
    // 0.15f は追従速度。速くしたいなら 0.25f くらいに。
    mViewProgress += (mTargetProgress - mViewProgress) * 0.15f;
    if (mViewProgress < 0.0f) mViewProgress = 0.0f;
    if (mViewProgress > 1.0f) mViewProgress = 1.0f;


    // スピナー回転（60fps想定の適当値）
    mSpinnerAngle += 0.12f; // ラジアン
    if (mSpinnerAngle > (float)DX_TWO_PI_F)
    {
        mSpinnerAngle -= (float)DX_TWO_PI_F;
    }
}

void LoadingScene::Draw()
{
    // 2D用に設定
    SetUseZBufferFlag(FALSE);
    SetWriteZBufferFlag(FALSE);


    // 背景描画
    DrawGraph(0, 0, mLoadGraph, TRUE);


    // プレイヤーの画像
    DrawGraph(0, 0, mnHandle_P, TRUE);


    // ===== スピナー（画面中央に回転する円）=====
    int sw = 0, sh = 0;
    GetDrawScreenSize(&sw, &sh);

    if(mSpinnerGraph != -1) // 画像があったら
    {
        const int cx = sw / 2;
        const int cy = sh / 2;
        const int r = 28;
        const int x = cx + (int)(cos(mSpinnerAngle) * r);
        const int y = cy + (int)(sin(mSpinnerAngle) * r);

        //回転描画
        DrawRotaGraph(1750, 880, 0.3f, mSpinnerAngle, mSpinnerGraph, TRUE);
    }

    // 進歩バーの位置決め
    const int barX = 40;      // X位置
    const int barY = 950;     // Y位置
    const int barW = 1800;    // バーの幅
    const int barH = 30;      // バーの高さ
 
    // 背景グレー
    DrawBox(barX, barY, (int)(barX + barW - 1.0f), barY + barH, GetColor(40, 40, 40), TRUE);

    // 進捗
    const int fillW = (int)(barW * mViewProgress);
    DrawBox(barX, barY, barX + fillW, barY + barH, GetColor(30, 250, 255), TRUE);

    // 背景枠
    DrawBox(barX, barY, (int)(barX + barW -1.0f), barY + barH, GetColor(255, 255, 255), FALSE); // 白い枠

    // %表示
    char buf[64]; // 文字が入る最大数
    const int percent = (int)(mViewProgress * 100 + 0.5f);
    sprintf_s(buf, "Now Loading...  %d%%", percent);
    DrawString(barX + barW - 550, barY - 35, buf, GetColor(255, 255, 255));

    // 白画像
    DrawRotaGraph(
        Utility::SCREEN_WIDTH / 2 + 300,
        Utility::SCREEN_HEIGHT / 2 - 100,
        1.03f,
        0.0f,
        mnHandle_Write,
        TRUE
    );
    // 説明の画像
    DrawRotaGraph(
        Utility::SCREEN_WIDTH / 2 + 300,
        Utility::SCREEN_HEIGHT / 2 - 100,
        1.0f,
        0.0f,
        mnHandle_Graph,
        TRUE
    );

    // メモ追加
    DrawFormatString(
        Utility::SCREEN_WIDTH / 2 - 140,
        750,
        GetColor(255, 255, 255),
        "操作方法はゲームのポーズ画面でも確認できます\n\n"
        "[tab / START] でポーズ画面が開けます"
    );



    // 失敗時表示
    if (mLoadFailed)
    {
       //  DrawString(40, 450, "Load Failed... (file missing or out of memory)", GetColor(255, 80, 80));
    }

    // 3D用に設定
    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);

    Scene::Draw();
}

void LoadingScene::Finalize()
{
    // ここでDeleteGraphするかは、画像を次シーンでも使う設計次第
     DeleteGraph(mLoadGraph);
     DeleteGraph(mSpinnerGraph);
     DeleteGraph(mnHandle_P);



    // これで Scene3D が始まった瞬間、ここにある番号を使い回せる
     GameManager::Resources.skyBoxModel = mnSkyBoxHandle;
     GameManager::Resources.stageModel = mnStageHandle;
     GameManager::Resources.stageCollModel = mnStageCollHandle;
     GameManager::Resources.playerModel = mnPlayerHandle;




     // 追加した
     for (int handle : mHandles)
     {
         if (handle != -1)
         {
            // MV1DeleteModel(handle); //ここで消さないように
         }
     }
     mHandles.clear();

     // BGM停止
     Master::mpSoundManager->StopBGM();
}