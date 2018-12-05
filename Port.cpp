#include <iostream>
#include "Port.h"


void Port::print()
{
    std::cout << "Port: " << number << "\n";
    std::cout << "  Service : " << service << "\n";
    std::cout << "  Protocol: " << protocol << "\n";
}
