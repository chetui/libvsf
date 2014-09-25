#include "sysinfo/vm/dynamic/cache_miss.h"

#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#include "framework/exception.h"

using namespace std;

constexpr const int CacheMiss::DEFAULT_LOOP_INTERVAL_MS;
constexpr const int CacheMiss::DEFAULT_SAMPLE_INTERVAL_MS;

CacheMiss::CacheMiss(): has_data_(false), loop_interval_ms_(DEFAULT_LOOP_INTERVAL_MS), sample_interval_us_(DEFAULT_SAMPLE_INTERVAL_MS), callback_func_(new cache_miss_callback_t)
{
    *callback_func_ = nullptr;
}

CacheMiss::~CacheMiss()
{
    stop();
}

CacheMiss *CacheMiss::get_instance()
{
    static CacheMiss cache_miss;
    return &cache_miss;
}

void CacheMiss::set_loop_interval(int interval_ms) 
{
    loop_interval_ms_ = interval_ms;
}

void CacheMiss::set_sample_interval(int interval_us) 
{
    sample_interval_us_ = interval_us;
}

void CacheMiss::set_callback(cache_miss_callback_t callback_func) 
{
    unique_lock<shared_timed_mutex> lock(data_mutex_);
    *callback_func_ = callback_func;
}

void CacheMiss::clear_param()
{
    loop_interval_ms_ = DEFAULT_LOOP_INTERVAL_MS;
    sample_interval_us_ = DEFAULT_SAMPLE_INTERVAL_MS;
    unique_lock<shared_timed_mutex> lock(data_mutex_);
    *callback_func_ = nullptr;;
}

std::shared_timed_mutex& CacheMiss::get_data_mutex()
{
    return data_mutex_;
}

std::atomic<bool>& CacheMiss::get_has_data()
{
    return has_data_;
}

CacheMissData CacheMiss::get_cache_miss_without_refresh(pid_t tid)
{
    while(!has_data_) 
        this_thread::sleep_for(chrono::milliseconds(10));

    shared_lock<shared_timed_mutex> lock(data_mutex_);
    auto iter = cache_miss_data_.find(tid);
    if (iter != cache_miss_data_.end())
        return iter->second;
    else
        return {tid};
}

CacheMissData CacheMiss::get_cache_miss(pid_t tid)
{
    if(!joinable())
        refresh();

    return get_cache_miss_without_refresh(tid);
}

void CacheMiss::start_watching(pid_t tid) 
{
    to_start_watching_.push_back(tid);
}

void CacheMiss::stop_watching(pid_t tid) 
{
    to_stop_watching_.push_back(tid);
}

void CacheMiss::start_sample()
{
    unique_lock<shared_timed_mutex> lock(data_mutex_);

    for (auto tid : to_start_watching_) {
        auto ret = cache_miss_data_.emplace(tid, tid);
        if (ret.second) { //insert successfully
            if (!cache_miss_data_[tid].open_fd()) { //open_fd failed
                cache_miss_data_.erase(ret.first);
                LDEBUG << "cache miss to_start_watching_ failed on tid:" << tid << ". open_fd failed.";
            }
        } else {
            LDEBUG << "cache miss insert to to_start_watching_ failed on tid:" << tid << ". already existed.";
        }
    }
    to_start_watching_.clear();

    for (auto tid : to_stop_watching_) {
        auto iter = cache_miss_data_.find(tid);
        if (iter != cache_miss_data_.end()) {
            iter->second.clear_fd();
            cache_miss_data_.erase(iter);
        } else {
            LDEBUG << "cache miss erase from to_stop_watching_ failed on tid:" << tid << ". not existed";
        }
    }
    to_stop_watching_.clear();

    for (auto& data : cache_miss_data_) {
        if (!data.second.first_read()) {
            to_stop_watching_.push_back(data.first);
            LDEBUG << "tid:" << data.first << " first_read() failed";
        }
    }

    return;
}

void CacheMiss::stop_sample()
{
    unique_lock<shared_timed_mutex> lock(data_mutex_);
    for (auto& data : cache_miss_data_) {
        if (!data.second.second_read()) {
            to_stop_watching_.push_back(data.first);
            LDEBUG << "tid:" << data.first << " second_read() failed";
        }
    }
    for (const auto& data : cache_miss_data_) {
        if (*callback_func_) {
            (*callback_func_)(data.second);
        }
    }

    return;
}

void CacheMiss::clear()
{
    for (auto& data : cache_miss_data_) {
        data.second.clear_fd();
    }
    cache_miss_data_.clear();
    to_start_watching_.clear();
    to_stop_watching_.clear();
    has_data_ = false;
}

void CacheMiss::refresh()
{
    start_sample();
    this_thread::sleep_for(chrono::microseconds(sample_interval_us_));
    stop_sample();

    has_data_ = true;

//    LDEBUG << "DDDD: sample_interval_us_: " << sample_interval_us_ << endl;
//    LDEBUG << "DDDD: loop_interval_ms_: " << loop_interval_ms_ << endl;
    return;
}

void CacheMiss::run()
{
    while(!stop_)
    {
        refresh();
        this_thread::sleep_for(chrono::milliseconds(loop_interval_ms_));
    }
    unique_lock<shared_timed_mutex> lock(data_mutex_);
    clear();
}

CacheMissData::CacheMissData():
    cache_misses(PERF_COUNT_HW_CACHE_MISSES),
    cpu_cycles(PERF_COUNT_HW_CPU_CYCLES),
    instructions(PERF_COUNT_HW_INSTRUCTIONS),
    cache_references(PERF_COUNT_HW_CACHE_REFERENCES)
{
}

CacheMissData::CacheMissData(pid_t tid):
    tid(tid),
    cache_misses(PERF_COUNT_HW_CACHE_MISSES),
    cpu_cycles(PERF_COUNT_HW_CPU_CYCLES),
    instructions(PERF_COUNT_HW_INSTRUCTIONS),
    cache_references(PERF_COUNT_HW_CACHE_REFERENCES)
{
}

CacheMissData::CacheMissData(const CacheMissData& c):
    tid(c.tid),
    cache_misses(PERF_COUNT_HW_CACHE_MISSES),
    cpu_cycles(PERF_COUNT_HW_CPU_CYCLES),
    instructions(PERF_COUNT_HW_INSTRUCTIONS),
    cache_references(PERF_COUNT_HW_CACHE_REFERENCES)
{
    mptc = c.mptc;
    mpti = c.mpti;
    mptr = c.mptr;
    rpti = c.rpti;
    cpi = c.cpi;
    cache_misses = c.cache_misses;
    cpu_cycles = c.cpu_cycles;
    instructions = c.instructions;
    cache_references = c.cache_references;
}

CacheMissData& CacheMissData::operator=(const CacheMissData& c)
{
    tid = c.tid;
    mptc = c.mptc;
    mpti = c.mpti;
    mptr = c.mptr;
    rpti = c.rpti;
    cpi = c.cpi;
    cache_misses = c.cache_misses;
    cpu_cycles = c.cpu_cycles;
    instructions = c.instructions;
    cache_references = c.cache_references;
    return *this;
}

bool CacheMissData::open_fd()
{
    if (!cache_misses.open_fd(tid))
        return false;
    if (!cpu_cycles.open_fd(tid, cache_misses.fd_)) {
        clear_data();
        cache_misses.clear_fd();
        return false;
    }
    if (!instructions.open_fd(tid, cache_misses.fd_)) {
        clear_data();
        cache_misses.clear_fd();
        cpu_cycles.clear_fd();
        return false;
    }
    if (!cache_references.open_fd(tid, cache_misses.fd_)) {
        clear_data();
        cache_misses.clear_fd();
        cpu_cycles.clear_fd();
        instructions.clear_fd();
        return false;
    }
    return true;
}

bool CacheMissData::first_read()
{
    if (cache_misses.first_read() 
        && cpu_cycles.first_read()
        && instructions.first_read()
        && cache_references.first_read()) {
        return true;
    } else {
        clear_data();
        return false;
    }
}

bool CacheMissData::second_read()
{
    if (cache_misses.second_read()
        && cpu_cycles.second_read()
        && instructions.second_read()
        && cache_references.second_read()) {
        update_miss_rate();
        return true;
    } else {
        clear_data();
        return false;
    }
}

void CacheMissData::update_miss_rate()
{
    if (cpu_cycles.count > 0) 
        mptc = cache_misses.count * 1.0 / cpu_cycles.count * 1000;
    if (cache_references.count > 0) 
        mptr = cache_misses.count * 1.0 / cache_references.count * 1000;
    if (instructions.count > 0) {
        mpti = cache_misses.count * 1.0 / instructions.count * 1000;
        rpti = cache_references.count * 1.0 / instructions.count * 1000;
        cpi = cpu_cycles.count * 1.0 / instructions.count;
    }
}

void CacheMissData::clear_fd()
{
    cache_misses.clear_fd();
    cpu_cycles.clear_fd();
    instructions.clear_fd();
    cache_references.clear_fd();
}

void CacheMissData::clear_data()
{
    cache_misses.clear_data();
    cpu_cycles.clear_data();
    instructions.clear_data();
    cache_references.clear_data();
}

CacheMissData::PerfData::PerfData(unsigned long long c)
{
    memset(&attr_, 0, sizeof(attr_));
    attr_.config = c;
    attr_.type = PERF_TYPE_HARDWARE;
    attr_.size = sizeof(attr_);
    attr_.inherit = 0;
    attr_.disabled = 1;
}

CacheMissData::PerfData::PerfData(const PerfData& pd)
{
    fd_ = pd.fd_;
    first_time_ = pd.first_time_;
    second_time_ = pd.second_time_;
    count = pd.count;
    attr_.config = pd.attr_.config;
    attr_.type = pd.attr_.type;
    attr_.size = pd.attr_.size;
    attr_.inherit = pd.attr_.inherit;
    attr_.disabled = pd.attr_.disabled;
}

CacheMissData::PerfData& CacheMissData::PerfData::operator=(const PerfData& pd)
{
    fd_ = pd.fd_;
    first_time_ = pd.first_time_;
    second_time_ = pd.second_time_;
    count = pd.count;
    attr_.config = pd.attr_.config;
    attr_.type = pd.attr_.type;
    attr_.size = pd.attr_.size;
    attr_.inherit = pd.attr_.inherit;
    attr_.disabled = pd.attr_.disabled;

    return *this;
}

bool CacheMissData::PerfData::open_fd(pid_t tid)
{
    fd_ = perf_event::perf_event_open(&attr_, tid, -1, -1, 0);
    if (fd_ < 0)
        return false;
    ioctl(fd_, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_, PERF_EVENT_IOC_ENABLE, 0);
    return true;
}

bool CacheMissData::PerfData::open_fd(pid_t tid, int fd_dep)
{
    fd_ = perf_event::perf_event_open(&attr_, tid, -1, fd_dep, 0);
    if (fd_ < 0)
        return false;
    ioctl(fd_, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_, PERF_EVENT_IOC_ENABLE, 0);
    return true;
}

bool CacheMissData::PerfData::first_read()
{

    ssize_t cnt = read(fd_, &first_time_, sizeof(first_time_));
    if (cnt < 0)
//    if (read(fd_, &first_time_, sizeof(first_time_)) < 0)
        return false;
    else {
        return true;
    }
}

bool CacheMissData::PerfData::second_read()
{
    if (read(fd_, &second_time_, sizeof(second_time_)) < 0)
        return false;
    if (second_time_ > first_time_) 
        count = second_time_ - first_time_;
    else
        count = 0;
    return true;
}

void CacheMissData::PerfData::clear_data()
{
    count = 0;
    first_time_ = 0;
    second_time_ = 0;
}

void CacheMissData::PerfData::clear_fd()
{
    ioctl(fd_, PERF_EVENT_IOC_DISABLE, 1);
    close(fd_);
}

std::ostream &operator<<(std::ostream &os, const CacheMissData&v)
{
    os << "[" 
        << v.tid << ":" 
        << v.mptc << ":" 
        << v.mpti << ":" 
        << v.mptr << ":" 
        << v.rpti << ":" 
        << v.cpi << "|" 
        << v.cache_misses.count << ":" 
        << v.cpu_cycles.count << ":" 
        << v.instructions.count << ":" 
        << v.cache_references.count
        << "]";
    return os;
}
