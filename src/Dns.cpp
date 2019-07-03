#include "Dns.h"

#include "NetworkParams.h"

#include <stdio.h>
#include <ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

#define MESSAGE_ERROR (-1)

dns::Client::Client(const std::string server_addr){

	std::string addr = server_addr;

	if (server_addr.length() == 0) {
		dns::NetworkParams networkParams;
		std::vector<std::string> dns_servers = networkParams.dns_servers();
		addr = std::string(dns_servers.size() != 0 ? dns_servers.back() : DEFAULT_DNS);
	}

	_server = addr;

	WSADATA wsdata;
	if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
		printf("Failed.Error Code : %d", WSAGetLastError());
		return;
	}

	if ((_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
		printf("socket() failed with error code : %d\n", WSAGetLastError());
		return;
	}

	memset((char *)&_sock_srv, 0, sizeof(_sock_srv));

	_sock_srv.sin_family = AF_INET;
	_sock_srv.sin_port = htons(53);
	InetPton(AF_INET, addr.data(), &_sock_srv.sin_addr.S_un.S_addr);
}

dns::Client::~Client() {
	closesocket(_socket);
	WSACleanup();
}

dns::response dns::Client::send(const std::string domain) {

	dns::request request;
	if (dns::create_request(domain, request) == MESSAGE_ERROR) {
		throw std::exception("Cannot parse the request.");
	}

	if (sendto(_socket, request.data, request.length, 0, (struct sockaddr *) &_sock_srv, sizeof(_sock_srv)) == SOCKET_ERROR) {
		throw std::exception("Cannot send the request.");
	}

	char buffer[512];
	sockaddr_in sender;
	int senderSize = sizeof(sender);

	int recieved_bytes = recvfrom(_socket, buffer, sizeof(buffer), 0, (SOCKADDR *)&sender, &senderSize);
	
	if (recieved_bytes == SOCKET_ERROR) {
		throw std::exception("Received message is empty.");
	}

	dns::response response;
	std::string buff_str = std::string(buffer, recieved_bytes);
	response.server = _server;


	if (dns::create_response(buff_str, response) == MESSAGE_ERROR) {
		throw std::exception("Cannot parse response.");
	}

	return response;
}

