#include "NetworkParams.h"


#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

dns::NetworkParams::NetworkParams() {

	ULONG ulOutBufLen;
	DWORD dwRetVal;

	pFixedInfo = (FIXED_INFO *)MALLOC(sizeof(FIXED_INFO));
	if (pFixedInfo == NULL) {
		printf("Error allocating memory needed to call GetNetworkParams\n");
		return;
	}
	ulOutBufLen = sizeof(FIXED_INFO);

	if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pFixedInfo);
		pFixedInfo = (FIXED_INFO *)MALLOC(ulOutBufLen);
		if (pFixedInfo == NULL) {
			printf("Error allocating memory needed to call GetNetworkParams\n");
			return;
		}
	}

	if (dwRetVal = GetNetworkParams(pFixedInfo, &ulOutBufLen) != NO_ERROR) {
		printf("GetNetworkParams failed with error: %d\n", dwRetVal);
		return;
	}
}

dns::NetworkParams::~NetworkParams() {
	if (pFixedInfo) {
		FREE(pFixedInfo);
	}
}


std::vector<std::string> dns::NetworkParams::dns_servers() {
	
	std::vector<std::string> servers;

	// servers.push_back(pFixedInfo->DnsServerList.IpAddress.String);
	auto pIPAddr = pFixedInfo->DnsServerList.Next;
	while (pIPAddr) {
		servers.push_back(pIPAddr->IpAddress.String);
		pIPAddr = pIPAddr->Next;
	}
	
	return servers;
}
