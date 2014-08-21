#ifndef _LOCK_PROXY_H_
#define _LOCK_PROXY_H_

#include <mutex>
#include <shared_mutex>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <iostream>

template <typename T>
class has_key_type {
    typedef char _a;
    struct _b { _a x[2]; };

    template <typename U>
        static _a foo( U const &, typename U::key_type* p = 0 );
    static _b foo( ... );
    static T& generate_ref();
public:
    static const bool value = sizeof(foo(generate_ref())) == sizeof(_a);
};

template <typename C>
class ReadWriteLock {
public:
    ReadWriteLock() {
        mutex_ = &backup_mutex_;
    }
    ReadWriteLock(std::shared_timed_mutex* mutex): mutex_(mutex) {
    }
    ~ReadWriteLock() {
    }

    struct ReadProxy{
        ReadProxy(const C& v, std::shared_timed_mutex& m) : value_(v), lock_(m) { }
        ~ReadProxy() { }
        const C& value_;
        std::shared_lock<std::shared_timed_mutex> lock_;
        const C& get() const { return value_; }
    };
    struct WriteProxy{
        WriteProxy(C& v, std::shared_timed_mutex& m) : value_(v), lock_(m) { }
        ~WriteProxy() { }
        C& value_;
        std::unique_lock<std::shared_timed_mutex> lock_;
        C& get() { return value_; }
    };

    const C& read() const { 
        return ReadProxy(value_, *mutex_).get();
    }

    C& write() { 
        return WriteProxy(value_, *mutex_).get();
    }

    std::shared_lock<std::shared_timed_mutex>& read_lock() {
        std::shared_lock<std::shared_timed_mutex>* rl = new std::shared_lock<std::shared_timed_mutex>(*mutex_);
        return *rl;
    }

    std::unique_lock<std::shared_timed_mutex>& write_lock() {
        std::unique_lock<std::shared_timed_mutex>* wl = new std::unique_lock<std::shared_timed_mutex>(*mutex_);
        return *wl;
    }

    //for associative container (e.g. set/map)
    template <class CC = C>
    typename std::enable_if< has_key_type<CC>::value, typename CC::value_type >::type
    find(typename CC::key_type const & key, const typename CC::value_type failed_value)
    {
        std::shared_lock<std::shared_timed_mutex> lock(*mutex_);
        auto iter = value_.find(key);
        if (iter != value_.end())
            return *iter;
        else
            return failed_value;
    }

    //for non-associative container (e.g. vector/list/string)
    template <class CC = C>
    typename std::enable_if< !has_key_type<CC>::value, typename CC::value_type >::type
    find(typename CC::value_type const & key, const typename CC::value_type failed_value)
    {
        std::shared_lock<std::shared_timed_mutex> lock(*mutex_);
        auto iter = std::find(value_.begin(), value_.end(), key);
        if (iter != value_.end())
            return *iter;
        else
            return failed_value;
    }

    C value_;
private:
    std::shared_timed_mutex* mutex_;
    mutable std::shared_timed_mutex backup_mutex_;
};

class MultiLock {
public:
    template<typename... Args>
    MultiLock(Args&... args) {
        std::cout << "multi lock all\n";
        std::lock(std::forward<Args&>(args)...);
        add_to(std::forward<Args&>(args)...);
    }
    ~MultiLock() {
        for (auto& l : unique_locks_) 
        {
            std::cout << "multi write unlock one\n";
            delete l;
        }
        for (auto& l : shared_locks_) 
        {
            std::cout << "multi read unlock one\n";
            delete l;
        }
    }
private:
    void add_to() {} //termination version
    template<typename T, typename... Args>
    void add_to(T& t, Args&... args) {
        add_to_locks(&t);
        add_to(args...);
    }
    void add_to_locks(std::unique_lock<std::shared_timed_mutex>* l) {
        unique_locks_.push_back(l);
    }
    void add_to_locks(std::shared_lock<std::shared_timed_mutex>* l) {
        shared_locks_.push_back(l);
    }
    std::vector<std::unique_lock<std::shared_timed_mutex>* > unique_locks_;
    std::vector<std::shared_lock<std::shared_timed_mutex>* > shared_locks_;
};

#endif
