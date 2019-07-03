#pragma once

#include "DnsHeader.h"

#include <string>

/*

The question section is used to carry the "question" in most queries,
i.e., the parameters that define what is being asked.  The section
contains QDCOUNT (usually 1) entries, each of the following format:

									1  1  1  1  1  1
	  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                                               |
	/                     QNAME                     /
	/                                               /
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                     QTYPE                     |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                     QCLASS                    |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

where:

QNAME           a domain name represented as a sequence of labels, where
				each label consists of a length octet followed by that
				number of octets.  The domain name terminates with the
				zero length octet for the null label of the root.  Note
				that this field may be an odd number of octets; no
				padding is used.

QTYPE           a two octet code which specifies the type of the query.
				The values for this field include all codes valid for a
				TYPE field, together with some more general codes which
				can match more than one type of RR.

QCLASS          a two octet code that specifies the class of the query.
				For example, the QCLASS field is IN for the Internet.
*/

namespace dns {

	enum class Qtype : uint16_t {
		A = 1,
		NS = 2,
		CNAME = 5,
		SOA = 6,
		WKS = 11,
		PTR = 12,
		MX = 15,
		SRC = 33,
		AAAA = 28,

		MAX_RESERVED = 255,
	};

	enum class Qclass : uint16_t {
		IN = 1,	// Internet 
		CH = 3, // Chaos
		HS = 4, // Hesiod

		MAX_RESERVED = 255,
	};

	struct domain {
		std::string original_name;
		std::string standard_name;
	};

	struct query {
		domain domain;
		Qtype qtype;		// 16 bit
		Qclass qclass;		// 16 bit
	};
}