/*
    Author: Justin Muskopf
    Instructor: Hoffman
    Course: CSCE 4550, Fall 2018
    Assignment: Project 3
*/
#include <cstdlib>
#include <iostream>
#include "PortScanner.h"
#include "ArgParser.h"

int main(int argc, char *argv[])
{

    ArgParser parser(argc, argv);

    PortScanner portScanner(parser);

    portScanner.Scan();

    portScanner.PrintScanReport();

	return 0;
}
