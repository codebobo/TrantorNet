#include "TimeWheel.h"
#include <muduo/base/Logging.h>
#include <functional>

TimeWheel::TimeWheel(double interval, long event_bucket_queue_size):
interval_(interval),
timer_(TrantorTimerLoop::Instance())
{
	event_bucket_queue_.resize(event_bucket_queue_size);
    timer_.runEvery(interval_, bind(&TimeWheel::onTimer, this));
}

void TimeWheel::runAfter(double delay, function<void()> cb, int64_t event_uuid)
{
    uint32_t bucketIndexToPush;
    uint32_t bucketNum = uint32_t(delay / interval_) + 1;
	uint32_t queue_size = event_bucket_queue_.size();

	if (bucketNum >= queue_size)
	{
		bucketIndexToPush = queue_size - 1;
	} 
	else
	{
		bucketIndexToPush = (bucketNum + queue_size) % queue_size;
	}

	auto f = [=]()
	{
		cb();
		//lock_guard<mutex> lock(mtx_);
		if(weak_entry_map_.find(event_uuid) != weak_entry_map_.end())
		{
			WeakEntryPtr weak_ptr = weak_entry_map_[event_uuid];
			if(!weak_ptr.lock())
			{
				weak_entry_map_.erase(event_uuid);
			}
		}
	};

	EntryPtr entry_ptr;
	lock_guard<mutex> lock(mtx_);
	if(weak_entry_map_.find(event_uuid) != weak_entry_map_.end())
	{
		entry_ptr = weak_entry_map_[event_uuid].lock();
	}
	else
	{
		entry_ptr = make_shared<Entry>(f);
		weak_entry_map_[event_uuid] = WeakEntryPtr(entry_ptr);
	}
	event_bucket_queue_[bucketIndexToPush].push_back(entry_ptr);
}

void TimeWheel::runAfter(double delay, function<void()> cb)
{
    uint32_t bucketIndexToPush = 0;
    uint32_t bucketNum = uint32_t(delay / interval_) + 1;
	uint32_t queue_size = event_bucket_queue_.size();

	if (bucketNum >= queue_size)
	{
		bucketIndexToPush = queue_size - 1;
	} 
	else
	{
		bucketIndexToPush = (bucketNum + queue_size) % queue_size;
	}
	EntryPtr entry_ptr = make_shared<Entry>(cb);
	lock_guard<mutex> lock(mtx_);
	event_bucket_queue_[bucketIndexToPush].push_back(entry_ptr);
}

void TimeWheel::onTimer()
{
	EventBucket tmp;
	{
		lock_guard<mutex> lock(mtx_);
		tmp = event_bucket_queue_.front();
		event_bucket_queue_.pop_front();
		event_bucket_queue_.push_back(EventBucket());
	}
}
