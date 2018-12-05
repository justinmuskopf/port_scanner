#include <cstdlib>
#include <iostream>
#include "PortScanner.h"
#include "ArgParser.h"

int main(int argc, char *argv[])
{

    ArgParser parser(argc, argv);

    PortScanner portScanner(parser);

	return 0;
}
