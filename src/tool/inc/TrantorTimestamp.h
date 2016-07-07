#ifndef TRANTOR_TIMESTAMP_H_
#define TRANTOR_TIMESTAMP_H_

#include <sys/time.h>
#include <stdint.h>
#include <cstddef>
#include <iostream>
#include <sstream>
using namespace std;

namespace trantor
{
	class TrantorTimestamp
	{
	public:
		explicit TrantorTimestamp(int64_t time_since_epoch_usec)
		:time_since_epoch_usec_(time_since_epoch_usec)
		{
		}
		inline void operator> (const TrantorTimestamp& rhs)
		{
			time_since_epoch_usec_ = rhs.time_since_epoch_usec_;
		}
		inline bool operator> (const TrantorTimestamp& rhs) const 
		{
			return time_since_epoch_usec_ > rhs.time_since_epoch_usec_;
		}
		inline bool operator< (const TrantorTimestamp& rhs) const
		{
			return time_since_epoch_usec_ < rhs.time_since_epoch_usec_;
		}
		inline bool operator>= (const TrantorTimestamp& rhs) const 
		{
			return time_since_epoch_usec_ >= rhs.time_since_epoch_usec_;
		}
		inline bool operator<= (const TrantorTimestamp& rhs) const
		{
			return time_since_epoch_usec_ <= rhs.time_since_epoch_usec_;
		}
		inline bool operator== (const TrantorTimestamp& rhs) const 
		{
			return time_since_epoch_usec_ == rhs.time_since_epoch_usec_;
		}

		static TrantorTimestamp now();
		string transToString() const;
		timeval getTimeval() const;
		timespec getTimespec() const;

		int64_t getTimeMicroSeconds() const
		{
			return time_since_epoch_usec_;
		}

		friend inline TrantorTimestamp operator+ (const TrantorTimestamp& lhs, const int64_t interval);
		friend inline int64_t operator- (const TrantorTimestamp& lhs, const TrantorTimestamp& rhs);
		friend inline ostream& operator<< (ostream& os, const TrantorTimestamp& rhs) ;

	private:
		int64_t time_since_epoch_usec_;
	};

	inline TrantorTimestamp operator + (const TrantorTimestamp& lhs, const int64_t interval)
	{
		return TrantorTimestamp(lhs.time_since_epoch_usec_ + interval); 
	}
	inline int64_t operator - (const TrantorTimestamp& lhs, const TrantorTimestamp& rhs)
	{
		return lhs.time_since_epoch_usec_ - rhs.time_since_epoch_usec_; 
	}
	inline ostream& operator<< (ostream& os, const TrantorTimestamp& rhs)  
	{
		os<<rhs.transToString();
		return os;
	}
}

#endif
