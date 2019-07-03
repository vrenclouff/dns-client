#include "DnsMessage.h"
#include "IdnaConverter.h"

#include <string>
#include <random>
#include <map>

#define DOMAIN_DELIMETER	'.'

std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> id_gen(1, UINT16_MAX);

struct domain_name {
	std::string name;
	int length;
};

std::map<uint16_t, domain_name> Saved_Names;

uint16_t generate_number() {
	return id_gen(rng);
}

dns::header default_header() {

	dns::parameter param;
	
	param.qr = dns::Qr::REQUEST;
	param.opcode = dns::Opcode::QUERY;
	param.aa = dns::Aa::DISABLE;
	param.tc = dns::Tc::DISABLE;
	param.rd = dns::Rd::ENABLE;
	param.ra = dns::Ra::DISABLE;
	param.z = dns::Z::DISABLE;
	param.rcode = dns::Rcode::NO_ERROR;

	dns::header header;

	header.id = 0b0;
	header.param = param;
	header.qdcount = 0b00;
	header.ancount = 0b00;
	header.nscount = 0b00;
	header.arcount = 0b00;

	return header;
}

dns::query default_query(const std::string domain) {

	dns::query query;

	query.domain.original_name = domain;
	query.qtype = dns::Qtype::A;
	query.qclass = dns::Qclass::IN;

	return query;
}

domain_name parse_name(std::string & data, const uint16_t startIndex, uint16_t length) {

	if (Saved_Names.find(startIndex) != Saved_Names.end()) {
		domain_name name = Saved_Names[startIndex];
		return name;
	}

	bool isLengthValid = static_cast<uint16_t>(-1) != length;

	domain_name result;

	uint16_t actualIndex = startIndex;
	uint8_t wordLength = data[actualIndex];
	uint8_t counter = 0;

	while (!isLengthValid && wordLength != 0x00 || (isLengthValid && counter < length)) {

		if ((wordLength >> 6) == 0b11) {
			uint16_t nameIndex = (static_cast<unsigned char>(data[actualIndex + 0]) << 8) | static_cast<unsigned char>(data[actualIndex + 1]);
			domain_name tempName = parse_name(data, nameIndex & 0x3FFF, -1);
			result.name.append(tempName.name).append(".");
			counter += 2;
			continue;
		}

		actualIndex += 1;
		counter += 1;

		for (char i = 0; i < wordLength; i++, actualIndex++) {
			result.name.append(1, data[actualIndex]);
			counter += 1;
		}

		result.name.append(1, '.');

		wordLength = data[actualIndex];
	}
	result.name.pop_back();

	result.length = actualIndex - startIndex;

	Saved_Names[startIndex] = result;

	return result;
}

std::string parse_answer_type_a(std::string & data, const uint16_t length, const uint8_t startIndex) {
	std::string address;
	for (uint16_t i = 0; i < length; i++) {
		address.append(std::to_string(static_cast<unsigned char>(data[startIndex + i]))).append(".");
	}
	address.pop_back();
	return address;
}

std::string parse_answer_type_cname(std::string & data, uint16_t length, uint8_t startIndex) {
	return parse_name(data, startIndex, length).name;
}

int dns::create_request(const std::string domain, request & message) {
	return create_request(message, default_header(), { default_query(domain) });
}

int dns::create_request(request & message, const header header, const std::vector<query> queries) {
	
	/* -------------------- HEADER -------------------------------*/

	message.header = header;

	if (header.id == 0) {
		message.header.id = generate_number();
	}

	message.header.qdcount = static_cast<decltype(header.qdcount)>(queries.size());

	message.data[0] = static_cast<char>(message.header.id >> 8);
	message.data[1] = message.header.id & 0xFF;

	char hParam1 = 0;
	hParam1 |= (static_cast<char>(message.header.param.rd)		& 0b0001) << 0;
	hParam1 |= (static_cast<char>(message.header.param.tc)		& 0b0001) << 1;
	hParam1 |= (static_cast<char>(message.header.param.aa)		& 0b0001) << 2;
	hParam1 |= (static_cast<char>(message.header.param.opcode)	& 0b1111) << 3;
	hParam1 |= (static_cast<char>(message.header.param.qr)		& 0b0001) << 7;
	message.data[2] = hParam1;

	char hParam2 = 0;
	hParam2 |= (static_cast<char>(message.header.param.rcode)	& 0b1111) << 0;
	hParam2 |= (static_cast<char>(message.header.param.z)		& 0b0111) << 4;
	hParam2 |= (static_cast<char>(message.header.param.ra)		& 0b0001) << 7;
	message.data[3] = hParam2;

	message.data[4]  = static_cast<char>(message.header.qdcount >> 8);
	message.data[5]  = message.header.qdcount & 0xFF;
	message.data[6]  = static_cast<char>(message.header.ancount >> 8);
	message.data[7]  = message.header.ancount & 0xFF;
	message.data[8]  = static_cast<char>(message.header.nscount >> 8);
	message.data[9]	= message.header.nscount & 0xFF;
	message.data[10] = static_cast<char>(message.header.arcount >> 8);
	message.data[11] = message.header.arcount & 0xFF;

	/* --------------------- QUERIES --------------------------------*/

	message.length = 0;
	uint16_t actualIndex = 12;

	for (auto query : queries) {

		message.queries.push_back(query);

		if (query.domain.original_name.length() == 0) {
			return -1;
		}

		if (query.domain.standard_name.length() == 0) {
			query.domain.standard_name = idna::to_standardization(query.domain.original_name);
		}

		char wordSize = 0;
		uint16_t wordSizeIndex = actualIndex;

		for (size_t i = 0; i < query.domain.standard_name.size(); i++, wordSize++) {
			char letter = query.domain.standard_name[i];

			if (letter == DOMAIN_DELIMETER) {
				message.data[wordSizeIndex] = wordSize;
				wordSizeIndex += wordSize + 1;
				wordSize = -1;
				continue;
			}

			message.data[wordSizeIndex + wordSize + 1] = letter;
		}

		if (query.domain.standard_name.size() == wordSize) {
			return -1;
		}

		message.data[wordSizeIndex] = wordSize;
		message.data[wordSizeIndex + wordSize + 1] = 0x00;

		uint16_t footerIndex = wordSizeIndex + wordSize + 2;

		message.data[footerIndex + 0] = static_cast<char>(static_cast<uint16_t>(query.qtype) >> 8);
		message.data[footerIndex + 1] = static_cast<uint16_t>(query.qtype) & 0xFF;
		message.data[footerIndex + 2] = static_cast<char>(static_cast<uint16_t>(query.qclass) >> 8);
		message.data[footerIndex + 3] = static_cast<uint16_t>(query.qclass) & 0xFF;

		message.length += footerIndex + 4;

		actualIndex = footerIndex + 4;
	}

	return 0;
}

int dns::create_response(std::string & data, response & response) {

	/* -------------------- HEADER -------------------------------*/

	dns::header header;
	header.id = (static_cast<unsigned char>(data[0]) << 8) | static_cast<unsigned char>(data[1]);;

	uint16_t param = (static_cast<unsigned char>(data[2]) << 8)  | static_cast<unsigned char>(data[3]);
	header.param.qr		= static_cast<dns::Qr>	  (param >> 15 & 0x1);
	header.param.opcode = static_cast<dns::Opcode>(param >> 11 & 0xF);
	header.param.aa		= static_cast<dns::Aa>	  (param >> 10 & 0x1);
	header.param.tc		= static_cast<dns::Tc>	  (param >> 9  & 0x1);
	header.param.rd		= static_cast<dns::Rd>	  (param >> 8  & 0x1);
	header.param.ra		= static_cast<dns::Ra>	  (param >> 7  & 0x1);
	header.param.z		= static_cast<dns::Z>	  (param >> 4  & 0x7);
	header.param.rcode	= static_cast<dns::Rcode> (param >> 0  & 0xF);

	if (header.param.rcode != dns::Rcode::NO_ERROR) {
		return -1;
	}


	header.qdcount = (static_cast<unsigned char>(data[4])  << 8) | static_cast<unsigned char>(data[5]);;
	header.ancount = (static_cast<unsigned char>(data[6])  << 8) | static_cast<unsigned char>(data[7]);;
	header.nscount = (static_cast<unsigned char>(data[8])  << 8) | static_cast<unsigned char>(data[9]);;
	header.arcount = (static_cast<unsigned char>(data[10]) << 8) | static_cast<unsigned char>(data[11]);

	response.header = header;

	uint8_t actualIndex = 12;

	/* -------------------- QUERIES -------------------------------*/

	if (response.header.qdcount == 0) {
		return -1;
	}

	response.queries.reserve(header.qdcount);

	for (uint8_t qdcount = 0; qdcount < header.qdcount; qdcount++) {
		dns::query query;

		domain_name nameRes = parse_name(data, actualIndex, -1);
		actualIndex += nameRes.length;

		if (data[actualIndex] != 0x00) {
			return -1;
		}
		actualIndex += 1;

		query.domain.standard_name = nameRes.name;
		query.domain.original_name = idna::to_internationalization(nameRes.name);
		query.qtype  = static_cast<dns::Qtype> ((static_cast<unsigned char>(data[actualIndex + 0]) << 8) | static_cast<unsigned char>(data[actualIndex + 1]));
		query.qclass = static_cast<dns::Qclass>((static_cast<unsigned char>(data[actualIndex + 2]) << 8) | static_cast<unsigned char>(data[actualIndex + 3]));

		response.queries.push_back(query);
	}

	/* -------------------- ANSWERS -------------------------------*/

	if (response.header.ancount == 0) {
		return -1;
	}

	response.answers.reserve(header.ancount);

	actualIndex += 4;
	while (actualIndex < data.length() - 1) {

		dns::answer answer;

		uint16_t nameIndex = (static_cast<unsigned char>(data[actualIndex + 0]) << 8) | static_cast<unsigned char>(data[actualIndex + 1]);
		answer.name = parse_name(data, nameIndex & 0x3FFF, -1).name;

		answer.atype = static_cast<dns::Qtype>  ((static_cast<unsigned char>(data[actualIndex + 2]) << 8) | static_cast<unsigned char>(data[actualIndex + 3]));
		answer.aclass = static_cast<dns::Qclass>((static_cast<unsigned char>(data[actualIndex + 4]) << 8) | static_cast<unsigned char>(data[actualIndex + 5]));

		answer.ttl = (static_cast<unsigned char>(data[actualIndex + 6]) << 24) | (static_cast<unsigned char>(data[actualIndex + 7]) << 16) |
				     (static_cast<unsigned char>(data[actualIndex + 8]) << 8)  | static_cast<unsigned char>(data[actualIndex + 9]);

		uint16_t length = (static_cast<unsigned char>(data[actualIndex + 10]) << 8) | static_cast<unsigned char>(data[actualIndex + 11]);

		actualIndex += 12;

		switch (answer.atype) {
			case dns::Qtype::A:		answer.address = parse_answer_type_a	(data, length, actualIndex); break;
			case dns::Qtype::CNAME: answer.address = parse_answer_type_cname (data, length, actualIndex); break;
		}

		actualIndex += length;

		response.answers.push_back(answer);
	}

	return 0;
}
