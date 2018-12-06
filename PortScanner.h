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

	std::vector<PortVector> Scan();

private:
    PortMap portMap;
    StringVector ips;
    PortVector ports;
    
    std::string protocol;

    bool servicesAvailable;

    void addTCPForPort(int portnum);
    void addUDPForPort(int portnum);
    void addBothProtocolsForPort(int portnum);

    PortState checkTCPPort(int portnum, std::string ip);
    PortState checkUDPPort(int portnuum, std::string ip);

    PortVector scanPortsForIP(std::string ip);

    void getPortServicesFromFile(std::string filename);
    Port getPortFromLine(std::string line);
    Port createPort(int portnum, std::string protocol);
    void getNecessaryPortsAndIPs(ArgParser argParser);
};

