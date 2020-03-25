#include <iostream>
#include <string>
#include "windivert.h"
#include "url.h"
#include "blacklist.h"
#include "packet.h"
#include "datapacket.h"
using namespace std;

#define ntohs(x)            WinDivertHelperNtohs(x)
#define ntohl(x)            WinDivertHelperNtohl(x)
#define htons(x)            WinDivertHelperHtons(x)
#define htonl(x)            WinDivertHelperHtonl(x)

#define MAXBUF              WINDIVERT_MTU_MAX
#define MAXURL              4096


int main() {
	HANDLE handle;          // WinDivert handle

	/*Open some filter*/
	handle = WinDivertOpen(
		"outbound && "              // Outbound traffic only
		"!loopback && "             // No loopback traffic
		"ip && "                    // Only IPv4 supported
		"tcp.DstPort == 443 && "     // HTTP (port 80) only
		"tcp.PayloadLength > 0",    // TCP data packets only
		WINDIVERT_LAYER_NETWORK, 0, 0);
	if (handle == INVALID_HANDLE_VALUE) {
		// Handle error
		cout << stderr << "error: failed to open the WinDivert device (%d)\n" << GetLastError() << endl;
		exit(1);
	}

	/* Initialize the pre-frabricated packets: */
	WINDIVERT_IPHDR ip;
	WINDIVERT_TCPHDR tcp;
	//create datapacket to place our blockpage.
	DataPacket* blockpage = new DataPacket();
	const char block_data[] =//THe block page contents.
		"HTTP/1.1 200 OK\r\n"
		"Connection: close\r\n"
		"Content-Type: text/html\r\n"
		"\r\n"
		"<!doctype html>\n"
		"<html>\n"
		"\t<head>\n"
		"\t\t<title>BLOCKED!</title>\n"
		"\t</head>\n"
		"\t<body>\n"
		"\t\t<h1>BLOCKED!</h1>\n"
		"\t\t<hr>\n"
		"\t\t<p>This URL has been blocked!</p>\n"
		"\t</body>\n"
		"</html>\n";
	UINT16 blockpage_len = sizeof(DataPacket) + sizeof(block_data) - 1;
	ip.Length = WinDivertHelperHtons(blockpage_len);
	tcp.SrcPort = WinDivertHelperHtons(80);
	tcp.Psh = 1;
	tcp.Ack = 1;
	blockpage->setIP(ip);
	blockpage->setTCP(tcp);
	blockpage->setData(block_data);
	//create packet to send 'reset' singal
	Packet* reset = new Packet();

	//create packet to send 'finish' singal
	Packet* finish = new Packet();


	/*Prepare blacklist. */
	Url* url = new Url("ip138.com", "index.html");
	Blacklist* blacklist = new Blacklist();

	/*Create buffer*/
	UINT8 packet[MAXBUF];
	WINDIVERT_ADDRESS addr;
	UINT packet_len;

	PVOID payload;
	UINT payload_len;

	WINDIVERT_IPHDR* ip_header;
	WINDIVERT_TCPHDR* tcp_header;

	/* Main capture-modify-inject loop: */
	while (TRUE) {
		if (!WinDivertRecv(handle, packet, sizeof(packet), &packet_len, &addr)) {
			// Handle recv error
			fprintf(stderr, "warning: failed to read packet (%d)\n", GetLastError());
			continue;
		}
		cout << sizeof(packet) << endl;
		/* Modify packet. */
		WinDivertHelperParsePacket(packet, packet_len, &ip_header, nullptr,
			nullptr, nullptr, nullptr, &tcp_header, nullptr,
			&payload, &payload_len,
			nullptr, nullptr);
		string str((char*)payload);
		cout << str << "got!" << endl;

		if (ip_header == NULL || tcp_header == NULL || payload == NULL) {
			// Packet does not match the blacklist; simply reinject it.
			if (!WinDivertSend(handle, NULL, packet_len, NULL, &addr)) {
				fprintf(stderr, "warning: failed to reinject packet (%d)\n",
					GetLastError());
			}
			continue;
		}


		WinDivertHelperCalcChecksums(packet, packet_len, &addr, 0);
		if (!WinDivertSend(handle, packet, packet_len, NULL, &addr)) {
			// Handle send error
			continue;
		}
	}

	//delete url;
	//delete blacklist;

	return 0;
}
