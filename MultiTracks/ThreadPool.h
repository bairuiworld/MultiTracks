#ifndef __MULTITRACKS_TREADPOOL_H__
#define __MULTITRACKS_TREADPOOL_H__

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace mt
{

class Task
{
public:
	Task(std::function<void()> task_) : task(task_), state(State::waiting) {}

	void operator()();

	enum class State { waiting, running, canceled, finished };

	State GetState();
	bool Cancel();

private:
	void SetState(State state);

private:
	std::function<void()> task;
	std::mutex state_mutex;
	State state;
};

template <class Ret>
class TaskResult : public Task
{
public:
	template <class T>
	TaskResult(std::function<void()> task, T res_) :
		Task(task), res(res_->get_future())
	{}

	const std::future<Ret>& GetFuture() { return res; }

private:
	std::future<Ret> res;
	std::shared_ptr<Task> task;
};

class ThreadPool
{
public:
    ThreadPool(size_t);
    template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args)->std::shared_ptr<TaskResult<typename std::result_of<F(Args...)>::type>>;
    ~ThreadPool();

private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // the task queue
    std::queue<std::shared_ptr<Task>> tasks;
    
    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

inline void Task::operator()()
{
	SetState(State::running);
	task();
	SetState(State::finished);
}

inline Task::State Task::GetState()
{
	std::unique_lock<std::mutex> lock(state_mutex);
	return state;
}

inline void Task::SetState(State state_)
{
	std::unique_lock<std::mutex> lock(state_mutex);
	state = state_;
}

inline bool Task::Cancel()
{
	std::unique_lock<std::mutex> lock(state_mutex);
	if(state == State::running) return false;
	state = State::canceled;
	return true;
}

 
// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads) :
    stop(false)
{
    for(size_t i = 0;i<threads;++i)
    {
		workers.emplace_back(
            [this]
            {
                for(;;)
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    while(!this->stop && this->tasks.empty())
                        this->condition.wait(lock);
                    if(this->stop && this->tasks.empty())
                        return;
                    std::shared_ptr<Task> task(this->tasks.front());
                    this->tasks.pop();
                    lock.unlock();
					if(task->GetState() == Task::State::waiting)
						(*task)();
                }
            }
        );
	}
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::shared_ptr<TaskResult<typename std::result_of<F(Args...)>::type>>
{
    typedef typename std::result_of<F(Args...)>::type return_type;
    
    // don't allow enqueueing after stopping the pool
    if(stop)
        throw std::runtime_error("enqueue on stopped ThreadPool");

    auto fn = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
	std::shared_ptr<TaskResult<return_type>> task;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
		task = std::make_shared<TaskResult<return_type>>([fn](){ (*fn)(); }, fn);
        tasks.push(task);
    }
    condition.notify_one();
    return task;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(size_t i = 0;i<workers.size();++i)
        workers[i].join();
}

}

#endif // !__MULTITRACKS_TREADPOOL_H__
