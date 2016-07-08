#include "TrantorTimestamp.h"

namespace trantor
{
	string TrantorTimestamp::transToString() const
	{
		struct tm tm_time;
		int64_t seconds = time_since_epoch_usec_ / 1000000;
		gmtime_r(&seconds, &tm_time);

		string date;
		string time;
		stringstream sstr;
		sstr<<(tm_time.tm_year + 1900)<<"-"<<(tm_time.tm_mon + 1)<<"-"<<tm_time.tm_mday<<" ";
		sstr>>date;

		sstr<<tm_time.tm_hour<<":"<<tm_time.tm_min<<":"<<tm_time.tm_sec<<".";
		sstr<<time_since_epoch_usec_ - seconds * 1000000;
		sstr>>time;

		return date + " " + time;
	}
	timeval TrantorTimestamp::getTimeval() const
	{
		timeval tv;
		tv.tv_sec = time_since_epoch_usec_/1000000;
		tv.tv_usec = time_since_epoch_usec_ - tv.tv_sec * 1000000;
		return tv;
	}
	timespec TrantorTimestamp::getTimespec() const
	{
		timespec ts;
		ts.tv_sec = time_since_epoch_usec_/1000000;
		ts.tv_nsec = time_since_epoch_usec_ * 1000 - ts.tv_sec * 1000000000;
		return ts; 
	}

	TrantorTimestamp TrantorTimestamp::now()
	{
		timeval tv;
		if(gettimeofday(&tv, NULL) == 0)
		{
			return TrantorTimestamp(tv.tv_sec*1000000 + tv.tv_usec);
		}
	}
}

