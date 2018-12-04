
#include "PortMap.h"

PortMap::PortMap()
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

PortVector PortMap::getPortByProtocol(int portnum, Protocol protocol)
{
    // Map doesn't contain this port
    if (!portMapContains(portnum))
    {
        return PortVector();
    }

    PortVector ret;

    std::string protocolString = PROTOCOLS[protocol];

    // Get only the port objects of protocol protocol
    PortVector thesePorts = ports[portnum];
    for (int idx = 0; idx < thesePorts.size(); idx++)
    {
        if (thesePorts[idx].protocol == protocolString)
        {
            ret.push_back(thesePorts[idx]);
        }
    }
}

PortVector PortMap::getPortTCP(int portnum)
{
    return getPortByProtocol("tcp");
}

PortVector PortMap::getPortUDP(int portnum)
{
    return getPortByProtocol("udp");
}


