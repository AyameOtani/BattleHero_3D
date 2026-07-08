#include "Loading.h"
#include "DxLib.h"
#include <string>
#include <cstdio>
#include "Utility.h"

// ========================
// LoadSoundTask 実装
// ========================

LoadSoundTask::LoadSoundTask(const char* path)
    : m_path(path)
    , m_handle(-1)
{

}

int LoadSoundTask::Execute()
{
    m_handle = LoadSoundMem(m_path);
    if (m_handle == -1)
    {
        // 読み込み失敗時はログなどに出しても良い
        char buf[256];
        //std::sprintf(buf, "Failed to load sound: %s", m_path);
        OutputDebugStringA(buf);
    }

    return m_handle;
}

const char* LoadSoundTask::GetTaskName() const
{
    return m_path;
}

int LoadSoundTask::GetHandle() const
{
    return m_handle;
}

// ========================
// InitializeSoundManagerTask 実装
// ========================

InitializeSoundManagerTask::InitializeSoundManagerTask() {}

int InitializeSoundManagerTask::Execute()
{
    // サウンドマネージャーの初期化処理をここに書く
    // 今はダミーで待機だけ
    WaitTimer(100);

    return 0;
}

const char* InitializeSoundManagerTask::GetTaskName() const
{
    return "Initialize Sound Manager";
}

// ========================
// InitializeSceneManagerTask 実装
// ========================

InitializeSceneManagerTask::InitializeSceneManagerTask() {}

int InitializeSceneManagerTask::Execute() {
    // シーンマネージャーの初期化処理をここに書く
    // 今はダミーで待機だけ
    WaitTimer(100);

    return 0;
}

const char* InitializeSceneManagerTask::GetTaskName() const
{
    return "Initialize Scene Manager";
}

// ========================
// LoadingManager 実装
// ========================


void LoadingManager::AddTask(std::unique_ptr<ILoadTask> task)
{
    m_tasks.push_back(std::move(task));
}



void LoadingManager::ExecuteAll()
{
    const int total = static_cast<int>(m_tasks.size());

    // 進捗バーの描画 位置など
    const int barX = Utility::SCREEN_WIDTH / 2 + 700, // Xの位置 元々440
              barY = Utility::SCREEN_HEIGHT / 2 + 400,// Yの位置 元々400
              barWidth = 400, // バーの幅
              barHeight = 20; // バーの高さ

    DrawString(barX + 50, barY - 40, "Now Loading...", GetColor(255, 0, 0));

    for (int i = 0; i < total; ++i) // 全部読み込むまで繰り返す
    {
        // ClearDrawScreen();
        float progress = static_cast<float>(i) / total;
        DrawBox((int)(barX -0.1f), (int)(barY - 0.1f), ((barX + barWidth) + 1), ((barY + barHeight) + 1), GetColor(255, 255, 255), FALSE); // 白枠の表示

        int filledWidth = static_cast<int>(barWidth * progress);
        DrawBox(barX, barY, barX + filledWidth, barY + barHeight, GetColor(180, 20, 20), TRUE); // バーの表示
        
        // ローディング文字列
        //std::string loadingText = "Loading: ";
        //loadingText += m_tasks[i]->GetTaskName();
        //DrawString(500, 360, loadingText.c_str(), GetColor(255, 255, 255)); // 読み込んでいるファイルの表示

        ScreenFlip();

        // タスク実行
        m_tasks[i]->Execute();

        // 少し待つ（UI更新時間）
        WaitTimer(100);
    }

    // 完了画面描画
    // ClearDrawScreen();
    //DrawBox(440, 400, 440 + 400, 420, GetColor(255, 255, 255), FALSE); // 白い枠（最後に表示するよう）
    DrawBox(barX, barY, (barX + barWidth), (barY + barHeight), GetColor(180, 20, 20), TRUE); // 赤いやつ

    ScreenFlip();

    WaitTimer(100); // 少し待ってから、ゲームへ
}
