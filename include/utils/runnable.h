#ifndef _RUNNABLE_H_
#define _RUNNABLE_H_

#include <atomic>
#include <thread>

/**
 * Java-like interface for std::thread.
 */
class Runnable
{
public:
    Runnable();
    virtual ~Runnable(); 

    /**
     * Set stop_ to false and destroy the thread.
     * MUST stop the thread when called.
     */
    void stop();
    void start(); 
    /**
     * Wait function run() to stop.
     */
    void join();
    /**
     * If it is still running and not detached.
     */
    bool joinable(); 

protected:
    virtual void run() = 0;

    /**
     * true if stop is called.
     */
    std::atomic<bool> stop_;

private:
    /**
     * using a pointer to prevent the inconsistency 
     * when a runnable is distructing
     */
    std::thread* thread_;
};

#endif  // _UTILS_RUNNABLE_H_
