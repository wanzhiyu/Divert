#pragma once
#include "windivert.h"

//Packet is just the header. It doesn't contain any data.
class Packet
{
public:
	Packet() {}
	Packet(const WINDIVERT_IPHDR& ip, const WINDIVERT_TCPHDR& tcp) :ip(ip), tcp(tcp) {}
	virtual void setIP(const WINDIVERT_IPHDR& ip);
	virtual void setTCP(const WINDIVERT_TCPHDR& tcp);
protected:
	WINDIVERT_IPHDR  ip;
	WINDIVERT_TCPHDR tcp;
};
