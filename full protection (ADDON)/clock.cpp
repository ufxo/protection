#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>

void clockCheck()
{
	while (true)
	{
		const auto now = std::chrono::system_clock::now();
		const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			now.time_since_epoch()
		).count();

		auto zero = ms - ms;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		const auto nowNew = std::chrono::system_clock::now();
		const auto msNew = std::chrono::duration_cast<std::chrono::milliseconds>(
			nowNew.time_since_epoch()
		).count();

		auto onehundred = msNew - ms;

		if (onehundred < 101)  // if u have a super bad PC its over
			std::cout << "CALL: exit() called due to time mistmatch in clock.cpp";
	}
}

void initClockCheck()
{
	std::thread clockThread(clockCheck);
	clockThread.detach();
}