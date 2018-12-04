#pragma once

#include <string>
#include <vector>
#include <map>

#include "Port.h"

typedef std::vector<std::string> StringVector;

class PortScanner
{
public:
	PortScanner();
	~PortScanner();

	PortVector scan(StringVector toScan);

private:
	PortVector ports;
    std::map<std::string, PortVector>;


    void getPortServicesFromFile(std::string filename);
    Port getPortFromLine(std::string line);
};

