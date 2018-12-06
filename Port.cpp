/*
    Author: Justin Muskopf
    Instructor: Hoffman
    Course: CSCE 4550, Fall 2018
    Assignment: Project 3
*/
#include <iostream>
#include "Port.h"


void Port::print()
{
    std::cout << "Port: " << number << "\n";
    std::cout << "  Service : " << service << "\n";
    std::cout << "  Protocol: " << protocol << "\n";
}
