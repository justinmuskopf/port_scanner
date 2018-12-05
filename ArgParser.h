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
        static ArgMap ParseArgs(int argc, char *argv[]);
    private:
        static ArgVector argStrings;
        static ArgMap argMap;
        static int numArgs;

        enum Arg {PORT, IP};

        static int  getArgumentFromArgs(Arg argType, int beginidx);
        static int  getPortsFromArgs(int beginIdx);
        static int  getIPAddressesFromArgs(int beginIdx);
        static int  getIPsFromFile(int idx);
        static int  getTransportFromArg(int idx);
        static void indexPanic(int idx);
        static void printHelp();
        static void addIPRangeToMap(int hyphenIdx, std::string);
        static void addPortRangeToMap(int hyphenIdx, std::string);
        static bool argumentIs(std::string want, std::string arg);
        static bool isValidIP(std::string ip);
        static bool isValidPort(std::string portnum);
        static bool isInvalidPort(int portnum);
};

