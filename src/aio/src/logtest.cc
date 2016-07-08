#include "FdLog.h"

int main()
{
	setLogLevel(DEBUG);
	for (int i = 0; i < 100; i++)
	{
		log(DEBUG, "hello fd");
	}
}
