#pragma once

#include <cstdint>

/*
	The header contains the following fields:

									1  1  1  1  1  1
	  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                      ID                       |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                    QDCOUNT                    |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                    ANCOUNT                    |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                    NSCOUNT                    |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                    ARCOUNT                    |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

ID              A 16 bit identifier assigned by the program that
				generates any kind of query.  This identifier is copied
				the corresponding reply and can be used by the requester
				to match up replies to outstanding queries.

QR              A one bit field that specifies whether this message is a
				query (0), or a response (1).

OPCODE          A four bit field that specifies kind of query in this
				message.  This value is set by the originator of a query
				and copied into the response.  The values are:

				0               a standard query (QUERY)

				1               an inverse query (IQUERY)

				2               a server status request (STATUS)

				3-15            reserved for future use

AA              Authoritative Answer - this bit is valid in responses,
				and specifies that the responding name server is an
				authority for the domain name in question section.

				Note that the contents of the answer section may have
				multiple owner names because of aliases.  The AA bit
				corresponds to the name which matches the query name, or
				the first owner name in the answer section.

TC              TrunCation - specifies that this message was truncated
				due to length greater than that permitted on the
				transmission channel.

RD              Recursion Desired - this bit may be set in a query and
				is copied into the response.  If RD is set, it directs
				the name server to pursue the query recursively.
				Recursive query support is optional.

RA              Recursion Available - this be is set or cleared in a
				response, and denotes whether recursive query support is
				available in the name server.

Z               Reserved for future use.  Must be zero in all queries
				and responses.

RCODE           Response code - this 4 bit field is set as part of
				responses.  The values have the following
				interpretation:

				0               No error condition

				1               Format error - The name server was
								unable to interpret the query.

				2               Server failure - The name server was
								unable to process this query due to a
								problem with the name server.

				3               Name Error - Meaningful only for
								responses from an authoritative name
								server, this code signifies that the
								domain name referenced in the query does
								not exist.

				4               Not Implemented - The name server does
								not support the requested kind of query.

				5               Refused - The name server refuses to
								perform the specified operation for
								policy reasons.  For example, a name
								server may not wish to provide the
								information to the particular requester,
								or a name server may not wish to perform
								a particular operation (e.g., zone
								transfer) for particular data.

				6-15            Reserved for future use.

QDCOUNT         an unsigned 16 bit integer specifying the number of
				entries in the question section.

ANCOUNT         an unsigned 16 bit integer specifying the number of
				resource records in the answer section.

NSCOUNT         an unsigned 16 bit integer specifying the number of name
				server resource records in the authority records
				section.

ARCOUNT         an unsigned 16 bit integer specifying the number of
				resource records in the additional records section.

*/

namespace dns {

	enum class Qr : uint8_t {
		REQUEST = 0,
		RESPONSE = 1,
	};

	enum class Opcode : uint8_t {
		QUERY = 0,
		IQUERY = 1,
		STATUS = 2,

		MAX_RESERVED = 15,
	};

	enum class Aa : uint8_t {
		DISABLE = 0,
		ENABLE = 1,
	};

	enum class Tc : uint8_t {
		DISABLE = 0,
		ENABLE = 1,
	};

	enum class Rd : uint8_t {
		DISABLE = 0,
		ENABLE = 1,
	};

	enum class Ra : uint8_t {
		DISABLE = 0,
		ENABLE = 1,
	};

	enum class Z : uint8_t {
		DISABLE = 0,
	};

	enum class Rcode : uint8_t {
		NO_ERROR = 0,
		FORMAT_ERROR = 1,
		SERVER_FAILURE = 2,
		NAME_ERROR = 3,
		NOT_IMPLEMENTED = 4,
		REFUSED = 5,

		MAX_RESERVED = 15,
	};

	struct parameter {
		Qr qr;			// 1 bit
		Opcode opcode;	// 4 bit
		Aa aa;			// 1 bit
		Tc tc;			// 1 bit
		Rd rd;			// 1 bit
		Ra ra;			// 1 bit
		Z z;			// 3 bit 		
		Rcode rcode;	// 4 bit
	};

	struct header {
		uint16_t id;		// 16 bit
		parameter param;	// 16 bit
		uint16_t qdcount;	// 16 bit
		uint16_t ancount;	// 16 bit
		uint16_t nscount;	// 16 bit
		uint16_t arcount;	// 16 bit
	};
}