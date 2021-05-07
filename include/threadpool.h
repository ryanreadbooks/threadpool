#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <pthread.h>
#include <memory>

#include "lock_gurad.h"

using std::string;
using std::vector;
using std::queue;
using std::shared_ptr;
using std::deque;

/**
 * 线程中需要跑的任务，里面定义一个虚函数run
 * run里面执行任务内容,现在暂时不往run里面传参数
*/
class Task {
    private:
        string name_;
    
    public:
        Task(const string& name) : name_(name) {}
        virtual ~Task() {}; // 注意要定义虚析构，以通过父类指针释放子类资源

    public:
        string get_name() const {return name_;}
        void set_name(const string& name) {name_ = name;}
        virtual void run() = 0;
};


/**
 * 线程池类的原型
*/
class ThreadPool {
    private:
        typedef pthread_t Thread;
        typedef pthread_mutex_t Lock;
        typedef pthread_cond_t Condition;
    
    private:
        // queue<Task*> task_queue_;    // 任务队列
        deque<Task*> task_queue_;    // 任务队列
        vector<Thread> threads_; // 线程集合
        volatile bool running_;  // 线程池是否正在运行
        Lock lock_;
        Condition cond_;

    public:
        ThreadPool(size_t init_size = 10);
        ~ThreadPool();

    private:
        ThreadPool& operator=(const ThreadPool&) = delete;
        ThreadPool(const ThreadPool&) = delete;

    public:
        int add_task(Task*);    // 添加任务进任务队列内
        void stop();    // 停止线程池
        bool running();
        int task_size();
        int size();

    private:
        void create_threads(size_t size);
        Task* dispatch();   // 分发任务
        static void* do_job(void*);  // 每个线程都执行的函数，里面执行从任务队列里面取出的Task，必须为static的
};


#endif