#pragma once

#include <iostream>
#include <cstdio>
#include <queue>
#include <vector>
#include <unistd.h>
#include "Mutex.hpp"
#include "Cond.hpp"
#include "Thread.hpp"

// 单例线程池 - 懒汉模式
const static int defaultthreadnum = 10; // for debug

template <class T>
class ThreadPool
{
private:
    bool QueueIsEmpty()
    {
        return _q.empty();
    }
    void Routine(const std::string &name)
    {
        while (true)
        {
            // 把任务从线程获取到线程私有！临界区 -> 私有的栈
            T t;
            {
                LockGuard lockguard(&_lock);
                while (QueueIsEmpty() && _is_running)
                {
                    _wait_thread_num++;
                    _cond.Wait(_lock);
                    _wait_thread_num--;
                }
                if (!_is_running && QueueIsEmpty())
                {
                    LOG(LogLevel::INFO) << " 线程池退出 && 任务队列为空, " << name << " 退出";
                    break;
                }
                // 队列中一定有任务了!, 但是
                // 1. 线程池退出 -- 消耗历史
                // 2. 线程池没有退出 -- 正常工作
                t = _q.front();
                _q.pop();
            }
            t(); // 规定，未来的任务，必须这样处理！，处理任务需要再临界区内部进行吗？1 or 0            
        }
    }
    ThreadPool(int threadnum = defaultthreadnum)
        : _threadnum(threadnum), _is_running(false), _wait_thread_num(0)
    {
        for (int i = 0; i < _threadnum; i++)
        {
            // 方法1:
            // auto f = std::bind(hello, this);
            // 方法2
            std::string name = "thread-" + std::to_string(i + 1);
            _threads.emplace_back([this](const std::string &name)
                                  { this->Routine(name); }, name);

            // Thread t([this](){
            //     this->hello();
            // }, name);
            // _threads.push_back(st::move(t));
        }
        LOG(LogLevel::INFO) << "thread pool obj create success";
    }
    ThreadPool<T> &operator=(const ThreadPool<T> &) = delete;
    ThreadPool(const ThreadPool<T> &) = delete;

public:
    void Start()
    {
        if (_is_running)
            return;
        _is_running = true;
        for (auto &t : _threads)
        {
            t.Start();
        }
        LOG(LogLevel::INFO) << "thread pool running success";
    }

    // 核心思想：我们应该让线程走正常的唤醒逻辑退出
    // 线程池要退出
    // 1. 如果被唤醒 && 任务队列没有任务 = 让线程退出
    // 2. 如果被唤醒 && 任务队列有任务 = 线程不能立即退出，而应该让线程把任务处理完，在退出
    // 3. 线程本身没有被休眠，我们应该让他把他能处理的任务全部处理完成， 在退出
    // 3 || 2 -> 1
    // 如果任务队列有任务，线程是不会休眠的!
    void Stop()
    {
        if (!_is_running)
            return;
        _is_running = false;
        if (_wait_thread_num)
            _cond.NotifyAll();

        // 这种做法不推荐
        // if (!_is_running)
        //     return;
        // _is_running = false;
        // for (auto &t : _threads)
        // {
        //     t.Stop();
        // }
        // LOG(LogLevel::INFO) << "thread pool stop success";
    }
    void Wait()
    {
        for (auto &t : _threads)
        {
            t.Join();
        }
        LOG(LogLevel::INFO) << "thread pool wait success";
    }
    void Enqueue(const T &t)
    {
        if (!_is_running)
            return;
        {
            LockGuard lockguard(&_lock);

            _q.push(t);
            if (_wait_thread_num > 0)
                _cond.NotifyOne();
        }
    }
    // debug
    static std::string ToHex(ThreadPool<T> *addr)
    {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%p", addr);
        return buffer;
    }
    // 获取单例 ？？
    static ThreadPool<T> *GetInstance()
    {
        // A, B, c
        {
            // 线程安全，提高效率式的获取单例
            if (!_instance)
            {
                LockGuard lockguard(&_singleton_lock);
                if (!_instance)
                {
                    _instance = new ThreadPool<T>();
                    LOG(LogLevel::DEBUG) << "线程池单例首次被使用，创建并初始化, addr: " << ToHex(_instance);

                    _instance->Start();
                }
            }
        }
        // else
        // {
        //     LOG(LogLevel::DEBUG) << "线程池单例已经存在,直接获取, addr: " << ToHex(_instance);
        // }
        return _instance;
    }
    ~ThreadPool()
    {
    }

private:
    // 任务队列
    std::queue<T> _q; // 整体使用的临界资源

    // 多个线程
    std::vector<Thread> _threads; // 1. 创建线程对象 2. 让线程对象启动
    int _threadnum;
    int _wait_thread_num;

    // 保护机制
    Mutex _lock;
    Cond _cond;

    // 其他属性
    bool _is_running;

    // 单例中静态指针
    // volatile static ThreadPool<T> *_instance;
    static ThreadPool<T> *_instance;

    static Mutex _singleton_lock;
};

template <class T>
ThreadPool<T> *ThreadPool<T>::_instance = nullptr;

template <class T>
Mutex ThreadPool<T>::_singleton_lock;