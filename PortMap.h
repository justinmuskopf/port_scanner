/*
    Author: Justin Muskopf
    Instructor: Hoffman
    Course: CSCE 4550, Fall 2018
    Assignment: Project 3
 */
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
        Port getPortTCP(int portnum);
        Port getPortUDP(int portnum);

    private:
        std::map<int, PortVector> ports;
        Port getPortByProtocol(int portnum, Protocol protocol);
        bool portMapContains(int portnum);
};
