#ifndef TIMEWHEEL_H
#define TIMEWHEEL_H

#include <queue>
#include <map>
#include <mutex>
#include <vector>
#include "TrantorTimerLoop.h"

using namespace std;
using namespace trantor;

class Entry
{
public:
    Entry(function<void()> cb):cb_(cb){}
    ~Entry()
    {
        cb_();
    }
private:
    function<void()> cb_;
};

typedef shared_ptr<Entry> EntryPtr;
typedef weak_ptr<Entry> WeakEntryPtr;

typedef std::vector<EntryPtr> EventBucket;
typedef std::deque<EventBucket> EventBucketQueue;

class TimeWheel
{
public:
    TimeWheel(double, long);
    void runAfter(double, function<void()>);
    void runAfter(double delay, function<void()> cb, int64_t event_uuid);
    void onTimer();

private:
    EventBucketQueue event_bucket_queue_;
    double interval_;
    TrantorTimerLoop& timer_;
    mutex mtx_;
    map<int64_t, WeakEntryPtr> weak_entry_map_;
};

#endif
