#pragma once

#include "DnsQuery.h"
#include "DnsAnswer.h"

namespace dns {

	struct request {
		header header;
		std::vector<query> queries;

		char data[512];
		int length;
	};

	struct response {
		header header;
		std::vector<query> queries;
		std::vector<answer> answers;
		std::string server;
	};

	int create_request(std::string domain, request & message);
	int create_request(request & message, const header header, const std::vector<query> queries);

	int create_response(std::string & data, response & response);

}