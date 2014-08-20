#ifndef _LOCK_PROXY_H_
#define _LOCK_PROXY_H_

#include <mutex>
#include <shared_mutex>
#include <algorithm>
#include <type_traits>

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
struct ReadWriteLock {
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
        return ReadProxy(value_, mutex_).get();
    }

    C& write() { 
        return WriteProxy(value_, mutex_).get();
    }

    //for associative container (e.g. set/map)
    template <class CC = C>
    typename std::enable_if< has_key_type<CC>::value, typename CC::value_type >::type
    find(typename CC::key_type const & key, const typename CC::value_type failed_value)
    {
        std::shared_lock<std::shared_timed_mutex> lock(mutex_);
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
        std::shared_lock<std::shared_timed_mutex> lock(mutex_);
        auto iter = std::find(value_.begin(), value_.end(), key);
        if (iter != value_.end())
            return *iter;
        else
            return failed_value;
    }

    mutable std::shared_timed_mutex mutex_;
    C value_;
};


#endif
