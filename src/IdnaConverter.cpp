#include "IdnaConverter.h"

#include "Windows.h"
#include <codecvt>
#include <string>

std::string idna::to_standardization(const std::string value) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	std::wstring domain = myconv.from_bytes(value);
	wchar_t lpASCIICharStr[253];
	int len = IdnToAscii(IDN_ALLOW_UNASSIGNED, domain.c_str(), domain.size(), lpASCIICharStr, 253);
	std::wstring tstr(lpASCIICharStr, len);
	return std::string(tstr.begin(), tstr.end());
}

std::string idna::to_internationalization(const std::string value) {
	return value;
}

