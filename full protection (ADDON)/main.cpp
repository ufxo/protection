#include <iostream>

#include "checks/handles.h"
#include "checks/clock.h"
#include "checks/debugger.h"
#include "globals.h"

#include "Windows.h"

int main()
{
    if(globals::devMode)
        std::cout << "[!!!] DEV MODE IS ON [!!!]\n\n";

    initHandlesThread();
    initClockCheck();
    if(!globals::devMode)
        initAntiDebugger();

    std::cout << "\n" << "Handle check passed." << "\n";

    std::string f = "";
    std::cin >> f;
    return 0;
}