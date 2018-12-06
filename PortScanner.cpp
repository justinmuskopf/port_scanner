/*
    Author: Justin Muskopf
    Instructor: Hoffman
    Course: CSCE 4550, Fall 2018
    Assignment: Project 3
*/
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
#define UDP_TIMEOUT_US 250000 //0.25 seconds

// Initialize with default values
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

    // Loop through all ports
    for (Port port: ports)
    {
        // Do the TCP
        if (port.protocol == PROTOCOLS[TCP])
        {
            port.state = checkTCPPort(port.number, ip);
        }
        // Do the UDP
        else if (port.protocol == PROTOCOLS[UDP])
        {
            port.state = checkUDPPort(port.number, ip);
        }

        // Do the Pusha T
        ipPorts.push_back(port);
    }

    return ipPorts;
}

std::vector<PortVector> PortScanner::Scan()
{
    // Loop through IPs and push their scans
    for (std::string ip : ips)
    {
        portsByIP.push_back(scanPortsForIP(ip));
    }

    return portsByIP;
}

Port PortScanner::getPortFromLine(std::string line)
{
    Port port;

    // Streamify the line
    std::stringstream linestream(line);

    linestream >> port.service;

    linestream >> port.number;

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
    // Base port with no service
    Port port;
    port.protocol = protocol;
    port.number   = portnum;    

    return port;
}

void PortScanner::addTCPForPort(int portnum)
{   
    if (servicesAvailable)
    {
        // Get service port
        ports.push_back(portMap.getPortTCP(portnum));
    }
    else
    {
        // Get base port
        ports.push_back(createPort(portnum, PROTOCOLS[TCP]));
    }
}

void PortScanner::addUDPForPort(int portnum)
{
    if (servicesAvailable)
    {
        // Get service port
        ports.push_back(portMap.getPortUDP(portnum));
    }
    else
    {  
        // Get base port
        ports.push_back(createPort(portnum, PROTOCOLS[UDP]));
    }
}

void PortScanner::addBothProtocolsForPort(int portnum)
{
    // Helper functions rule!
    addTCPForPort(portnum);
    addUDPForPort(portnum);
}

void PortScanner::getNecessaryPortsAndIPs(ArgParser argParser)
{
    std::stringstream portstream;

    // Get the ports from the argument parser
    StringVector portVector = argParser.getPorts();
    if (portVector.size() == 0)
    {
        // Default ports, none provided
        for (int i = MIN_DEFAULT_PORT; i <= MAX_DEFAULT_PORT; i++)
        {
            portVector.push_back(std::to_string(i));
        }
    }

    // Get protocol ("TCP" || "UDP") from parser
    protocol = argParser.getProtocol();
    
    // Get IPs from parser
    ips = argParser.getIPs();

    // Sequentially add each port to ports
    for (unsigned int i = 0; i < portVector.size(); i++)
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

// Is this TCP port open?
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

    // TCP RST (I think...)
    shutdown(sock, SHUT_RDWR);
    
    // DOUBLE UP
    close(sock);    

    return OPEN;
}

PortState PortScanner::checkUDPPort(int portnum, std::string ip)
{
    // Socket fd
    int sock;

    // Reps the SVR
    struct sockaddr_in svr_addr;

    // Timeout for select
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = UDP_TIMEOUT_US;

    // Rock 'Em, 'n Sock 'Em
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        die("Error opening UDP socket");
    }

    // Create fd_set
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

    /*============================================*/
    /*    BEWARE: THIS IS WHERE IT GETS HAIRY     */
    /*============================================*/

    char c = 0;
    char buffer[5];

    // Zero out buffer and send it default array
    bzero((char *)buffer, sizeof(buffer));
    if (sendto(sock, &c, 1, 0, (sockaddr *)&svr_addr, svr_len) < 0)
    {
        die("UDP sendto");
    }

    // Get max fd
    int max_fd = sock + 1;
    int sel = -1;

    // Something came back
    if ((sel = select(max_fd, &fds, NULL, NULL, &timeout)) > 0)
    {
        // Something is in that socket
        int n = recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr *)&svr_addr, &svr_len);
        if (n > 0)
        {
            return OPEN;
        }

        close(sock);

        return CLOSED;
    }
    // Timed out, probably open
    else if (sel == 0)
    {
        close(sock);
        return OPEN;
    }
    // ERR
    else
    {
        die("Select");
    }

    close(sock);

    return CLOSED;
}


void PortScanner::printScanReportForIP(int idx)
{
    PortVector tcp, udp;

    // Separate IPs into TCP/UDP
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
    
    // Gross and ugly cout stuff
    std::cout << "------------------------------------------------\n";
    std::cout << "IP: " << ips[idx] << "\n";
    std::cout << "------------------------------------------------\n";
    std::cout << "TCP PORT\tSTATE\tSERVICE\n";
    std::cout << "------------------------------------------------\n";
    for (Port port : tcp)
    {
        if (port.state != OPEN && port.service.length() == 0)
        {
            continue;
        }

        std::string stateString = (port.state == OPEN) ? "OPEN" : "CLOSED";

        std::cout << " " << port.number << "\t\t" << stateString << "\t" << port.service << "\n";
    }

    // Print UDP too!
    if (protocol == "both")
    {
        std::cout << "------------------------------------------------\n";
        std::cout << "UDP PORT\tSTATE\tSERVICE\n";
        std::cout << "------------------------------------------------\n";
        for (Port port : udp)
        {
            if (port.state != OPEN && port.service.length() == 0)
            {
                continue;
            }

            std::string stateString = (port.state == OPEN) ? "OPEN" : "CLOSED";

            std::cout << port.number << "\t\t" << stateString << "\t" << port.service << "\n";
        }
    }
}


void PortScanner::PrintScanReport()
{
    // Print for every IP
    for (unsigned int i = 0; i < ips.size(); i++)
    {
        printScanReportForIP(i);
    }
}
