#include "packet.h"

void Packet::setIP(const WINDIVERT_IPHDR& ip) {
	this->ip = ip;
}

void Packet::setTCP(const WINDIVERT_TCPHDR& tcp) {
	this->tcp = tcp;
}
