#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Object3D.h"

// ILoadTaskインターフェース
// ILoadTaskはタスクの基底クラス
class ILoadTask //: public Object3D // 追加 ★
{
public:
	virtual ~ILoadTask() = default;
	virtual int Execute() = 0;
	virtual const char* GetTaskName() const = 0;
	virtual int GetHandle() const { return -1; }
};

// サウンド読み込みタスク
class LoadSoundTask : public ILoadTask
{
public:
	explicit LoadSoundTask(const char* path);
	int Execute() override;
	const char* GetTaskName() const override;
	int GetHandle() const;

private:
	const char* m_path;
	int m_handle;
};

// 初期化タスク
class InitializeSoundManagerTask : public ILoadTask
{
public:
	InitializeSoundManagerTask();
	int Execute() override;
	const char* GetTaskName() const override;
};

class InitializeSceneManagerTask : public ILoadTask
{
public:
	InitializeSceneManagerTask();
	int Execute() override;
	const char* GetTaskName() const override;
};

// ローディングマネージャー
class LoadingManager
{
private:
	std::vector<std::unique_ptr<ILoadTask>> tasks;
	std::vector<std::unique_ptr<ILoadTask>> m_tasks;

	//bool mbNowLoading = false; // 今ローディング中か

public:
	void AddTask(std::unique_ptr<ILoadTask> task);

	void ExecuteAll();

	// ここを追加
	const std::vector<std::unique_ptr<ILoadTask>>& GetTasks() const
	{
		return tasks;
	}
};