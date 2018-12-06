#pragma once

#include <string>
#include <vector>

enum PortState {OPEN, CLOSED};

const std::string PROTOCOLS[] = {"tcp", "udp"};
enum Protocol {TCP, UDP};

struct Port
{
	int number;

	std::string service;

    std::string protocol;

	PortState state;

    void print();
};

typedef std::vector<Port> PortVector;
