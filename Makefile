all: portmap portscanner main
	g++ -std=c++11 -o portScanner portmap portscanner main

portmap: Port.h
	g++ -std=c++11 -o portmap -c PortMap.cpp

portscanner: Port.h portmap PortMap.h
	g++ -std=c++11 -o portscanner -c PortScanner.cpp

main: Port.h portmap portscanner
	g++ -std=c++11 -o main -c main.cpp
