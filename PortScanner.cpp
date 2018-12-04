#include "PortScanner.h"
#include <fstream>

PortScanner::PortScanner()
{
}


PortScanner::~PortScanner()
{
}

Port PortScanner::getPortFromLine(std::string line)
{
    Port port;
    std::string numberString;

    port.service << line;

    numberString << line;
    port.number = std::stoi(numberString);

    port.protocol << line;

    port.state = CLOSED;

    return port;
}

void PortScanner::getPortServicesFromFile(std::string filename)
{
    ifstream file(filename);
    if (file.fail())
    {
        std::cout << "Error opening " << filename << "... Port services will be unavailable.\n";
    }

    std::string line;
    while (getline(file, line))
    {
        Port port = getPortFromLine(line);
        


    }

}
