/*
    Author: Justin Muskopf
    Instructor: Hoffman
    Course: CSCE 4550, Fall 2018
    Assignment: Project 3
 */
#include <arpa/inet.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <fstream>
#include "ArgParser.h"
#include "Port.h"

#define MAX_ARGS 5
#define MIN_IP_LEN sizeof("0.0.0.0")
#define MAX_IP_LEN sizeof("255.255.255.255")
#define OCTET_MAX 255

#define MIN_PORT 0
#define MAX_PORT 65535

// Default constructor
ArgParser::ArgParser()
{
    protocol = "both";
}

// Initializing Constructor
ArgParser::ArgParser(int argc, char *argv[])
{
    protocol = "both";
    ParseArgs(argc, argv);
}

// Error and DIE
void die(std::string message, int code)
{
    if (message.length())
    {
        std::cout << message << "\n";
    }

    exit(code);
}


// Prints help to screen
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

// Checks if 'want' == 'arg'
bool ArgParser::argumentIs(std::string want, std::string arg)
{
    // Get first  character from want
    std::string first = std::string(1, want[0]);

    // Compose short argument and long argument
    std::string shortArg = "-" + first;
    std::string longArg  = "--" + want;

    // Not the right format
    if (arg != shortArg && arg != longArg)
    {
        return false;
    }

    return true;
}


// Invalid index? Panic at The Disco!
void ArgParser::indexPanic(int idx)
{
    if (idx == -1)
    {
        die("Invalid argument sequence provided!", 2);
    }
}

// Is `ip` a valid IP?
bool ArgParser::isValidIP(std::string ip)
{
    sockaddr_in temp;

    // Use inet_pton to check ip validity
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

    // The index directly after the last period
    int afterLastPeriod = ipWithRange.find_last_of('.') + 1;

    // IP Base, e.g. 192.168.254
    std::string ipBase = ipWithRange.substr(0, afterLastPeriod);

    // What comes after the last period, e.g. 110-122
    std::string rangeString = ipWithRange.substr(afterLastPeriod);

    // Replace the hyphen so this ss will be whitespace delimited
    std::replace(rangeString.begin(), rangeString.end(), '-', ' ');

    // Create a stringstream out of it
    std::stringstream rangeStream(rangeString);

    int rangeBegin; // Beginning of range
    int rangeEnd;   // End of range

    // Feed string -> ints
    rangeStream >> rangeBegin;
    rangeStream >> rangeEnd;

    // Create IP String for each value and add to vector
    for (int rangeValue = rangeBegin; rangeValue <= rangeEnd; rangeValue++)
    {
        std::string ipString = ipBase + std::to_string(rangeValue);
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
    // Replace the hyphen with a space and stringstreamify it
    std::replace(portWithRange.begin(), portWithRange.end(), '-', ' ');
    std::stringstream portstream(portWithRange);

    int beginRange; // Beginning of range
    int endRange;   // End of range

    // Feed into strings -> int
    portstream >> beginRange;
    portstream >> endRange;

    // Check port validity before continuing
    if (isInvalidPort(beginRange) || isInvalidPort(endRange))
    {
        die("Invalid Port range provided: " + portWithRange);
    }

    // Add each of the string port values to the vector
    for (int rangeValue = beginRange; rangeValue <= endRange; rangeValue++)
    {
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
    // Fire? Where?!
    indexPanic(beginIdx);

    std::string possibleArgs;

    std::string printKey = (argType == IP) ? "IP Addressess" : "Ports";

    // Counts the number of arguments this function consumed from argv
    int numArgsTaken = 0;

    // Loop through argv
    for (int i = beginIdx; i < numArgs; i++)
    {
        std::string arg = argStrings[i];

        // Not a new argument yet
        if (arg[0] != '-')
        {

            // Comma delimited, make 'em spaces
            if (arg.find(',') >= 0)
            {
                std::replace(arg.begin(), arg.end(), ',', ' ');
            }

            // Add to found args
            possibleArgs += " " + arg;

            numArgsTaken++;
        }
        // New argument! Everybody out!
        else
        {
            break;
        }
    }

    // :(
    if (numArgsTaken == -1)
    {
        die("Could not find any potential " + printKey + " in arguments");
    }

    std::string currentArg;
    std::stringstream argstream(possibleArgs);

    // Feed the found args individually into currentArg
    while (argstream >> currentArg)
    {
        // It's got a hyphen; this sneaky boy is a RANGE!
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

        // It's IP time, y'all
        if (argType == IP && isValidIP(currentArg))
        {
            ips.push_back(currentArg);
        }

        // Sailin' from port to port
        else if (argType == PORT && isValidPort(currentArg))
        {
            ports.push_back(currentArg);
        }
        // HISSSSSS
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
    // Seriously... Anyone have a fire extinguisher?
    indexPanic(idx);

    // Grab that there protocol why don't ya
    std::string transport = argStrings[idx];
    std::string _protocol;

    if (transport == "tcp" || transport == "TCP")
    {
        _protocol = PROTOCOLS[TCP];
    }
    else if (transport == "udp" || transport == "UDP")
    {
        _protocol = PROTOCOLS[UDP];
    }
    else
    {
        die("Invalid transport provided: " + transport);
    }

    protocol = _protocol;

    std::cout << protocol << "\n";

    // Where to begin from after return
    return idx + 1;
}


int ArgParser::getIPsFromFile(int idx)
{
    // IT BURNS
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
        // Users and their invalid input
        if (isValidIP(line) == false)
        {
            die("Invalid IP found in file: " + line);
        }

        ips.push_back(line);
    }

    return idx + 1;
}

// Parses the arguments given to main
void ArgParser::ParseArgs(int argc, char *argv[])
{
    bool foundIP = false;

    if (argc == 1)
    {
        die("Hey... You got the args?");
    }

    // Stringify those gross C-Strings!
    for (int i = 1; i < argc; i++)
    {
        argStrings.push_back(std::string(argv[i]));
    }

    // Take out that useless script name
    numArgs = argc - 1;


    // Leniency is for the weak... but it's a requirement
    if (numArgs == 1 && isValidIP(argStrings[0]))
    {
        ips.push_back(argStrings[0]);
        return;
    }

    int argIdx = 0;

    // It's always good to know your boundaries
    while (argIdx < numArgs)
    {
        std::string argString = argStrings[argIdx];
        int nextArg = (argIdx + 1 >= numArgs) ? -1 : argIdx + 1;

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
            foundIP = true;
        }
        else
        {
            die("Invalid argument provided: " + argString);
        }
    }


    if (foundIP == false)
    {

        die("Please provide at least one IP address using the '--ip' argument");
    }
}
