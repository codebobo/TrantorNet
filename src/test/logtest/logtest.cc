#include <thread>
#include "FdLog.h"
using namespace trantor;


void test()
{
	for (int i = 0; i < 1; i++)
	{
		log_info<<"this is info test";
		log_debug<<"this is debug test";
		log_warn<<"this is warn test";
		log_error<<"this is error test";
		log_fatal<<"this is fatal test";
	}
}

int main()
{
	//setAsyncLogFileBasename("trantornet");
	//setAsyncLogPath("/app/boboWorkdir/trantornet/TrantorNet/");
	setAsyncLogLevel(DEBUG);
	//std::thread thread1(test);
	//std::thread thread2(test);
	//std::thread thread3(test);
	//std::thread thread4(test);
	//std::thread thread5(test);
	test();
	//thread1.join();
	//thread2.join();
	//thread3.join();
	//thread4.join();
	//thread5.join();
	//sleep(5);
}
