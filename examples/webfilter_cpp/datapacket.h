#pragma once
#include "packet.h"

//DATAPacket include both the headerand data.
class DataPacket :public Packet
{
public:
	void setData(const char* data) {
		memcpy(this->data, data, sizeof(data) - 1);
	}
private:
	UINT8 data[];
};
