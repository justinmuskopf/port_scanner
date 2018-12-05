#include "PortScanner.h"
#include <fstream>
#include <iostream>
#include <sstream>

#define PORTS_FILE "ports_to_services.txt"
#define MIN_DEFAULT_PORT 1
#define MAX_DEFAULT_PORT 1024

PortScanner::PortScanner(ArgMap argMap)
{
    servicesAvailable = true;

    getPortServicesFromFile(PORTS_FILE);
    getNecessaryPortsAndIPs(argMap);
}


PortScanner::~PortScanner()
{
}

Port PortScanner::getPortFromLine(std::string line)
{
    Port port;
    std::string numberString;

    std::stringstream linestream(line);

    linestream >> port.service;

    linestream >> numberString;
    port.number = std::stoi(numberString);

    linestream >> port.protocol;

    port.state = CLOSED;

    return port;
}

void PortScanner::getPortServicesFromFile(std::string filename)
{
    std::ifstream file(filename);
    if (file.fail())
    {
        std::cout << "Error opening " << filename << "... Port services will be unavailable.\n";
        servicesAvailable = false;

        return;
    }

    std::string line;
    while (getline(file, line))
    {
        Port port = getPortFromLine(line);
        
        portMap.addPort(port);
    }
}


Port PortScanner::createPort(int portnum, std::string service)
{
    Port port;
    Port.service  = service;
    Port.protocol = protocol;
    Port.number   = portnum;    

    return port;
}

void PortScanner::getNecessaryPorts(ArgMap argMap)
{
    std::stringstream portstream;

    StringVector portVector;
    if (argMap["ports"].size() > 0)
    {
        portVector = argMap["ports"];
    }
    else
    {
        for (int i = MIN_PORT_NUMBER; i <= MAX_PORT_NUMBER; i++)
        {
            portVector.push_back(std::to_string(i));
        }
    }

    StringVector protocol = argMap["protocol"];
    if (protocol.size() > 0)
    {
        protocol = protocol[0];
    }
    else
    {
        protocol = "both";
    }

    ips = argMap["ips"];

    

    std::copy(portVector.begin(), portVector.end(), std::ostream_iterator<std::string>(portstream, " "));

    int portNumber;

    while (portstream >> portNumber)
    {
        if (servicesAvailable)
        {
            if (protocol == "both")
            {
                ports.append(portMap.getPortTCP(portNumber);
                ports.append(portMap.getPortUDP(portNumber);
            }
            else if (protocol == "tcp")
            {
                ports.append(portMap.getPortTCP(portNumber);
            }
            else if (protocol == "udp")
            {
                ports.append(portMap.getPortUDP(portNumber);
            }
        }
        else
        {

        }
    }

}
