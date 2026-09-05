#include <iostream>

#include "handles.h"
#include "clock.h"

#include "Windows.h"

int main()
{
    initHandlesThread();
    initClockCheck();

    std::cout << "\n" << "Handle check passed." << "\n";

    std::string f = "";
    std::cin >> f;
    return 0;
}