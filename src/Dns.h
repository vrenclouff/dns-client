#pragma once

#include "DnsMessage.h"

#include <winsock2.h>
#include <memory>

namespace dns {

	class Client {
	private:
		int _socket;
		sockaddr_in _sock_srv;
		std::string _server;

	public:

		static std::unique_ptr<Client> init(const std::string server_addr) {
			return std::make_unique<Client>(server_addr);
		}

		Client(const std::string server_addr);
		~Client();

		response send(const std::string domain);
	};
}