#include "PortScanner.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cstring>
#include <unistd.h>
#include <errno.h>

#include <cstdlib>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


#define PORTS_FILE "ports_to_services.txt"
#define MIN_DEFAULT_PORT 1
#define MAX_DEFAULT_PORT 1024

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
            std::cout << "Scanning TCP Port " << port.number << "\n";
            port.state = checkTCPPort(port.number, ip);
            std::cout <<"... " << port.state << "\n";
        }
        else if (port.protocol == PROTOCOLS[UDP])
        {
            std::cout << "Scanning UDP Port " << port.number << "\n";
            port.state = checkUDPPort(port.number, ip);
            std::cout <<"... " << port.state << "\n";
        }

        ipPorts.push_back(port);
    }

    return ipPorts;
}

std::vector<PortVector> PortScanner::Scan()
{
    std::vector<PortVector> portsByIP;
 
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

    std::string protocol = argParser.getProtocol();
    
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

    uint8_t buffer[5];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        die("Error opening UDP socket");
    }

	// Zero bytes of svr_addr and assign characteristics
	bzero((char *)&svr_addr, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	svr_addr.sin_port = htons(portnum);
 
    bzero((char *)&this_addr, sizeof(this_addr));
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = 0;


    size_t svr_len = sizeof(svr_addr);
    size_t this_len = sizeof(this_addr);
    if (bind(sock, (sockaddr *)&this_addr, this_len) == -1)
    {
        close(sock);
        die("Could not bind socket");
    }

/*    if (sendto(sock, "hello", 5, 0, (sockaddr *)&svr_addr, svr_len)  < 0)
    {
        if (errno == EHOSTUNREACH || errno == EHOSTDOWN)
        {
            std::cout << "CAUGHT YA YE BASTEARD\n";
            return CLOSED;
        }
        std::cout << "Could not send byte\n";
        close(sock);
        return CLOSED;
    }*/


    if (recvfrom(sock, buffer, 4, 0, (sockaddr *)&svr_addr, &svr_len) < 0)
    {
        std::cout << "receive failed\n";
        close(sock);
        return CLOSED;
    }

    for (int i = 0; i < 5; i++)
    {
        std::cout << buffer[i] << " ";
    }
    std::cout << "\n";

    return OPEN;
}
