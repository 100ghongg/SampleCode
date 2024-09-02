#pragma once
#include "Default.h"

template<typename T>
class ThreadWrapper
{
public:
    class Vars {
    public:
        std::atomic<bool> runnig{true};
        std::condition_variable* condvar;
        std::queue<T>  jobs;
        std::mutex*    job_queue_lock;
        void*         conn{ nullptr };
        Vars() {};
        Vars(const Vars&) = delete;
        Vars& operator=(const Vars&) = delete;
    };
public:
    int     InsertJob(const T* job);
    int     GetJobCount() { 
                if (auto lock = std::unique_lock{var_.job_queue_lock, std::try_to_lock}) {return var_.jobs.size(); }
                else return -1; 
            };
    bool    SetRunningFlag(bool flag) { var_.runnig.store(flag, std::memory_order_release); return var_.runnig; };
    int     InitThread(std::function<void(int idx, Vars* var)> worker);
    void    EndThread() { SetRunningFlag(false); tr_.join(); };
    void    SetJobMutex(std::mutex* lock) { var_.job_queue_lock = lock; };
    void    SetCondVar(std::condition_variable* cv) { var_.condvar = cv; }
    void    SetJobQueue(std::queue<T>* q) { var_.jobs = *q; };
    void    SetConnection(void* conn) { var_.conn = conn; }

private:
    int         idx_ = 0;
    Vars        var_;
    std::thread tr_;

public:
    ThreadWrapper() = delete;
    ThreadWrapper(const ThreadWrapper&) = delete;
    ThreadWrapper& operator=(const ThreadWrapper&) = delete;
    explicit ThreadWrapper(int id) : idx_(id)  {};
    explicit ThreadWrapper(int id, Vars& var) : idx_(id), var_(var)  {};
    ~ThreadWrapper() {};
};

template<typename T>
using Worker = ThreadWrapper<T>;

template <typename T>
int ThreadWrapper<T>::InitThread(std::function<void(int idx, Vars* var)> worker) {
    SetRunningFlag(true);
    tr_ = std::thread(worker, idx_, &var_);
    return idx_;
}
