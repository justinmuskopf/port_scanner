#pragma once

#include <string>
#include <vector>
#include <map>
#include "PortMap.h"
#include "ArgParser.h"

typedef std::vector<std::string> StringVector;

class PortScanner
{
public:
    PortScanner(ArgParser argParser);
	~PortScanner();

	PortVector Scan();

private:
    PortMap portMap;
    StringVector ips;
    PortVector ports;
    
    std::string protocol;

    bool servicesAvailable;

    void addTCPForPort(int portnum);
    void addUDPForPort(int portnum);
    void addBothProtocolsForPort(int portnum);

    void getPortServicesFromFile(std::string filename);
    Port getPortFromLine(std::string line);
    Port createPort(int portnum, std::string protocol);
    void getNecessaryPortsAndIPs(ArgParser argParser);
};

