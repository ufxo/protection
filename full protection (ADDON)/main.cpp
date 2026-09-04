#include <iostream>

#include "handles.h"
#include "encryption.h"

#include "Windows.h"

int main()
{
    initHandlesThread();

    std::cout << "\n" << "handles are nice" << "\n";

    std::string f = "";
    std::cin >> f;
    return 0;
}