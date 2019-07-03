#pragma once

#include <string>

#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <windows.h>
#include <vector>

#define DEFAULT_DNS "8.8.8.8"

namespace dns {
	class NetworkParams {
	private:
		 FIXED_INFO *pFixedInfo;

	public:
		 NetworkParams();
		 ~NetworkParams();

		std::vector<std::string> dns_servers();
	};
}