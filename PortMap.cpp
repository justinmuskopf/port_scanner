#include <iostream>
#include "PortMap.h"


PortMap::PortMap()
{

}

PortMap::~PortMap()
{

}

bool PortMap::portMapContains(int portnum)
{
    return (ports.count(portnum) > 0) ? true : false;
}


void PortMap::addPort(Port port)
{
    if (!portMapContains(port.number))
    {
        ports[port.number] = PortVector{port};
    }
    else
    {
        ports[port.number].push_back(port);
    }
}

Port PortMap::getPortByProtocol(int portnum, Protocol protocol)
{
    // Map doesn't contain this port
    if (!portMapContains(portnum))
    {
        return PortVector();
    }

    Port ret;

    std::string protocolString = PROTOCOLS[protocol];

    // Get only the port objects of protocol protocol
    PortVector thesePorts = ports[portnum];
    int numPorts = thesePorts.size();

    std::string services;

    std::cout << "Found " << thesePorts.size() << " ports at port number " << portnum << "\n";
    for (int idx = 0; idx < numPorts; idx++)
    {
        if (thesePorts[idx].protocol == protocolString)
        {
            services += thesePorts[idx].service;
            if (idx != numPorts - 1)
            {
                services += ", ";
            }
        }
    }

    ret.number   = portnum;
    ret.protocol = protocolString;
    ret.service  = services;

    return ret;
}

Port PortMap::getPortTCP(int portnum)
{
    return getPortByProtocol(portnum, TCP);
}

Port PortMap::getPortUDP(int portnum)
{
    return getPortByProtocol(portnum, UDP);
}
