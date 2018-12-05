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
    PortScanner(ArgMap argMap);
	~PortScanner();

	PortVector Scan();

private:
    PortMap portMap;
    StringVector ips;
    PortVector ports;
    
    std::string protocol;

    bool servicesAvailable;

    void getPortServicesFromFile(std::string filename);
    Port getPortFromLine(std::string line);
    Port createPort(int portnum, std::string service = "");
    void getNecessaryPortsAndIPs(ArgMap argMap);
};

