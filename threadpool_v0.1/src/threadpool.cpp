#include <sstream>
#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <algorithm>

#include "threadpool.h"

using namespace std;

ThreadPool::ThreadPool(size_t init_size) :running_(true), 
        lock_(PTHREAD_MUTEX_INITIALIZER), 
        cond_(PTHREAD_COND_INITIALIZER) 
{
    threads_.reserve(init_size);
    // 初始化创建线程
    this->create_threads(init_size);
}


/**
 * 线程池的析构函数
 * 需要回收所有所使用的资源
*/
ThreadPool::~ThreadPool() {
    this->stop();
    // 任务也要delete掉
    // 任务队列可能已经为空
    for (deque<Task*>::iterator it = task_queue_.begin(); it != task_queue_.end(); ++it) {
        delete *it;
    }
    task_queue_.clear();
}


/**
 * 任务队列中添加新任务，返回当前任务队列的任务数
*/
int ThreadPool::add_task(Task* task) {
    // 往任务队列当中添加任务
    {
        LockGuard(this->lock_);
        this->task_queue_.push_back(task);
    }
    
    // pthread_mutex_lock(&lock_);
    // this->task_queue_.push_back(task);
    // pthread_mutex_unlock(&lock_);
    
    // 唤醒一个阻塞在条件变量上的线程去执行任务
    pthread_cond_signal(&cond_);
    return task_queue_.size();
}


/**
 * 手动停掉线程池
*/
void ThreadPool::stop() {
    if (!this->running_) {
        return;
    }
    {
        LockGuard(this->lock_);
        this->running_ = false;
    }

    // 唤醒所有线程，并且等待他们结束后回收
    pthread_cond_broadcast(&(this->cond_));
    for (auto thread: this->threads_) {
        pthread_join(thread, NULL);
    }

    this->threads_.clear();

    // 销毁锁和条件变量
    pthread_mutex_destroy(&this->lock_);
    pthread_cond_destroy(&this->cond_);
}


/**
 * 一次性创建size个线程
*/
void ThreadPool::create_threads(size_t s) {
    assert(running_ == true);
    int ret;
    for (size_t i = 0; i < s; i ++) {
        Thread tid;
        ret = pthread_create(&tid, NULL, do_job, this);
        this->threads_.push_back(tid);
        if (ret != 0) {
            // 创建某一个子线程失败，暂时不处理，仅打印信息
            stringstream ss;
            ss << "create thread " << i << " failed;";
            perror(ss.str().c_str());
        }
    }
}


/**
 * 从任务队列中取出任务并且下发
*/
Task* ThreadPool::dispatch() {
    Task* task = nullptr;

    while(!task) {
        // 任务队列为空时阻塞在条件变量上
        pthread_mutex_lock(&lock_);
        while (running_ && task_queue_.empty()) {
            pthread_cond_wait(&cond_, &lock_);
        }
        if (!running_) {
            pthread_mutex_unlock(&lock_);
            break;
        }
        else if (task_queue_.empty()) {
            pthread_mutex_unlock(&lock_);
            continue;
        }
        // 任务队列中有任务可以拿
        assert(!task_queue_.empty());
        task = task_queue_.front();
        task_queue_.pop_front();
        pthread_mutex_unlock(&lock_);
    }

    return task;
}


/**
 * 每个线程在后台正在跑的函数
 * 如果没有任务，则一直阻塞在条件变量上
 * 如果有任务，就去执行Task的run函数中的任务
 * 这是一个static函数
*/
void* ThreadPool::do_job(void* args) {
    // 每个线程一创建出来就执行的函数，当没有拿到任务时，线程就在条件变量上面阻塞
    ThreadPool* pool = static_cast<ThreadPool*>(args);
    // 当线程池正在运行，但是任务队列为空时，线程就等着
    // 只要线程池一直在运行，那么池中的所有线程就需要尝试去拿任务
    while (pool->running()) {
        // 等待任务下发
        Task* task = pool->dispatch();
        if (!task) {
            break;
        }
        
        assert(task);
        printf("thread-%lu took task %s\n", pthread_self(), task->get_name().c_str());
        task->run();
        delete task;    // 任务执行完了，释放堆区资源
        // 这个线程的任务执行完了，继续从任务队列中拿任务
    }
};


int ThreadPool::task_size() {
    LockGuard(this->lock_);
    int size = task_queue_.size();
    return size;
}


int ThreadPool::size() {
    LockGuard(this->lock_);
    int size = threads_.size();
    return size;
}


bool ThreadPool::running() {
    LockGuard(this->lock_);
    bool ret = running_;
    return ret;
}