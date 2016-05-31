#ifndef TRANTOR_SINGLETON_H_
#define TRANTOR_SINGLETON_H_

#include <pthread.h>
#include "TrantorTimer.h"

namespace trantor
{
	template <typename T>
	class TrantorSingleton
	{
	public:
		static T& Instance()
		{
			pthread_once(&once_, &TrantorSingleton::Init);
			return *value_;
		}

	private:
		TrantorSingleton();
		~TrantorSingleton();
		static void Init()
		{
			if(!value_)
			{
				value_ = new T();
			}
		}
		static T* value_;
		static pthread_once_t once_;
	};

	template <typename T>
	pthread_once_t TrantorSingleton<T>::once_ = PTHREAD_ONCE_INIT;

	template <typename T>
	T* TrantorSingleton<T>::value_ = NULL;
}

#endif
