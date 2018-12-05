#include "PortScanner.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>

#define PORTS_FILE "ports_to_services.txt"
#define MIN_DEFAULT_PORT 1
#define MAX_DEFAULT_PORT 1024

PortScanner::PortScanner(ArgParser argParser)
{
    servicesAvailable = true;

    getPortServicesFromFile(PORTS_FILE);
    getNecessaryPortsAndIPs(argParser);
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


Port PortScanner::createPort(int portnum, std::string protocol)
{
    Port port;
    port.protocol = protocol;
    port.number   = portnum;    

    return port;
}

void PortScanner::addBothProtocolsForPort(int portnum)
{
    if (servicesAvailable)
    {
        ports.push_back(portMap.getPortTCP(portnum));
        ports.push_back(portMap.getPortUDP(portnum));
    }
    else
    {
        ports.push_back(createPort(portnum, "tcp"));
        ports.push_back(createPort(portnum, "udp"));
    }
}

void PortScanner::addTCPForPort(int portnum)
{   
    if (servicesAvailable)
    {
        ports.push_back(portMap.getPortTCP(portnum));
    }
    else
    {
        ports.push_back(createPort(portnum, "tcp"));
    }
}

void PortScanner::addUDPForPort(int portnum)
{
    if (servicesAvailable)
    {
        ports.push_back(portMap.getPortUDP(portnum));
    }
    else
    {  
        ports.push_back(createPort(portnum, "udp"));
    }
}

void PortScanner::getNecessaryPortsAndIPs(ArgParser argParser)
{
    std::stringstream portstream;

    StringVector portVector = argParser.getPorts();
    if (portVector.size() == 0)
    {
        for (int i = MIN_DEFAULT_PORT; i <= MAX_DEFAULT_PORT; i++)
        {
            portVector.push_back(std::to_string(i));
        }
    }

    std::string protocol = argParser.getProtocol();
    
    ips = argParser.getIPs();

    for (int i = 0; i < portVector.size(); i++)
    {
        int portNumber = std::stoi(portVector[i]);

        if (protocol == "both")
        {
            addBothProtocolsForPort(portNumber);
        }
        else if (protocol == "tcp")
        {
            addTCPForPort(portNumber);
        }
        else if (protocol == "udp")
        {
            addUDPForPort(portNumber);
        }
    }
}
