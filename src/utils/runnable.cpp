#include "utils/runnable.h"

#include <cassert>

//#include "utils/logstream.h"

using namespace std;

Runnable::Runnable():
    stop_(true), thread_(nullptr) 
{}

Runnable::~Runnable()
{
    /*if (!stop_) {
        try {
            stop();
        } catch (const exception& e) {
            LOG(LogLevel::warn) << "Thread stop exception " << e.what() << endl;
        }
    }*/     
    delete thread_;
}

void Runnable::join()
{
    thread_->join();
}

bool Runnable::joinable() 
{
    return thread_ != nullptr && thread_->joinable();
}

void Runnable::start()
{
    if (!stop_) {
//        LOG(LogLevel::err) << "Runnable::start: "
//            << "Thread is running" << endl;
        return;
    }
    if (thread_ != nullptr) {
        delete thread_;
        thread_ = nullptr;
    }
    stop_ = false;
    thread_ = new thread(&Runnable::run, this);
}

void Runnable::stop()
{
    if (stop_) {
        return;
    }
    stop_ = true;
    if (thread_ != nullptr && thread_->joinable()) {
        thread_->detach();
    } 
}
