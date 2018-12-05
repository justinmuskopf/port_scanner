#include <cstdlib>
#include <iostream>
#include "PortScanner.h"
#include "ArgParser.h"

int main(int argc, char *argv[])
{

    ArgMap args = ArgParser::ParseArgs(argc, argv);

    PortScanner portScanner(args);

	return 0;
}
