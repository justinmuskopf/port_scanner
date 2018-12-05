GPP = g++
GPP_ARGS += -std=c++11
GPP_ARGS += -Wall
GPP_ARGS += -o

PORT = port
MODULES += $(PORT)

PORTMAP = portmap
MODULES += $(PORTMAP)

PORTSCANNER = portscanner
MODULES += $(PORTSCANNER)

ARGPARSER = argparser
MODULES += $(ARGPARSER)

MAIN = main
MODULES += $(MAIN)

RM_CMD = rm -f
GPP_CMD = $(GPP) $(GPP_ARGS)



all: clean $(MODULES)
	$(GPP_CMD) portScanner $(MODULES)

clean: 
	$(RM_CMD) $(MODULES) portScanner

port: Port.h
	$(GPP_CMD) $(PORT) -c Port.cpp

portmap: port PortMap.h
	$(GPP_CMD) $(PORTMAP) -c PortMap.cpp

portscanner: port portmap PortMap.h
	$(GPP_CMD) $(PORTSCANNER) -c PortScanner.cpp

argparser: ArgParser.h
	$(GPP_CMD) $(ARGPARSER) -c ArgParser.cpp

main: port portmap portscanner argparser
	$(GPP_CMD) $(MAIN) -c main.cpp

