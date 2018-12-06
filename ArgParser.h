/*
    Author: Justin Muskopf
    Instructor: Hoffman
    Course: CSCE 4550, Fall 2018
    Assignment: Project 3
 */
#pragma once

#include <string>
#include <map>
#include <vector>

void die(std::string message = "", int code = 0);

typedef std::vector<std::string> ArgVector;
typedef std::map<std::string, ArgVector> ArgMap;

class ArgParser
{
    public:
        ArgParser();
        ArgParser(int argc, char *argv[]);
        void ParseArgs(int argc, char *argv[]);
        ArgVector getPorts();
        ArgVector getIPs();
        std::string getProtocol();
    private:
        ArgVector argStrings;
        ArgMap argMap;
        int numArgs;

        ArgVector ports;
        ArgVector ips;
        std::string protocol;

        enum Arg {PORT, IP};


        int  getArgumentFromArgs(Arg argType, int beginidx);
        int  getPortsFromArgs(int beginIdx);
        int  getIPAddressesFromArgs(int beginIdx);
        int  getIPsFromFile(int idx);
        int  getTransportFromArg(int idx);
        void indexPanic(int idx);
        void printHelp();
        void addIPRangeToMap(int hyphenIdx, std::string);
        void addPortRangeToMap(int hyphenIdx, std::string);
        bool argumentIs(std::string want, std::string arg);
        bool isValidIP(std::string ip);
        bool isValidPort(std::string portnum);
        bool isInvalidPort(int portnum);
};

