/*
    Author: Justin Muskopf
    Instructor: Hoffman
    Course: CSCE 4550, Fall 2018
    Assignment: Project 3
*/
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
    void PrintScanReport();

private:
    PortMap portMap;
    StringVector ips;
    PortVector ports;
    
    std::string protocol;

    std::vector<PortVector> portsByIP;

    bool servicesAvailable;

    void addTCPForPort(int portnum);
    void addUDPForPort(int portnum);
    void addBothProtocolsForPort(int portnum);

    PortState checkTCPPort(int portnum, std::string ip);
    PortState checkUDPPort(int portnuum, std::string ip);

    PortVector scanPortsForIP(std::string ip);

    void printScanReportForIP(int idx);
    void getPortServicesFromFile(std::string filename);
    Port getPortFromLine(std::string line);
    Port createPort(int portnum, std::string protocol);
    void getNecessaryPortsAndIPs(ArgParser argParser);
};

