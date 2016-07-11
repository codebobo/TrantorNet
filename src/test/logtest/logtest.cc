#include <thread>
#include "FdLog.h"
using namespace trantor;


void test()
{
	for (int i = 0; i < 10; i++)
	{
		log_debug<<"this is debug test";
		log_info<<"this is info test";
		log_warn<<"this is warn test";
		log_error<<"this is error test";
		log_fatal<<"this is fatal test";
	}
}

int main()
{
	setAsyncLogLevel(WARN);
	std::thread thread1(test);
	std::thread thread2(test);
	std::thread thread3(test);
	std::thread thread4(test);
	std::thread thread5(test);
	thread1.join();
	thread2.join();
	thread3.join();
	thread4.join();
	thread5.join();
	sleep(1);
}
