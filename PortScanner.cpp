#include "PortScanner.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <iomanip>
#include <cstdlib>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORTS_FILE "ports_to_services.txt"
#define MIN_DEFAULT_PORT 1
#define MAX_DEFAULT_PORT 1024
#define UDP_TIMEOUT 1

PortScanner::PortScanner(ArgParser argParser)
{
    servicesAvailable = true;

    getPortServicesFromFile(PORTS_FILE);
    getNecessaryPortsAndIPs(argParser);
}


PortScanner::~PortScanner()
{
}

PortVector PortScanner::scanPortsForIP(std::string ip)
{
    PortVector ipPorts;

    std::cout << "Scanning ports on " << ip << "...\n";

    for (Port port: ports)
    {
        if (port.protocol == PROTOCOLS[TCP])
        {
            port.state = checkTCPPort(port.number, ip);
        }
        else if (port.protocol == PROTOCOLS[UDP])
        {
            port.state = checkUDPPort(port.number, ip);
        }

        ipPorts.push_back(port);
    }

    return ipPorts;
}

std::vector<PortVector> PortScanner::Scan()
{
    for (std::string ip : ips)
    {
        portsByIP.push_back(scanPortsForIP(ip));
    }

    return portsByIP;
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

void PortScanner::addTCPForPort(int portnum)
{   
    if (servicesAvailable)
    {
        ports.push_back(portMap.getPortTCP(portnum));
    }
    else
    {
        ports.push_back(createPort(portnum, PROTOCOLS[TCP]));
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
        ports.push_back(createPort(portnum, PROTOCOLS[UDP]));
    }
}

void PortScanner::addBothProtocolsForPort(int portnum)
{
    addTCPForPort(portnum);
    addUDPForPort(portnum);
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

    protocol = argParser.getProtocol();
    
    ips = argParser.getIPs();

    for (int i = 0; i < portVector.size(); i++)
    {
        int portNumber = std::stoi(portVector[i]);

        if (protocol == "both")
        {
            addBothProtocolsForPort(portNumber);
        }
        else if (protocol == PROTOCOLS[TCP])
        {
            addTCPForPort(portNumber);
        }
        else if (protocol == PROTOCOLS[UDP])
        {
            addUDPForPort(portNumber);
        }
    }
}


PortState PortScanner::checkTCPPort(int portnum, std::string ip)
{

	int sock;
	
    struct sockaddr_in svr_addr;
	
    // Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) 
	{
		die("Error opening TCP socket");
	}

	// Zero bytes of svr_addr and assign characteristics
	bzero((char *) &svr_addr, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	svr_addr.sin_port = htons(portnum);

	// Connect to server
	if (connect(sock,(sockaddr *) &svr_addr,sizeof(svr_addr)) < 0) 
	{
        close(sock);
        return CLOSED;
	}

    shutdown(sock, SHUT_RDWR);
    
    close(sock);    

    return OPEN;
}

PortState PortScanner::checkUDPPort(int portnum, std::string ip)
{
    int sock;

    struct sockaddr_in this_addr;
    struct sockaddr_in svr_addr;

    timeval timeout;
    timeout.tv_sec = 0.5;

    bool shouldRun = false;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        die("Error opening UDP socket");
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);

	// Zero bytes of svr_addr and assign characteristics
	bzero((char *)&svr_addr, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	svr_addr.sin_port = htons(portnum);
 
    size_t svr_len = sizeof(svr_addr);
    
	// Connect to server
	if (connect(sock, (sockaddr *)&svr_addr, sizeof(svr_addr)) < 0) 
	{
        close(sock);
        return CLOSED;
	}

    char buffer[5];
    bzero((char *)buffer, sizeof(buffer));
    if (sendto(sock, buffer, strlen(buffer), 0, (sockaddr *)&svr_addr, svr_len)  < 0)
    {
        std::cout << "Could not send byte\n";
        close(sock);
        return CLOSED;
    }

    int max_fd = sock + 1;
    int sel;
    if ((sel = select(max_fd, &fds, NULL, NULL, &timeout)) > 0)
    {

        std::cout << buffer << std::endl;
    }
    else if (sel == 0)
    {
        return OPEN;
    }
    else
    {
       return CLOSED;
    }

    return OPEN;
}


void PortScanner::printScanReportForIP(int idx)
{
    PortVector tcp, udp;
    for (Port port : portsByIP[idx])
    {
        if (port.protocol == PROTOCOLS[TCP])
        {
            tcp.push_back(port);
        }
        
        if (port.protocol == PROTOCOLS[UDP])
        {
            udp.push_back(port);
        }
    }
    
    std::cout << "------------------------------------------------\n";
    std::cout << "IP: " << ips[idx] << "\n";
    std::cout << "------------------------------------------------\n";
    std::cout << " TCP PORT   STATE   SERVICE\n";
    for (Port port : tcp)
    {
        if (port.state != OPEN)
        {
            continue;
        }

        std::cout << " " << port.number << "\t" << "OPEN\t" << port.service << "\n";
    }

    if (protocol == "both")
    {
        std::cout << "------------------------------------------------\n";
        std::cout << "UDP PORT\tSTATE\tSERVICE\n";
        for (Port port : udp)
        {
            std::cout << port.number << "\t" << "OPEN\t" << port.service << "\n";
        }
    }
}


void PortScanner::PrintScanReport()
{
    for (int i = 0; i < ips.size(); i++)
    {
        printScanReportForIP(i);
    }
}
