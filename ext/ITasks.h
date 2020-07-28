#pragma once

#include <queue>

#include "common/ICriticalSection.h"
#include "skse64/gamethreads.h"

class TaskQueueBase
{
public:
    void AddTask(TaskDelegate* task)
    {
        m_taskLock.Enter();
        m_taskQueue.push(task);
        m_taskLock.Leave();
    }

    inline bool IsTaskQueueEmpty()
    {
        m_taskLock.Enter();
        bool r = m_taskQueue.size() == 0;
        m_taskLock.Leave();
        return r;
    }

    virtual void ProcessTasks() = 0;

protected:
    std::queue<TaskDelegate*> m_taskQueue;
    ICriticalSection m_taskLock;

};

class TaskQueue :
    public TaskQueueBase
{
public:

    virtual void ProcessTasks()
    {
        while (!IsTaskQueueEmpty())
        {
            m_taskLock.Enter();
            auto task = m_taskQueue.front();
            m_taskQueue.pop();
            m_taskLock.Leave();

            task->Run();
            task->Dispose();
        }
    }

};

class TaskQueueUnsafe :
    public TaskQueueBase
{
public:

    virtual void ProcessTasks()
    {
        m_taskLock.Enter();

        while (m_taskQueue.size())
        {
            auto task = m_taskQueue.front();
            m_taskQueue.pop();

            task->Run();
            task->Dispose();
        }

        m_taskLock.Leave();
    }

    void ClearTasks()
    {
        m_taskLock.Enter();
        while (m_taskQueue.size()) {
            auto task = m_taskQueue.front();
            m_taskQueue.pop();

            task->Dispose();
        }
        m_taskLock.Leave();
    }
};

class TaskDelegateFixed
{
public:
    virtual void Run() = 0;
};

class ITask
{
    typedef void (*RTTaskEnter_t)(void);
public:
    static void AddTaskFixed(TaskDelegateFixed* cmd);
    static void AddTask(TaskDelegate* cmd);

    static bool Initialize();
private:
    ITask() = default;


    static void RTTaskEnter_Hook();

    static std::vector<TaskDelegateFixed*> s_tasks_fixed;
    static TaskQueue s_tasks;

    static RTTaskEnter_t RTTaskEnter_O;
};