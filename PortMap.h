#include <map>
#include <string>
#include <vector>

#include "Port.h"

class PortMap
{
    public:
        PortMap();
        ~PortMap();

        void addPort(Port port);
        PortVector getPortTCP(int portnum);
        PortVector getPortUDP(int portnum);

    private:
        std::map<int, PortVector> ports;
        PortVector getPortByProtocol(int portnum, Protocol protocol);
        bool portMapContains(int portnum);
};
