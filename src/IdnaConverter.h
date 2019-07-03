#pragma once

#include <string>

namespace idna {

	std::string to_standardization(const std::string value);
	std::string to_internationalization(const std::string value);
}