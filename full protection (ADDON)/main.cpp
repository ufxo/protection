#include <iostream>

#include "handles.h"
#include "encryption.h"

#include "Windows.h"

int main()
{
    initHandlesThread();

    std::cout << "\n" << "Handle check passed." << "\n";

    std::string f = "";
    std::cin >> f;
    return 0;
}