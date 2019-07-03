#include <iostream>
#include <codecvt>
#include <exception>

#include "Dns.h"


std::string ws2s(const std::wstring& wstr) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}

void print_response(const dns::response & response) {

	std::cout << "Server:\t" << response.server << "\n\n";

	switch (response.header.param.aa) {
		case dns::Aa::DISABLE: std::cout << "Non-authoritative answer:" << "\n"; break;
		case dns::Aa::ENABLE:  std::cout << "Authoritative answer:"		<< "\n"; break;
	}

	for (const auto resp : response.answers) {
		switch (resp.atype) {
			case dns::Qtype::A: std::cout << "Name:\t" << resp.name << "\n" << "Addr:\t" << resp.address << "\n"; break;
			case dns::Qtype::CNAME: std::cout << resp.name << "\tcanonical name = " << resp.address << "\n"; break;
		}
	}
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[]) {
	std::string domain, server;

	switch (argc - 1) {
		case 2: server = ws2s(std::wstring(argv[2]));
		case 1: domain = ws2s(std::wstring(argv[1])); break;
		default: std::cerr << "No specified domain address." << std::endl; exit(-1);
	}

	try {
		const auto client = dns::Client::init(server);
		const auto response = client->send(domain);

		print_response(response);
	} catch (std::exception & e) {
		std::cout << e.what() << "\n";
	}
	
	return 0;
}