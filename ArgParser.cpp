#include <arpa/inet.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <fstream>
#include "ArgParser.h"

#define MAX_ARGS 5
#define MIN_IP_LEN sizeof("0.0.0.0")
#define MAX_IP_LEN sizeof("255.255.255.255")
#define OCTET_MAX 255

#define MIN_PORT 0
#define MAX_PORT 65535


ArgParser::ArgParser()
{
    protocol = "both";
}

ArgParser::ArgParser(int argc, char *argv[])
{
    protocol = "both";
    ParseArgs(argc, argv);
}

void die(std::string message, int code)
{
    if (message.length())
    {
        std::cout << message << "\n";
    }

    exit(code);
}


void ArgParser::printHelp()
{
    std::cout << "PORTSCANNER HELP:\n"
                 "--help      (-h): Display this help screen\n"
                 "--port      (-p): Specify the port(s) to scan, either hyphenated (e.g. 10-100), or in a comma separated list (e.g. 32, 23, 55)\n"
                 "--ip        (-i): Specify a set of IP(s) to run the scan on by a hyphenated list on the same subnet (e.g. 192.168.254.10-100), or an individual IP address\n"
                 "--file      (-f): Specify a set of IP(s) to run the scan on by providing a filename that contains a line-delimited set of IP addresses\n"
                 "--transport (-t): Explicitly decide which protocol to check on each of the ports: either TCP or UDP\n";
    die();
}

bool ArgParser::argumentIs(std::string want, std::string arg)
{
    std::string first = std::string(1, want[0]);
    std::string shortArg = "-" + first;
    std::string longArg  = "--" + want;

    if (arg != shortArg && arg != longArg)
    {
        return false;
    }
    
    return true;
}


void ArgParser::indexPanic(int idx)
{
    if (idx == -1)
    {
        die("Invalid argument sequence provided!", 2);
    }
}


bool ArgParser::isValidIP(std::string ip)
{
    sockaddr_in temp;

    if (inet_pton(AF_INET, ip.c_str(), &(temp.sin_addr)) != 1)
    {
        return false;
    }
 
    return true;   
}



void ArgParser::addIPRangeToMap(int hyphenIdx, std::string ipWithRange)
{
    // Error check the pre-range IP
    std::string firstIP = ipWithRange.substr(0, hyphenIdx);
    if (isValidIP(firstIP) == false)
    {
        die("Invalid IP range provided: " + ipWithRange);
    }

    int afterLastPeriod = ipWithRange.find_last_of('.') + 1;

    std::string ipBase = ipWithRange.substr(0, afterLastPeriod);

    std::string rangeString = ipWithRange.substr(afterLastPeriod);

    std::replace(rangeString.begin(), rangeString.end(), '-', ' ');

    std::stringstream rangeStream(rangeString);

    int rangeBegin;
    int rangeEnd;   

    rangeStream >> rangeBegin;
    rangeStream >> rangeEnd;

    for (int rangeValue = rangeBegin; rangeValue <= rangeEnd; rangeValue++)
    {
        std::string ipString = ipBase + std::to_string(rangeValue);
        std::cout << "adding IP " + ipString + "\n";
        ips.push_back(ipString);        
    }
}


bool ArgParser::isValidPort(std::string port)
{
    std::stringstream portstream(port);
    int portnum;

    portstream >> portnum;

    return (portnum >= MIN_PORT && portnum <= MAX_PORT) ? true : false;    
}

bool ArgParser::isInvalidPort(int portnum)
{
    if (portnum < MIN_PORT || portnum > MAX_PORT)
    {
        return true;
    }

    return false;
}

void ArgParser::addPortRangeToMap(int hyphenIdx, std::string portWithRange)
{
    std::replace(portWithRange.begin(), portWithRange.end(), '-', ' ');
    std::stringstream portstream(portWithRange);

    int beginRange;
    int endRange;

    portstream >> beginRange;
    portstream >> endRange;

    if (isInvalidPort(beginRange) || isInvalidPort(endRange))
    {
        die("Invalid Port range provided: " + portWithRange);
    }
    
    for (int rangeValue = beginRange; rangeValue <= endRange; rangeValue++)
    {
        std::cout << "adding port " << rangeValue << "\n";
        std::string portString = std::to_string(rangeValue);
        ports.push_back(portString);
    }
}

ArgVector ArgParser::getIPs()
{
    return ips;
}

ArgVector ArgParser::getPorts()
{
    return ports;
}

std::string ArgParser::getProtocol()
{
    return protocol;
}  

int ArgParser::getArgumentFromArgs(Arg argType, int beginIdx)
{
    indexPanic(beginIdx);

    std::string possibleArgs;

    std::string printKey = (argType == IP) ? "IP Addressess" : "Ports";

    int numArgsTaken = 0;

    for (int i = beginIdx; i < numArgs; i++)
    {
        std::string arg = argStrings[i];

        std::cout << arg << "\n";


        if (arg[0] != '-')
        {
        
            if (arg.find(',') >= 0)
            {
                std::replace(arg.begin(), arg.end(), ',', ' ');
            }

            possibleArgs += " " + arg;

            numArgsTaken++;
        }
        else
        {
            break;
        }
    }

    if (numArgsTaken == -1)
    {
        die("Could not find any potential " + printKey + " in arguments");
    }

    std::string currentArg;
    std::stringstream argstream(possibleArgs);

    while (argstream >> currentArg)
    {
        int hyphenIdx = currentArg.find('-');
        if (hyphenIdx != -1)
        {
            if (argType == IP)
            {
                addIPRangeToMap(hyphenIdx, currentArg);
            }
            else
            {
                addPortRangeToMap(hyphenIdx, currentArg);
            }

            continue;
        }

        if (argType == IP && isValidIP(currentArg))
        {
            std::cout << "Adding IP address " + currentArg + " to map";
            ips.push_back(currentArg);
        }
        
        else if (argType == PORT && isValidPort(currentArg))
        {
            std::cout << "Adding port " + currentArg + " to map";
            ports.push_back(currentArg);
        }

        else
        {
            die("Invalid " + printKey + " provided: " + currentArg);
        }
    }
    
    return beginIdx + numArgsTaken;
}


int ArgParser::getIPAddressesFromArgs(int beginIdx)
{
    return getArgumentFromArgs(IP, beginIdx);
}


int ArgParser::getPortsFromArgs(int beginIdx)
{
    return getArgumentFromArgs(PORT, beginIdx);
}


int ArgParser::getTransportFromArg(int idx)
{
    indexPanic(idx);

    std::string transport = argStrings[idx];
    std::string _protocol;

    if (transport == "tcp" || transport == "TCP")
    {
        _protocol = "TCP";
    }
    else if (transport == "udp" || transport == "UDP")
    {
        _protocol = "UDP";
    }
    else
    {
        die("Invalid transport provided: " + transport);
    }

    protocol = _protocol;

    std::cout << "ptrooroo\n";

    return idx + 1;
}

int ArgParser::getIPsFromFile(int idx)
{
    indexPanic(idx);

    std::string filename = argStrings[idx];

    std::ifstream file(filename);
    if (file.fail())
    {
        die("Could not open '" + filename + "' for reading");
    }

    std::string line;
    while (getline(file, line))
    {
        if (isValidIP(line) == false)
        {
            die("Invalid IP found in file: " + line);
        }

        ips.push_back(line);
    }

    return idx + 1;
}

void ArgParser::ParseArgs(int argc, char *argv[])
{
    bool foundIP = false;

    for (int i = 1; i < argc; i++)
    {
        argStrings.push_back(std::string(argv[i]));
    }

    numArgs = argc - 1;

    ArgMap argMap;
    int argIdx = 0;

    while (argIdx < numArgs)
    {
        std::string argString = argStrings[argIdx];
        int nextArg = (argIdx + 1 >= numArgs) ? -1 : argIdx + 1;

        std::cout << "arg " + argString + "\n";

        if (argumentIs("help", argString))
        {
            printHelp();
        }
        else if (argumentIs("port", argString))
        {
            argIdx = getPortsFromArgs(nextArg);
        }
        else if (argumentIs("ip", argString))
        {
            argIdx = getIPAddressesFromArgs(nextArg);
            foundIP = true;
        }
        else if (argumentIs("transport", argString))
        {
            argIdx = getTransportFromArg(nextArg);
        }
        else if (argumentIs("file", argString))
        {
            argIdx = getIPsFromFile(nextArg);
        }
    }
    

    if (foundIP == false)
    {
        die("Please provide at least one IP address using the '--ip' argument");
    }

    std::cout << "extiing with #" << ports.size() << " ports\n";

}
